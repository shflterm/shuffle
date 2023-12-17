#include "shflai.h"

#include <llama.h>
#include <common/common.h>
#include <memory>

using std::shared_ptr;

string replaceAll(string str, const string&from, const string&to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

namespace shflai {
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

    void generateResponse(const string&prompt, const string&instruction) {
        gpt_params params;
        params.prompt = instruction +
                        "[INST]" + prompt + "[/INST]";
        params.sparams.penalty_repeat = 1.1f;
        params.sparams.temp = 0.7f;

        const int n_len = 100;

        if (model == nullptr) {
            fprintf(stderr, "%s: AI not loaded\n", __func__);
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
        }

        // main loop

        int n_cur = batch.n_tokens;
        int n_decode = 0;

        const auto t_main_start = ggml_time_us();

        string detectedEndSign;

        std::cout << "\033[33mGenerating response, this may take a few seconds. (Almost done!)\033[0m" << std::endl;
        bool codeOpened = false;
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
                    LOG_TEE("\n");
                    break;
                }

                string next = llama_token_to_piece(ctx, new_token_id);
                if (next == "</") detectedEndSign = "</";
                else if (detectedEndSign == "</" && next == "s") detectedEndSign = "</s";
                else if (detectedEndSign == "</s" && next == ">") break;
                else {
                    detectedEndSign = "";

                    string newRes;
                    for (char ch: next) {
                        if (ch == '`') {
                            if (codeOpened) newRes += "\033[0m";
                            else newRes += "\033[100m";

                            codeOpened = !codeOpened;
                        }
                        else newRes += ch;
                    }
                    LOG_TEE("%s", newRes.c_str());
                }

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
                return;
            }
        }
    }
}
