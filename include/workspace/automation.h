//
// Created by 08sih on 6/25/2023.
//

#ifndef SHFL_AUTOMATION_H
#define SHFL_AUTOMATION_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "workspace.h"

using std::string, std::vector, std::shared_ptr;

class Automation {
protected:
    string name;
    vector<string> commands;

public:
    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const vector<string> &getCommands() const;

    Automation(string name, vector<string> commands);
};

extern vector<shared_ptr<Automation>> automations;
extern bool recordingAutomation;
extern vector<string> recordingCommands;
extern string recordingName;

vector<Automation> getAutomations();

bool addAutomation(const string &name, const vector<string> &commands);

void loadAutomations();

void startRecording(const string &name);

void stopRecording();

void runAutomation(Workspace &ws, const string& name);

#endif //SHFL_AUTOMATION_H
