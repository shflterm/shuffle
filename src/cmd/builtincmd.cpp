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
#include "job.h"
#include "task.h"

using std::cout, std::endl, std::make_shared,
        cmd::Command, cmd::CommandOption, cmd::commands, cmd::findCommand,
        appmgr::loadApp, appmgr::unloadAllApps,appmgr::getApps, appmgr::installApp, appmgr::removeApp, appmgr::addRepo,
        appmgr::removeRepo;

string shflReloadCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    if (!bgMode) info("Reloading command...");
    loadCommands();
    loadSnippets();
    if (!bgMode) success("Reloaded all commands!");
    return "true";
}

string shflUpgradeCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    if (checkUpdate(false)) {
        updateShuffle();
        return "true";
    }
    return "false";
}

string shflCreditsCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    if (!bgMode) cout << createCreditText();
    return "thanks";
}

string shflCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    // TODO: Print How to use
    return "true";
}

string appMgrAddCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return installApp(options["app"]) ? "true" : "false";
}

string appMgrRemoveCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return removeApp(options["app"]) ? "true" : "false";
}

string appMgrRepoAddCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return addRepo(options["repo"]) ? "true" : "false";
}

string appMgrRepoRemoveCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return removeRepo(options["repo"]) ? "true" : "false";
}

string appMgrCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    // TODO: Print How to use
    return "true";
}

string helpCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    if (options.count("command") == 0) {
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
    vector<string> cmdName = splitBySpace(options["command"]);
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

string snippetCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    //snf create aa help cd
    const string snippetName = options["create"];
    const string cmd = options["value"];

    addSnippet(snippetName, cmd);
    if (!bgMode) cout << "Snippet Created: " << snippetName << " => " << cmd << endl;

    return snippetName;
}

string clearCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
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

string taskStartCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    tasks.emplace_back(options["job"]);
    tasks.back().start(ws);
    const string taskId = tasks.back().getId();
    if (!bgMode) info("New Job started: " + taskId);
    return taskId;
}

string taskLogCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    const string taskId = options["taskId"];
    for (auto&task: tasks) {
        if (task.getId() == taskId) {
            if (task.job == nullptr) {
                error("Job is null.");
                return "job is null";
            }
            string jobId = task.job->id;

            path jobOutout = DOT_SHUFFLE / "outputs" / (jobId + ".log");
            string outputStr = readFile(jobOutout);
            if (!bgMode) info(outputStr);
            return outputStr;
        }
    }

    return "task not found";
}

string taskListCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    if (bgMode) return std::to_string(tasks.size());

    if (tasks.empty()) {
        info("No tasks running.");
        return "0";
    }
    for (auto&task: tasks)
        info("$0 ($1) : $2", {task.getId(), std::to_string(task.getStatus()), task.command});
    return std::to_string(tasks.size());
}

void loadCommands() {
    commands.clear();

    commands.push_back(make_shared<Command>(Command(
        "shfl", "Shuffle Command", {
            Command("reload", "Reload all commands.",
                    {"shfl reload"}, shflReloadCmd),
            Command("upgrade", "Upgrade Shuffle to a new version.",
                    {"shfl upgrade"}, shflUpgradeCmd),
            Command("credits", "Shuffle Credits",
                    {"shfl credits"}, shflCreditsCmd),
        }, {"shfl"}, shflCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "appmgr", "App Manager", {
            Command("add", "Get new apps from the repository.", {
                        CommandOption("app", "", cmd::TEXT)
                    }, {
                        "appmgr add textutilities", "appmgr add filesystem", "appmgr add /path/to/myapp"
                    }, appMgrAddCmd),
            Command("remove", "Delete the app from your device.", {
                        CommandOption("app", "", cmd::TEXT)
                    }, {"appmgr remove textutilities", "appmgr remove filesystem", "appmgr remove myapp"},
                    appMgrRemoveCmd),
            Command("repo", "Repository Management", {
                        Command("add", "Add Repository", {
                                    CommandOption("repo", "", cmd::TEXT)
                                }, {"appmgr repo add https://example.com/shflrepo.json"}, appMgrRepoAddCmd),
                        Command("remove", "Remove Repository", {
                                    CommandOption("repo", "", cmd::TEXT)
                                }, {"appmgr repo remove https://example.com/shflrepo.json"}, appMgrRepoRemoveCmd)
                    }, {"appmgr repo"}, do_nothing),
        }, {"appmgr"}, appMgrCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "help", "Show help", {
            CommandOption("command", "", cmd::COMMAND, {"cmd", "help"})
        }, {"help", "help shfl", "help appmgr"}, helpCmd
    )));
    // commands.push_back(make_shared<Command>(Command(
    //     "snf", "Manage Snippets", {
    //         CommandOption("create", "", TEXT_T, {"mk", "c", "new"}),
    //         CommandOption("value", "", TEXT_T, {"v"}),
    //     }, {"snf create sayhello echo Hi!"}, snippetCmd
    // )));
    commands.push_back(make_shared<Command>(Command(
        "clear", "Clear everything", {"clear"}, clearCmd
    )));
    commands.push_back(make_shared<Command>(Command(
        "task", "Manage background tasks", {
            Command("start", "Start a new background task.", {
                        CommandOption("job", "", cmd::COMMAND)
                    }, {"task start dwnld https://examples.com/largefile"}, taskStartCmd),
            Command("log", "Print logs", {
                        CommandOption("taskId", "", cmd::TEXT)
                    }, {"task log abcdefg12345"}, taskLogCmd),
            Command("list", "List tasks", {"task list"}, taskListCmd),
        }, {"task"}, do_nothing
    )));

    unloadAllApps();
    for (const string&appName: getApps()) {
        loadApp(appName);
    }
}
