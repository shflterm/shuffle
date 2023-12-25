//
// Created by 08sih on 2023-12-20.
//

#ifndef SHLFAI_H
#define SHLFAI_H

#include <string>
#include <utility>
#include <vector>
#include <map>

#include "cmd/commandmgr.h"

using std::string, std::vector, std::pair, std::map,
        cmd::Command;

class parameter_property {
public:
    string type;
    string description;
    bool required;

    parameter_property(string type, string description, const bool required)
        : type(std::move(type)),
          description(std::move(description)), required(required) {
    }
};

class function_parameter {
public:
    string type;
    map<string, parameter_property> properties;

    function_parameter(string type, const map<string, parameter_property>&properties)
        : type(std::move(type)),
          properties(properties) {
    }
};

class function_declaration {
public:
    string name;
    string description;
    function_parameter parameters;

    function_declaration(string name, string description, function_parameter parameters)
        : name(std::move(name)),
          description(std::move(description)),
          parameters(std::move(parameters)) {
    }
};

function_declaration createFunctionDeclarationByCommand(const Command&command);

class shflai_response {
public:
    enum response_type {
        TEXT,
        COMMAND
    };

    response_type type;
    string result_str;

    shflai_response(const response_type type, string  result_str)
        : type(type),
          result_str(std::move(result_str)) {
    }
};

shflai_response generateResponse(Workspace* workspace, const string&prompt);

#endif //SHLFAI_H
