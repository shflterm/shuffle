#include "builtincmd.h"

#include <memory>

#include "term.h"
#include "console.h"
#include "downloader.h"
#include "credit.h"
#include "utils.h"
#include "version.h"
#include "snippetmgr.h"
#include "appmgr.h"

using std::make_shared;

string shflReloadCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    info("Reloading command...");
    loadCommands();
    loadSnippets();
    success("Reloaded all commands!");
    return "true";
}

string shflUpgradeCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    if (checkUpdate(false)) {
        updateShuffle();
        return "true";
    }
    return "false";
}

string shflCreditsCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    term << createCreditText();
    return "thanks";
}

string shflCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    // TODO: Print How to use
    return "true";
}

string appMgrAddCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    return installApp(optionValues["app"]) ? "true" : "false";
}

string appMgrRemoveCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    return removeApp(optionValues["app"]) ? "true" : "false";
}

string appMgrRepoAddCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    return addRepo(optionValues["repo"]) ? "true" : "false";
}

string appMgrRepoRemoveCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    return removeRepo(optionValues["repo"]) ? "true" : "false";
}

string appMgrCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    // TODO: Print How to use
    return "true";
}

string helpCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    if (optionValues.count("command") == 0) {
        term << "== Shuffle Help ==" << newLine
                << "Version: " << SHUFFLE_VERSION.str() << newLine << newLine
                << "Commands: " << newLine;
        for (const auto&item: commands) {
            if (auto command = *item; command.getDescription() != "-") {
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

        return "true";
    }
    vector<string> cmdName = splitBySpace(optionValues["command"]);
    shared_ptr<Command> cmd = findCommand(cmdName.front());
    if (cmd == nullptr) {
        term << "Command '" << cmdName.front() << "' not found." << newLine;
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

    term << "== About '" << cmd->getName() << "' ==" << newLine;
    term << "Name: " << cmd->getName() << newLine;
    if (!cmd->getDescription().empty())
        term << "Description: " << cmd->getDescription() << newLine;
    if (!cmd->getUsage().empty())
        term << "Usage: " << cmd->getUsage() << newLine;
    if (!subcommands.empty())
        term << "Subcommands: " << subcommands.substr(0, subcommands.size() - 2) << newLine;
    term << "Examples: " << examples << newLine;

    return cmd->getName();
}

string snippetCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    //snf create aa help cd
    const string snippetName = optionValues["create"];
    const string cmd = optionValues["value"];

    addSnippet(snippetName, cmd);
    term << "Snippet Created: " << snippetName << " => " << cmd << newLine;

    return snippetName;
}

string clearCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    term << eraseAll;
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
