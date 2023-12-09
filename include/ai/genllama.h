//
// Created by 08sih on 2023-12-06.
//

#ifndef GENLLAMA_CPP_H
#define GENLLAMA_CPP_H

#include <string>

using std::string;

bool loadAiModel(const string& modelPath);

string generateResponse(const string&prompt);

#endif //GENLLAMA_CPP_H
