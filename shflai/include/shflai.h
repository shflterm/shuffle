//
// Created by 08sih on 2023-12-06.
//

#ifndef GENLLAMA_CPP_H
#define GENLLAMA_CPP_H

#include <string>

using std::string;

namespace shflai {
    bool loadAiModel(const string&modelPath);

    string generateResponse(const string&prompt, const string& docs);
}

#endif //GENLLAMA_CPP_H
