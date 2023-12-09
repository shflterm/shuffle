#include "ai/genllama.h"

#include <llama.h>
#include <common/common.h>

#include <utility>
#include <utils/console.h>
#include <utils/utils.h>

llama_model* model;

bool loadAiModel(const string&modelPath) {
    llama_backend_init(false);
    llama_model_params model_params = llama_model_default_params();

    model_params.n_gpu_layers = 150; // offload all layers to the GPU

    model = llama_load_model_from_file(modelPath.c_str(), model_params);
    if (model == nullptr) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        return false;
    }
    return true;
}

string promptTemplete = "<|im_start|>system"
        "{SYSTEM}<|im_end|>"
        "<|im_start|>user"
        "{USER}<|im_end|>"
        "<|im_start|>assistant";

string systemPrompt =
        "You are a robot that helps you easily use the shell called 'Shuffle'. Shuffle can only use Shuffle's special commands. Let me show you Shuffle command help."
        "{DOCS}"
        "Based on this article, we analyze your question and answer only the commands you need, in one line. The command must be complete, including all arguments. The answer can be written as a written command or as a variable declaration (when time permits). Afterwards, you should check the help to see if the command exists.) The answer must be wrapped in ` (e.g. `help`) The ANSWER must contain the command and the arguments. Or it can also declare a variable. The arguments must be known. If not, please contact the user again."
        "I'll teach you how to use variables. To assign a value to a variable, use \"VAR_NAME = VALUE\". You can enter text, variables, or commands in VALUE. Text can be executed as \"TEXT\". Quotes Without it, variables are displayed as \"$VAR_NAME\" and commands are displayed as \"(COMMAND)!\" (Commands can be used by enclosing their arguments in parentheses!) For example, \"a = text\" (save \"text in a variable called a)\", \"b = $a\" (store the value of variable a in b), \"c = (capitalize $a)! \"(execute $a in uppercase and store result in c), etc.";

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
    params.prompt = promptTemplete;
    params.prompt = replace(params.prompt, "{SYSTEM}", replace(systemPrompt, "{DOCS}", docs));
    params.prompt = replace(params.prompt, "{USER}", prompt);


    const int n_len = 100;

    if (model == nullptr) {
        fprintf(stderr, "%s: AI not loaded\n", __func__);
        return "AI NOT LOADED";
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
    llama_batch batch = llama_batch_init(2048, 0, 1);
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

    warning("Generating response, this may take a few seconds. (Almost done!)");
    string res = "";
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
                res += "\n";
                break;
            }

            string next = llama_token_to_piece(ctx, new_token_id);
            if (next == "<|im_end|>") break;

            next = replace(next, "}", reset);
            next = replace(next, "{", bgb_black);
            res += next;

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
    return res;
}
