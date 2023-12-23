#include "ai/shlfai.h"
#include "gemini.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmd/builtincmd.h>

#include <json/json.h>
#include "appmgr/appmgr.h"
#include "restclient-cpp/restclient.h"

using std::string, std::vector, std::pair, std::cout, std::endl;

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
    }

    std::reverse(commandHierarchy.begin(), commandHierarchy.end());

    string name;
    for (const auto&p: commandHierarchy) name += p->getName() + "_";
    name = name.substr(0, name.size() - 1);
    function_declaration declaration = {name, ss.str(), parameter};
    return declaration;
}

vector<pair<string, string>> createChats(const shared_ptr<Command>&command) {
    vector<pair<string, string>> chats;
    chats.emplace_back("user", "How to store a value in a variable 'var'?");
    chats.emplace_back("model", "Try this: { var = [VALUE] }");
    chats.emplace_back("user", "How to store the result of the command 'help shfl' in the variable 'res'?");
    chats.emplace_back("model", "Try this: { res = (help shfl)! }");
    chats.emplace_back("user", "Save the value entered from the console in a variable called 'res'.");
    chats.emplace_back("model", "Try this: { res = (input)! }");
    chats.emplace_back("user", "How to use the value of a variable 'var'?");
    chats.emplace_back("model", "Try this: { $var } (Example: { echo $var })");
    chats.emplace_back("user", "How to run basic commands on the OS?");
    chats.emplace_back("model", "Try this: { & <os command> } (Example: { & help })");
    chats.emplace_back("user", "How to run commands in cmd, bash, and terminal?");
    chats.emplace_back("model", "Try this: { & <os command> } (Example: { & help })");
    chats.emplace_back("user", "How to move to another workspace?");
    chats.emplace_back("model", "Try this: { @ <workspace name> } (Example: { @ main }, { @ run_download })");
    chats.emplace_back("user", "How do I ask AI a question?");
    chats.emplace_back(
        "model",
        "Try this: { # <ai prompt> } (Example: { # How to download apps from the new repository? }, { # Download 'example_app' from 'https://example.com/shflrepo.json'. })");

    for (const auto&example: command->getExamples()) {
        chats.emplace_back("user", example.whatItDoes);
        chats.emplace_back("model", "Try this command: { " + example.command + " }");
    }
    for (const auto&subcommand: command->getSubcommands()) {
        for (const auto&inst: createChats(subcommand))
            chats.push_back(inst);
    }

    for (const auto&cha: chatLogs) chats.push_back(cha);

    return chats;
}

string generateResponse(Workspace* workspace, const string&prompt) {
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
    cout << requestBody.toStyledString();

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

            result = replace(result, "{", "\033[100m");
            result = replace(result, "}", "\033[0m");
            return result;
        }
        if (part["functionCall"].isObject()) {
            Json::Value func = part["functionCall"];
            string funcName = func["name"].asString();
            funcName = replace(funcName, "_", " ");
            vector<string> cmdName = splitBySpace(funcName);
            cout << func["name"];
            shared_ptr<Command> cmd = cmd::findCommand(cmdName.front());
            if (cmd == nullptr) {
                return "Sorry, AI failed to find the command. please try again.";
            }
            for (int i = 1; i < cmdName.size(); ++i) {
                if (cmd == nullptr) break;
                cmd = findCommand(cmdName[i], cmd->getSubcommands());
            }

            std::stringstream command;
            command << funcName << " ";
            for (const auto&option: cmd->getOptions()) {
                if (func["args"][option.name].isNull()) continue;
                command << "-" << option.name << " \"" << func["args"][option.name].asString() << "\" ";
            }

            chatLogs.emplace_back("model", command.str());
            return command.str();
        }
        chatLogs.emplace_back("model", part.toStyledString());
        return part.toStyledString();
    }
    chatLogs.emplace_back("model", "Failed to generate response. (response: " + body + ")");
    return "Failed to generate response. (response: " + body + ")";
}
