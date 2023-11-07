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

using std::make_shared;

void loadCommands() {
    clearCommands();

    commands.push_back(make_shared<Command>(Command(
            "shfl", "Shuffle Command", {
                    make_shared<Command>(Command("reload", "Reload all commands.", shflReloadCmd)),
                    make_shared<Command>(Command("upgrade", "Upgrade Shuffle to a new version.", shflUpgradeCmd)),
                    make_shared<Command>(Command("credits", "Shuffle Credits", shflCreditsCmd)),
            }, shflCmd
    )));
    commands.push_back(make_shared<Command>(Command(
            "appmgr", "App Manager", {
                    make_shared<Command>(Command("add", "Get new apps from the repository.", {
                            CommandOption("app", TEXT_T)
                        }, appMgrAddCmd)),
                    make_shared<Command>(Command("remove", "Delete the app from your device.", {
                            CommandOption("app", TEXT_T)
                        }, appMgrRemoveCmd)),
            }, appMgrCmd
    )));
    commands.push_back(make_shared<Command>(Command(
            "help", "Show help", {
                    CommandOption("command", TEXT_T, {"cmd", "help"})
            }, helpCmd
    )));
    commands.push_back(make_shared<Command>(Command(
            "snf", "Manage Snippets", {
                    CommandOption("create", TEXT_T, {"mk", "c", "new"}),
                    CommandOption("value", TEXT_T, {"v"}),
            }, snippetCmd
    )));

    for (const CommandData &command: getRegisteredCommands()) {
        loadApp(command);
    }
}

void shflReloadCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    info("Reloading command...");
    loadCommands();
    success("Reloaded all commands!");
}

void shflUpgradeCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    if (checkUpdate(false)) updateShuffle();
}

void shflCreditsCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    term << createCreditText();
}

void shflCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    // TODO: Print How to use
}

void appMgrAddCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    addSAPP(optionValues["app"]);
}

void appMgrRemoveCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    removeSAPP(optionValues["app"]);
}

void appMgrCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    // TODO: Print How to use
}

void helpCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
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
    }
}

void snippetCmd([[maybe_unused]] Workspace &ws, map<string, string> &optionValues) {
    //snf create aa help cd
    string snippetName = optionValues["create"];
    string cmd = optionValues["value"];

    addSnippet(snippetName, cmd);
    term << "Snippet Created: " << snippetName << " => " << cmd << newLine;
}