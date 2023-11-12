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

void shflReloadCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    info("Reloading command...");
    loadCommands();
    loadSnippets();
    success("Reloaded all commands!");
}

void shflUpgradeCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    if (checkUpdate(false)) updateShuffle();
}

void shflCreditsCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    term << createCreditText();
}

void shflCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    // TODO: Print How to use
}

void appMgrAddCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    installApp(optionValues["app"]);
}

void appMgrRemoveCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    removeApp(optionValues["app"]);
}

void appMgrRepoAddCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    addRepo(optionValues["repo"]);
}

void appMgrRepoRemoveCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    removeRepo(optionValues["repo"]);
}

void appMgrCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    // TODO: Print How to use
}

void helpCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
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
    }
    else {
        const string cmdName = optionValues["command"];
        const shared_ptr<Command> cmd = findCommand(cmdName);
        if (cmd == nullptr) {
            term << "Command '" << cmdName << "' not found." << newLine;
            return;
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
        if (!subcommands.empty())
            term << "Subcommands: " << subcommands.substr(0, subcommands.size() - 2) << newLine;
        term << "Examples: " << examples << newLine;
    }
}

void snippetCmd([[maybe_unused]] Workspace* ws, map<string, string>&optionValues) {
    //snf create aa help cd
    const string snippetName = optionValues["create"];
    const string cmd = optionValues["value"];

    addSnippet(snippetName, cmd);
    term << "Snippet Created: " << snippetName << " => " << cmd << newLine;
}

void clearCmd([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
    term << eraseAll;
}


void loadCommands() {
    commands.clear();

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
                                             CommandOption("app", "", TEXT_T)
                                         }, appMgrAddCmd)),
            make_shared<Command>(Command("remove", "Delete the app from your device.", {
                                             CommandOption("app", "", TEXT_T)
                                         }, appMgrRemoveCmd)),
            make_shared<Command>(Command("repo", "Repository Management", {
                                             make_shared<Command>(Command("add", "Add Repository", {
                                                                              CommandOption("repo", "", TEXT_T)
                                                                          }, appMgrRepoAddCmd)),
                                             make_shared<Command>(Command("remove", "Remove Repository", {
                                                                              CommandOption("repo", "", TEXT_T)
                                                                          }, appMgrRepoRemoveCmd))
                                         }, do_nothing)),
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

    for (const string&appName: getApps()) {
        loadApp(appName);
    }
}
