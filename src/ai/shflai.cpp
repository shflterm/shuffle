#include "ai/shlfai.h"
#include "gemini.h"

#include <string>
#include <vector>

#include <json/json.h>
#include "appmgr/appmgr.h"
#include "restclient-cpp/restclient.h"

using std::string, std::vector, std::pair, std::endl;

vector<pair<string, string>> chatLogs;

function_declaration createFunctionDeclarationByCommand(const shared_ptr<Command>&command) {
    map<string, parameter_property> properties;
    for (const auto&option: command->getOptions()) {
        properties.emplace(option.name, parameter_property("string", option.description, true));
    }
    const function_parameter parameter = {"object", properties};

    std::stringstream ss;
    ss << command->getDescription() << " / ";
    ss << "Usage: " << command->getUsage() << " / ";
    ss << "Examples: ";
    for (const auto&example: command->getExamples())
        ss << "`" << example.command << "` : " << example.whatItDoes << ", ";

    std::vector<shared_ptr<Command>> commandHierarchy;

    auto currentCommand = command;
    while (currentCommand) {
        commandHierarchy.push_back(currentCommand);
        currentCommand = currentCommand->parent;
    } //TODO : parent 한개만 됨.

    std::reverse(commandHierarchy.begin(), commandHierarchy.end());

    string name;
    for (const auto&p: commandHierarchy) name += p->getName() + "_";
    name = name.substr(0, name.size() - 1);
    function_declaration declaration = {name, ss.str(), parameter};
    return declaration;
}

shflai_response generateResponse(Workspace* workspace, const string&prompt) {
    if (prompt.empty()) {
        return {shflai_response::TEXT, "Failed to generate response. (prompt is empty)"};
    }

    chatLogs.emplace_back("user", prompt);

    Json::Value requestBody;
    Json::Value contents;
    for (const auto&[role, text]: chatLogs) {
        Json::Value content;
        content["role"] = role;
        Json::Value parts;
        Json::Value part;
        part["text"] = text;
        parts.append(part);
        content["parts"] = parts;
        contents.append(content);
    }
    requestBody["contents"] = contents;
    chatLogs.pop_back();

    vector<function_declaration> functionsDeclarations;
    std::deque<shared_ptr<Command>> cmds;
    for (const auto&command: appmgr::getCommands()) cmds.push_back(command);
    while (!cmds.empty()) {
        const auto currentCommand = cmds.front();
        cmds.pop_front();

        functionsDeclarations.push_back(createFunctionDeclarationByCommand(currentCommand));

        for (const auto&subcommand: currentCommand->getSubcommands()) {
            cmds.push_back(subcommand);
        }
    }

    Json::Value tools;
    Json::Value tool;
    Json::Value function_declarations;
    for (const auto&functionDeclaration: functionsDeclarations) {
        Json::Value function_declaration;
        function_declaration["name"] = functionDeclaration.name;
        function_declaration["description"] = functionDeclaration.description;

        Json::Value parameters(Json::objectValue);
        parameters["type"] = "object";

        Json::Value properties(Json::objectValue);
        for (const auto&[name, property]: functionDeclaration.parameters.properties) {
            Json::Value parameter;
            parameter["type"] = property.type;
            parameter["description"] = property.description;
            properties[name] = parameter;
        }
        parameters["properties"] = properties;

        Json::Value required_parameters(Json::arrayValue);
        for (const auto&[name, property]: functionDeclaration.parameters.properties) {
            if (property.required) required_parameters.append(name);
        }
        parameters["required"] = required_parameters;

        function_declaration["parameters"] = parameters;

        function_declarations.append(function_declaration);
    }
    tool["function_declarations"] = function_declarations;
    tools.append(tool);
    requestBody["tools"] = tools;

    Json::FastWriter fastWriter;
    std::string requestBodyContent = fastWriter.write(requestBody);

    auto [code, body, headers] = RestClient::post(
        "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key=" + string(
            GEMINI_API_KEY),
        "application/json", requestBodyContent);
    Json::Value responseRoot;
    Json::Reader responseReader;
    responseReader.parse(body, responseRoot, false);
    for (auto part: responseRoot["candidates"][0]["content"]["parts"]) {
        if (part["text"].isString()) {
            string result = part["text"].asString();
            chatLogs.emplace_back("user", prompt);
            chatLogs.emplace_back("model", result);
            return {shflai_response::TEXT, result};
        }
        if (part["functionCall"].isObject()) {
            Json::Value func = part["functionCall"];
            string funcName = func["name"].asString();
            funcName = replace(funcName, "_", " ");
            vector<string> cmdName = splitBySpace(funcName);

            std::stringstream command;
            command << funcName << " ";
            for (const auto& key : func["args"].getMemberNames()) {
                command << "-" << key << " \"" << func["args"][key].asString() << "\" ";
            }


            return {shflai_response::COMMAND, command.str()};
        }
        return {shflai_response::TEXT, "Sorry, AI failed to generate response. please try again."};
    }
    return {shflai_response::TEXT, "Failed to generate response. (response: " + body + ")"};
}
