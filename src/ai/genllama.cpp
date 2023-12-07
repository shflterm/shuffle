#include "ai/genllama.h"

#include <llama.h>
#include <common/common.h>

#include <utility>
#include <utils/console.h>
#include <utils/utils.h>

llama_model* model;

void loadAiModel(const string&modelPath) {
    llama_backend_init(false);
    llama_model_params model_params = llama_model_default_params();

    model_params.n_gpu_layers = 150; // offload all layers to the GPU

    model = llama_load_model_from_file(modelPath.c_str(), model_params);
    if (model == nullptr) fprintf(stderr, "%s: error: unable to load model\n", __func__);
}

string systemPrompt =
        "You are an AI built into a shell program called 'Shuffle'. Basic Linux commands cannot be used."
        "However, users want it to be concise, so please refrain from providing additional explanations. "
        "If you think the user wants an exact command (complete with arguments), please provide it. "
        "When YOU(without INST) finish answering, write \"END\"\n\n"
        "Here's an example of your response:\n"
        "Here's how to {}:\n"
        "<{COMMAND}>\n"
        " - {a concise description of the command}\n";

string writeDocs(const shared_ptr<cmd::Command>&command, const string&prefix = "") {
    string docs;
    string examples;
    for (const auto&example: command->getExamples()) examples += example + ", ";
    docs += prefix + command->getName() + " - " + command->getDescription() + " / Usage: " + command->getUsage() +
            " / Examples: " + examples
            + "\n";
    for (const auto&subcommand: command->getSubcommands()) {
        docs += writeDocs(subcommand, prefix + command->getName() + " ");
    }
    return docs;
}

string generateResponse(const string&prompt) {
    string docs;
    for (const auto&command: cmd::commands) {
        docs += writeDocs(command);
    }

    gpt_params params;
    params.prompt = "[INST] <<SYS>>"
                    "Here is docs: " + docs + "\n\n" +
                    systemPrompt +
                    "<</SYS>>" +
                    prompt + "[/INST] END";

    const int n_len = 100;

    if (model == nullptr) {
        fprintf(stderr, "%s: AI not loaded\n", __func__);
        return "ERROR";
    }

    llama_context_params ctx_params = llama_context_default_params();

    ctx_params.n_ctx = 2048;
    ctx_params.n_batch = 2048;
    ctx_params.n_threads = get_num_physical_cores();
    ctx_params.n_threads_batch = get_num_physical_cores();

    llama_context* ctx = llama_new_context_with_model(model, ctx_params);

    if (ctx == nullptr) fprintf(stderr, "%s: error: failed to create the llama_context\n", __func__);
    // tokenize the prompt

    std::vector<llama_token> tokens_list;
    tokens_list = llama_tokenize(ctx, params.prompt, true);

    const int n_ctx = llama_n_ctx(ctx);
    const int n_kv_req = tokens_list.size() + (n_len - tokens_list.size());

    // make sure the KV cache is big enough to hold all the prompt and generated tokens
    if (n_kv_req > n_ctx) {
        LOG_TEE("%s: error: n_kv_req > n_ctx, the required KV cache size is not big enough\n", __func__);
        LOG_TEE("%s:        either reduce n_len or increase n_ctx\n", __func__);
        return "ERROR";
    }

    // create a llama_batch with size 512
    // we use this object to submit token data for decoding
    llama_batch batch = llama_batch_init(1024, 0, 1);
    // evaluate the initial prompt
    for (size_t i = 0; i < tokens_list.size(); i++) {
        llama_batch_add(batch, tokens_list[i], i, {0}, false);
    }

    // llama_decode will output logits only for the last token of the prompt
    batch.logits[batch.n_tokens - 1] = true;

    if (llama_decode(ctx, batch) != 0) {
        LOG_TEE("%s: llama_decode() failed\n", __func__);
        return "ERROR";
    }

    // main loop

    int n_cur = batch.n_tokens;
    int n_decode = 0;

    const auto t_main_start = ggml_time_us();

    std::cout << erase_line;
    while (true) {
        // sample the next token
        {
            auto n_vocab = llama_n_vocab(model);
            auto* logits = llama_get_logits_ith(ctx, batch.n_tokens - 1);

            std::vector<llama_token_data> candidates;
            candidates.reserve(n_vocab);

            for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
                candidates.emplace_back(llama_token_data{token_id, logits[token_id], 0.0f});
            }

            llama_token_data_array candidates_p = {candidates.data(), candidates.size(), false};

            // sample the most likely token
            const llama_token new_token_id = llama_sample_token_greedy(ctx, &candidates_p);

            // is it an end of stream?
            if (new_token_id == llama_token_eos(model) || n_cur == n_len) {
                break;
            }

            string next = llama_token_to_piece(ctx, new_token_id).c_str();
            next = replace(next, ">", reset);
            next = replace(next, "<", bgb_black);
            LOG_TEE("%s", next.c_str());

            // prepare the next batch
            llama_batch_clear(batch);


            // push this new token for next evaluation
            llama_batch_add(batch, new_token_id, n_cur, {0}, true);

            n_decode += 1;
        }

        n_cur += 1;

        // evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch)) {
            fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);
            return "ERROR";
        }
    }
}
