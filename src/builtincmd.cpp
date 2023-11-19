#include "builtincmd.h"

#include <iostream>
#include <memory>

#include "console.h"
#include "downloader.h"
#include "credit.h"
#include "utils.h"
#include "version.h"
#include "snippetmgr.h"
#include "appmgr.h"

using std::cout, std::endl, std::make_shared;

string shflReloadCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    if (!backgroundMode) info("Reloading command...");
    loadCommands();
    loadSnippets();
    if (!backgroundMode) success("Reloaded all commands!");
    return "true";
}

string shflUpgradeCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    if (checkUpdate(false)) {
        updateShuffle();
        return "true";
    }
    return "false";
}

string shflCreditsCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    if (!backgroundMode) cout << createCreditText();
    return "thanks";
}

string shflCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    // TODO: Print How to use
    return "true";
}

string appMgrAddCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    return installApp(optionValues["app"]) ? "true" : "false";
}

string appMgrRemoveCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    return removeApp(optionValues["app"]) ? "true" : "false";
}

string appMgrRepoAddCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    return addRepo(optionValues["repo"]) ? "true" : "false";
}

string appMgrRepoRemoveCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    return removeRepo(optionValues["repo"]) ? "true" : "false";
}

string appMgrCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    // TODO: Print How to use
    return "true";
}

string helpCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    if (optionValues.count("command") == 0) {
        cout << "== Shuffle Help ==" << endl
                << "Version: " << SHUFFLE_VERSION.str() << endl << endl
                << "Commands: " << endl;
        for (const auto&item: commands) {
            if (auto command = *item; command.getDescription() != "-") {
                cout << "  " << command.getName() << " : " << command.getDescription()
                        << endl;
            }
        }
        cout << endl << "Additional Help: " << endl
                << "  For more information on a specific command, type 'help <command>'" << endl
                << "  Visit the online documentation for Shuffle at "
                "https://github.com/shflterm/shuffle/wiki." << endl << endl;

        cout << "Thanks to: " << bg_green << "shfl credits" << reset << endl;

        return "true";
    }
    vector<string> cmdName = splitBySpace(optionValues["command"]);
    shared_ptr<Command> cmd = findCommand(cmdName.front());
    if (cmd == nullptr) {
        cout << "Command '" << cmdName.front() << "' not found." << endl;
        return "false";
    }

    for (int i = 1; i < cmdName.size(); ++i) {
        if (cmd == nullptr) break;
        cmd = findCommand(cmdName[i], cmd->getSubcommands());
    }

    string subcommands;
    for (const auto&item: cmd->getSubcommands()) {
        subcommands += item->getName() + ", ";
    }

    string examples;
    for (const auto&item: cmd->getExamples()) {
        examples += "\n  - " + item;
    }

    cout << "== About '" << cmd->getName() << "' ==" << endl;
    cout << "Name: " << cmd->getName() << endl;
    if (!cmd->getDescription().empty())
        cout << "Description: " << cmd->getDescription() << endl;
    if (!cmd->getUsage().empty())
        cout << "Usage: " << cmd->getUsage() << endl;
    if (!subcommands.empty())
        cout << "Subcommands: " << subcommands.substr(0, subcommands.size() - 2) << endl;
    cout << "Examples: " << examples << endl;

    return cmd->getName();
}

string snippetCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    //snf create aa help cd
    const string snippetName = optionValues["create"];
    const string cmd = optionValues["value"];

    addSnippet(snippetName, cmd);
    if (!backgroundMode) cout << "Snippet Created: " << snippetName << " => " << cmd << endl;

    return snippetName;
}

string clearCmd(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    if (!isAnsiSupported()) {
        error("'clear' cannot be used on terminals that do not support ANSI escape codes.");
        return "false";
    }
#ifdef _WIN32
    system("cls");
#elif defined(__linux__) || defined(__APPLE__)
    system("clear");
#endif
    return "true";
}


void loadCommands() {
    commands.clear();

    commands.push_back(make_shared<Command>(Command(
        "shfl", "Shuffle Command", {
            Command("reload", "Reload all commands.", shflReloadCmd),
            Command("upgrade", "Upgrade Shuffle to a new version.", shflUpgradeCmd),
            Command("credits", "Shuffle Credits", shflCreditsCmd),
        }, shflCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "appmgr", "App Manager", {
            Command("add", "Get new apps from the repository.", {
                        CommandOption("app", "", TEXT_T)
                    }, appMgrAddCmd),
            Command("remove", "Delete the app from your device.", {
                        CommandOption("app", "", TEXT_T)
                    }, appMgrRemoveCmd),
            Command("repo", "Repository Management", {
                        Command("add", "Add Repository", {
                                    CommandOption("repo", "", TEXT_T)
                                }, appMgrRepoAddCmd),
                        Command("remove", "Remove Repository", {
                                    CommandOption("repo", "", TEXT_T)
                                }, appMgrRepoRemoveCmd)
                    }, do_nothing),
        }, appMgrCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "help", "Show help", {
            CommandOption("command", "", TEXT_T, {"cmd", "help"})
        }, helpCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "snf", "Manage Snippets", {
            CommandOption("create", "", TEXT_T, {"mk", "c", "new"}),
            CommandOption("value", "", TEXT_T, {"v"}),
        }, snippetCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "clear", "Manage Snippets", clearCmd
    )));

    unloadAllApps();
    for (const string&appName: getApps()) {
        loadApp(appName);
    }
}
