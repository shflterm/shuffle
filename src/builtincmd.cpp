#include "builtincmd.h"

#include <memory>

#include "term.h"
#include "console.h"
#include "downloader.h"
#include "credit.h"
#include "utils.h"
#include "version.h"
#include "snippetmgr.h"
#include "sapp.h"
#include "automation.h"

using std::make_shared;

void loadCommands() {
    clearCommands();

    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "shfl", "Shuffle Command", {
                    CommandOption("reload", BOOL_T, {"rl"}),
                    CommandOption("update", BOOL_T, {"upgrade"}),
                    CommandOption("credits", BOOL_T, {"credit"}),
            }, shflCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "appmgr", "App Manager", {
                    CommandOption("add", TEXT_T, {"ad", "a"}),
                    CommandOption("remove", TEXT_T, {"rm", "r"}),
            }, appMgrCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "help", "Show help", {
                    CommandOption("command", TEXT_T, {"cmd", "help"})
            }, helpCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "snf", "Manage Snippets", {
                    CommandOption("create", TEXT_T, {"mk", "c", "new"}),
                    CommandOption("value", TEXT_T, {"v"}),
            }, snippetCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "auto", "Manage Automation", {
                    CommandOption("record", TEXT_T, {"create"}),
                    CommandOption("finish", BOOL_T, {"end", "stop"}),
                    CommandOption("run", TEXT_T, {"execute"}),
            }, automationCmd
    )));

    for (const CommandData &command: getRegisteredCommands()) {
        loadApp(command);
    }
}

void shflCmd(Workspace &ws, map<string, string> &optionValues) {
    if (optionValues["reload"] == "true") {
        info("Reloading command...");

        loadCommands();

        success("Reloaded all commands!");
    } else if (optionValues["apps"] == "true") {
        if (optionValues.count("add")) {
            addSAPP(optionValues["add"]);
        } else if (optionValues.count("remove")) {
            removeSAPP(optionValues["remove"]);
        }
    } else if (optionValues["update"] == "true") {
        if (checkUpdate(false)) updateShuffle();
    } else if (optionValues["credits"] == "true") {
        term << createCreditText();
    }
}

void appMgrCmd(Workspace &ws, map<string, string> &optionValues) {
    if (optionValues.count("add")) {
        addSAPP(optionValues["add"]);
    } else if (optionValues.count("remove")) {
        removeSAPP(optionValues["remove"]);
    }
}

void helpCmd(Workspace &ws, map<string, string> &optionValues) {
    if (optionValues.count("command") == 0) {
        term << "== Shuffle Help ==" << newLine
             << "Version: " << SHUFFLE_VERSION.str() << newLine << newLine
             << "Commands: " << newLine;
        for (const auto &item: commands) {
            auto command = *item;
            if (command.getDescription() != "-") {
                term << "  " << command.getName() << " : " << command.getDescription()
                     << newLine;
            }
        }
        term
                << newLine << "Additional Help: " << newLine
                << "  For more information on a specific command, type 'help <command>'" << newLine
                << "  Visit the online documentation for Shuffle at "
                   "https://github.com/shflterm/shuffle/wiki." << newLine << newLine;

        term << "Thanks to: " << color(BACKGROUND, Green) << "shfl credits" << resetColor << newLine;
    } else {
        string cmdName = optionValues["command"];
        shared_ptr<Command> cmd = findCommand(cmdName);
        if (cmd == nullptr) {
            term << "Command '" << cmdName << "' not found." << newLine;
            return;
        }

        term << "== About '" << cmd->getName() << "' ==" << newLine
             << "Name: " << cmd->getName() << newLine
             << "Description: " << cmd->getDescription() << newLine;

        if (!cmd->getUsage().empty()) term << "Usage: " << newLine;
        for (const auto &item: cmd->getUsage()) {
            string example = item.first;
            string description = item.second;
            term << "  " << example << " : " << description << newLine;
        }
    }
}

void snippetCmd(Workspace &ws, map<string, string> &optionValues) {
    //snf create aa help cd
    string snippetName = optionValues["create"];
    string cmd = optionValues["value"];

    addSnippet(snippetName, cmd);
    term << "Snippet Created: " << snippetName << " => " << cmd << newLine;
}

void automationCmd(Workspace &ws, map<string, string> &optionValues) {
    if (optionValues.count("record")) {
        string name = optionValues["record"];
        term << "Automation recording has started!" << newLine
             << "Automation is created with the commands you type forward." << newLine
             << "You can stop recording by typing \"auto stop\"" << newLine;
        startRecording(name);
    } else if (optionValues["finish"] == "true") {
        recordingCommands.pop_back();
        term << "Automation recorded successfully!" << newLine
             << "An Automation named '" + recordingName + "' was created." << newLine
             << "You can run it by typing \"auto -run " + recordingName + "\"." << newLine;
        stopRecording();
    } else if (optionValues.count("run")) {
        string name = optionValues["run"];
        runAutomation(ws, name);
    }
}

void BuiltinCommand::run(Workspace &ws, map<string, string> &optionValues) const {
    cmd(ws, optionValues);
}
