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
    chats.emplace_back("model", "Try this: { # <ai prompt> } (Example: { # How to download apps from the new repository? }, { # Download 'example_app' from 'https://example.com/shflrepo.json'. })");

    for (const auto&example: command->getExamples()) {
        chats.emplace_back("user", example.whatItDoes);
        chats.emplace_back("model", "Try this command: { " + example.command + " }");
    }
    for (const auto&subcommand: command->getSubcommands()) {
        for (const auto&inst: createChats(subcommand))
            chats.push_back(inst);
    }

    for (const auto& cha : chatLogs) chats.push_back(cha);

    return chats;
}

string generateResponse(const string& prompt) {
    vector<pair<string, string>> chats;
    for (const auto&command: appmgr::getCommands())
        for (const auto&inst: createChats(command))
            chats.push_back(inst);

    chats.emplace_back("user", prompt);

    Json::Value requestBody;
    Json::Value contents;
    for (const auto& [role, text] : chats) {
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

    Json::FastWriter fastWriter;
    std::string requestBodyContent = fastWriter.write(requestBody);
    auto [code, body, headers] = RestClient::post("https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key=" + string(GEMINI_API_KEY),
                                                  "application/json", requestBodyContent);
    Json::Value responseRoot;
    Json::Reader responseReader;
    responseReader.parse(body, responseRoot, false);
    string result = responseRoot["candidates"][0]["content"]["parts"][0]["text"].asString();
    chatLogs.emplace_back("user", prompt);
    chatLogs.emplace_back("model", result);

    result = replace(result, "{", "\033[100m");
    result = replace(result, "}", "\033[0m");
    return result;
}
