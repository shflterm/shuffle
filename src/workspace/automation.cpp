//
// Created by 08sih on 6/25/2023.
//

#include "automation.h"

#include <utility>
#include "json/json.h"

#include "utils.h"
#include "workspace.h"
#include "term.h"

using std::vector, std::make_shared;

vector<shared_ptr<Automation>> automations;
bool recordingAutomation;
vector<string> recordingCommands;
string recordingName;

Automation::Automation(string name, vector<string> commands) : name(std::move(name)), commands(std::move(commands)) {}

const string &Automation::getName() const {
    return name;
}

const vector<string> &Automation::getCommands() const {
    return commands;
}

vector<Automation> getAutomations() {
    vector<Automation> res;

    Json::Value automationList = getShflJson("automations");
    for (auto automation: automationList) {
        Json::Value commands = automation["commands"];
        vector<string> commandsVector;
        for (const auto &command: commands) commandsVector.push_back(command.asString());

        Automation data = Automation(automation["name"].asString(), commandsVector);
        res.push_back(data);
    }

    return res;
}

bool addAutomation(const string &name, const vector<string> &commands) {
    automations.push_back(make_shared<Automation>(Automation(name, commands)));

    Json::Value snippetsJson = getShflJson("automations");
    Json::Value newAutomation;
    newAutomation["name"] = name;
    for (const auto &item: commands) newAutomation["commands"].append(item);

    snippetsJson.append(newAutomation);
    setShflJson("automations", snippetsJson);
    return true;
}

void loadAutomations() {
    automations.clear();

    for (const Automation &command: getAutomations()) {
        automations.push_back(make_shared<Automation>(command));
    }
}

void startRecording(const string &name) {
    recordingAutomation = true;
    recordingName = name;
}

void stopRecording() {
    recordingAutomation = false;
    addAutomation(recordingName, recordingCommands);
    recordingCommands.clear();
    recordingName = "";
}

void runAutomation(Workspace &ws, const string &name) {
    for (const auto &item: automations) {
        if (item->getName() == name) {
            for (const auto &command: item->getCommands()) {
                term << color(FOREGROUND, Yellow) << "\u2192 " << resetColor << command << newLine;
                ws.execute(command);
                term << newLine;
            }
        }
    }
}