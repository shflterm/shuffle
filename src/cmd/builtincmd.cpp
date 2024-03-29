#include "cmd/builtincmd.h"

#include <iostream>
#include <memory>

#include "shfl.h"
#include "appmgr/appmgr.h"
#include "appmgr/downloader.h"
#include "cmd/job.h"
#include "utils/console.h"
#include "utils/credit.h"
#include "utils/utils.h"
#include "workspace/snippetmgr.h"
#include "utils/task.h"
#include "version.h"

using std::cout, std::endl, std::make_shared,
        cmd::Command, cmd::CommandOption, cmd::findCommand,
        appmgr::loadApp, appmgr::unloadAllApps,appmgr::getApps, appmgr::installApp, appmgr::removeApp, appmgr::addRepo,
        appmgr::removeRepo, appmgr::App, appmgr::loadedApps;

string writeHelp() {
    stringstream ss;
    ss << "== Shuffle Help ==" << endl
            << "Version: " << SHUFFLE_VERSION.str() << endl
            << endl
            << "Actions: " << endl
            << "When you enter the designated symbol for each Action and enter a command, "
            "the command is executed in a specific manner for the Action." << endl
            << "  " << fg_green << "\u2192(default) <shuffle command>" << reset << " : Execute the shuffle command" <<
            endl
            << "  " << fg_green << "@ <workspace name>" << reset << " : Move to the specified workspace" << endl
            << "  " << fg_green << "# <ai prompt>" << reset << " : Interact with AI" << endl
            << "  " << fg_green << "& <os command>" << reset << " : Execute the OS command" << endl
            << endl;

    const auto shuffleApp = loadedApps[0];
    ss << "Commands in '" << shuffleApp->name << "': " << endl;
    for (const auto&command: shuffleApp->commands) {
        ss << "  " << command->getName() << " : " << command->getDescription()
                << endl;
    }

    ss << endl
            << "If you want to see all apps, type 'appmgr list'." << endl
            << "If you want to know commands in other apps, type 'help <app>'." << endl;

    ss << endl << "Additional Help: " << endl
            << "  For more information on a specific command, type 'help <command>'" << endl
            << "  Visit the online documentation for Shuffle at "
            "https://github.com/shflterm/shuffle/wiki." << endl
            << endl;

    ss << "Thanks to: " << bg_green << "shfl credits" << reset << endl;

    return ss.str();
}

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

string appmgrAddCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return installApp(options["appname"]) ? "true" : "false";
}

string appmgrRemoveCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return removeApp(options["appname"]) ? "true" : "false";
}

string appmgrListCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    if (!bgMode) {
        for (const auto&app: loadedApps) {
            cout << app->name << " v" << app->getVersion() << ": " << app->description << " (by " << app->author << ")"
                    << endl;
        }
    }
    return std::to_string(loadedApps.size());
}

string appmgrRepoAddCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return addRepo(options["repourl"]) ? "true" : "false";
}

string appmgrRepoRemoveCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    return removeRepo(options["repourl"]) ? "true" : "false";
}

string appmgrAddlinkCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    Json::Value links = getShflJson("links");
    links.append(options["program"]);
    setShflJson("links", links);
    return "true";
}

string helpCmd(Workspace* ws, map<string, string>&options, bool bgMode, const string&id) {
    if (options.count("command") == 0) {
        cout << writeHelp();
        return "true";
    }

    const string command = options["command"];
    if (command.empty()) {
        cout << writeHelp();
        return "true";
    }

    shared_ptr<App> app;
    for (auto loaded_app: loadedApps) {
        if (loaded_app->name == command) {
            app = loaded_app;
            break;
        }
    }
    if (app != nullptr) {
        stringstream ss;
        ss << "Commands in '" << app->name << "': " << endl;
        for (const auto&cmd: app->commands) {
            ss << "  " << cmd->getName() << " : " << cmd->getDescription()
                    << endl;
        }
        cout << ss.str();
        return "true";
    }

    const vector<string> cmdName = splitBySpace(command);
    shared_ptr<Command> cmd = findCommand(cmdName.front());
    if (cmd == nullptr) {
        cout << "Command '" << cmdName.front() << "' not found." << endl;
        return "false";
    }

    for (int i = 1; i < cmdName.size(); ++i) {
        if (cmd == nullptr) break;
        cmd = findCommand(cmdName[i], cmd->getSubcommands());
    }

    cout << "== About '" << cmd->getName() << "' ==" << endl;
    cout << cmd->createHelpMessage();

    return cmd->getName();
}

string snfCreateCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    const string snippetName = options["name"];

    if (const string cmd = options["value"];
        addSnippet(snippetName, cmd)) {
        if (!bgMode) cout << "Snippet Created: " << snippetName << " => " << cmd << endl;
        return snippetName;
    }
    error("Snippet '" + snippetName + "' already exists.");
    return "false";
}

string snfRemoveCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    const string snippetName = options["name"];

    if (removeSnippet(snippetName)) {
        if (!bgMode) cout << "Snippet Removed: " << snippetName << endl;
        return snippetName;
    }

    error("Snippet '" + snippetName + "' not found.");
    return "false";
}

string snfListCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    if (!bgMode) {
        if (snippets.empty()) {
            info("No snippets.");
            return "0";
        }

        for (const auto&snippet: snippets) {
            cout << snippet->getName() << " => " << snippet->getTarget() << endl;
        }
    }
    return std::to_string(snippets.size());
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

string taskStopCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    const string taskId = options["taskId"];
    for (auto&task: tasks) {
        if (task.getId() == taskId) {
            if (task.job == nullptr) {
                error("Cannot stop job. Job is already stopped.");
                return "job is null";
            }
            return task.job->stop() ? "true" : "false";
        }
    }

    return "task not found";
}

string taskLogCmd(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    const string taskId = options["taskId"];
    for (auto&task: tasks) {
        if (task.getId() == taskId) {
            if (task.job == nullptr) {
                error("Cannot print log. Job is null.");
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
    unloadAllApps();

    vector<shared_ptr<Command>> builtinCommands;

    auto shflReload = make_shared<Command>(Command("reload", "Reload all commands.",
                                                   {
                                                       {"shfl reload", "Reload all commands."}
                                                   }, shflReloadCmd));
    auto shflUpgrade = make_shared<Command>(Command("upgrade", "Upgrade Shuffle.",
                                                    {
                                                        {"shfl upgrade", "Upgrade Shuffle."}
                                                    }, shflUpgradeCmd));
    auto shflCredits = make_shared<Command>(Command("credits", "Show credits.",
                                                    {
                                                        {"shfl credits", "Show credits."}
                                                    }, shflCreditsCmd));
    auto shfl = make_shared<Command>(Command(
        "shfl", "Shuffle Command", {}, incorrect_usage
    ));
    shfl->setSubcommands({
        shflReload, shflUpgrade, shflCredits
    });
    shflReload->setParent(shfl);
    shflUpgrade->setParent(shfl);
    shflCredits->setParent(shfl);

    builtinCommands.push_back(shfl);

    auto appmgrAdd = make_shared<Command>(Command("add", "Get new apps from the repository.", {
                                                      CommandOption("appname", "", "text", true)
                                                  }, {
                                                      {
                                                          "appmgr add textutilities",
                                                          "Download and add the app 'textutilities' from a known repository."
                                                      },
                                                      {
                                                          "appmgr add filesystem",
                                                          "Download and add the app 'filesystem' from a known repository."
                                                      },
                                                      {
                                                          "appmgr add /path/to/myapp",
                                                          "Add the app 'myapp' from a local file."
                                                      }
                                                  }, appmgrAddCmd));

    auto appmgrRemove = make_shared<Command>(Command("remove", "Delete the appmgr from your device.", {
                                                         CommandOption("appname", "", "app", true)
                                                     }, {
                                                         {
                                                             "appmgr remove textutilities",
                                                             "Delete the app 'textutilities'."
                                                         },
                                                         {"appmgr remove filesystem", "Delete the app 'filesystem'."},
                                                         {"appmgr remove myapp", "Delete the app 'myapp'."}
                                                     }, appmgrRemoveCmd));

    auto appmgrList = make_shared<Command>(Command("list", "List all apps.", {
                                                       {"appmgr list", "List all apps."}
                                                   }, appmgrListCmd));

    auto appmgrRepoAdd = make_shared<Command>(Command("add", "Add Repository", {
                                                          CommandOption("repourl", "", "text", true)
                                                      }, {
                                                          {
                                                              "appmgr repo add https://example.com/shflrepo.json",
                                                              "Add 'https://example.com/shflrepo.json' to known repositories."
                                                          }
                                                      }, appmgrRepoAddCmd));

    auto appmgrRepoRemove = make_shared<Command>(Command("remove", "Remove Repository", {
                                                             CommandOption("repourl", "", "text", true)
                                                         }, {
                                                             {
                                                                 "appmgr repo remove https://example.com/shflrepo.json",
                                                                 "Remove 'https://example.com/shflrepo.json' from known repositories."
                                                             }
                                                         }, appmgrRepoRemoveCmd));

    auto appmgrRepo = make_shared<Command>(Command(
        "repo", "Repository Management", {}, incorrect_usage
    ));
    appmgrRepo->setSubcommands({appmgrRepoAdd, appmgrRepoRemove});
    appmgrRepoAdd->setParent(appmgrRepo);
    appmgrRepoRemove->setParent(appmgrRepo);

    auto appmgrAddlink = make_shared<Command>(Command("addlink", "Link OS command to shuffle command", {
                                                          CommandOption("program", "program to link to", "text", true),
                                                      }, {
                                                          {
                                                              "appmgr addlink ipconfig",
                                                              "(In Windows) Link so that the 'ipconfig' command can be used in shuffle as well."
                                                          },
                                                          {
                                                              "appmgr addlink ifconfig",
                                                              "(In Unix) Link so that the 'ifconfig' command can be used in shuffle as well."
                                                          }
                                                      }, appmgrAddlinkCmd));

    auto appmgr = make_shared<Command>(Command(
        "appmgr", "App Manager", {}, incorrect_usage
    ));
    appmgr->setSubcommands({appmgrAdd, appmgrRemove, appmgrList, appmgrRepo, appmgrAddlink});
    appmgrAdd->setParent(appmgr);
    appmgrRemove->setParent(appmgr);
    appmgrList->setParent(appmgr);
    appmgrRepo->setParent(appmgr);
    appmgrAddlink->setParent(appmgr);

    builtinCommands.push_back(appmgr);

    auto snfCreate = make_shared<Command>(Command("create", "Create a new snippet.", {
                                                      CommandOption("name", "", "text", true),
                                                      CommandOption("value", "", "command", true)
                                                  }, {
                                                      {
                                                          "snf create aa help cd",
                                                          "Create a new snippet 'aa' with value 'help cd'."
                                                      }
                                                  }, snfCreateCmd));

    auto snfRemove = make_shared<Command>(Command("remove", "Remove a snippet.", {
                                                      CommandOption("name", "", "snippet", true)
                                                  }, {
                                                      {
                                                          "snf remove aa",
                                                          "Remove the snippet 'aa'."
                                                      }
                                                  }, snfRemoveCmd));

    auto snfList = make_shared<Command>(Command("list", "List all snippets.", {
                                                    {"snf list", "List all snippets."}
                                                }, snfListCmd));

    auto snf = make_shared<Command>(Command(
        "snf", "Snippet Management", {}, incorrect_usage
    ));
    snf->setSubcommands({snfCreate, snfRemove, snfList});
    snfCreate->setParent(snf);
    snfRemove->setParent(snf);
    snfList->setParent(snf);

    builtinCommands.push_back(snf);

    auto help = make_shared<Command>(Command(
        "help", "Show help", {
            CommandOption("command", "If given, a detailed description of the command is provided.", "cmdorapp", false)
        }, {
            {"help", "Show help"},
            {"help shfl", "Show help for 'shfl'"},
            {"help shfl upgrade", "Show help for 'shfl upgrade'"},
            {"help appmgr", "Show help for 'appmgr'"}
        }, helpCmd
    ));

    builtinCommands.push_back(help);

    auto clear = make_shared<Command>(Command(
        "clear", "Clear everything", {
            {"clear", "Clears all text to clear the screen."}
        }, clearCmd
    ));

    builtinCommands.push_back(clear);

    auto taskStart = make_shared<Command>(Command("start", "Start a new background task.", {
                                                      CommandOption("job", "", "command", true)
                                                  }, {
                                                      {
                                                          "task start dwnld https://examples.com/largefile",
                                                          "Start a new background task to 'dwnld https://examples.com/largefile'."
                                                      }
                                                  }, taskStartCmd));

    auto taskStop = make_shared<Command>(Command("stop", "Stop a background task.", {
                                                     CommandOption("taskId", "", "text", true)
                                                 }, {
                                                     {"task stop abcdefg12345", "Stop the task 'abcdefg12345'."}
                                                 }, taskStopCmd));

    auto taskLog = make_shared<Command>(Command("log", "Print logs", {
                                                    CommandOption("taskId", "", "text", true)
                                                }, {
                                                    {"task log abcdefg12345", "Print logs of the task 'abcdefg12345'."}
                                                }, taskLogCmd));

    auto taskList = make_shared<Command>(Command("list", "List tasks", {
                                                     {"task list", "List tasks"}
                                                 }, taskListCmd));

    auto task = make_shared<Command>(Command(
        "task", "Manage background tasks", {}, incorrect_usage
    ));
    task->setSubcommands({taskStart, taskStop, taskLog, taskList});
    taskStart->setParent(task);
    taskStop->setParent(task);
    taskLog->setParent(task);
    taskList->setParent(task);

    builtinCommands.push_back(task);


    auto builtinApp = App("shuffle", "Shuffle built-in app", "Shuffle", SHUFFLE_VERSION.str().substr(1),
                          builtinCommands);
    loadApp(make_shared<App>(builtinApp));

    for (const string&appName: getApps()) {
        loadApp(make_shared<App>(appName));
    }
}
