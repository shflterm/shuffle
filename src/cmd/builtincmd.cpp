#include "cmd/builtincmd.h"

#include <memory>
#include "term.h"

#include "cmd/commandmgr.h"
#include "console.h"
#include "sapp/downloader.h"
#include "utils/credit.h"
#include "utils/utils.h"
#include "version.h"
#include "snippetmgr.h"

void shflCmd(Workspace &ws, map<string, string> &optionValues) {
//    if (args[1] == "reload") {
//        info("Reloading command...");
//        loadCommands();
//        success("Reloaded all commands!");
//    } else if (args[1] == "apps") {
//        if (args.size() < 3) {
//            too_many_arguments();
//            return;
//        }
//        if (args[2] == "add") {
//            if (args.size() < 4) {
//                too_many_arguments();
//                return;
//            }
//
//            for (int i = 3; i < args.size(); ++i) {
//                addSAPP(args[i]);
//            }
//        } else if (args[2] == "remove") {
//            if (args.size() < 4) {
//                too_many_arguments();
//                return;
//            }
//
//            for (int i = 3; i < args.size(); ++i) {
//                removeSAPP(args[i]);
//            }
//        }
//    } else if (args[1] == "update") {
//        string latest = trim(readTextFromWeb(
//                "https://raw.githubusercontent.com/shflterm/shuffle/main/LATEST"));
//        if (latest != SHUFFLE_VERSION.str()) {
//            term << "New version available: " << SHUFFLE_VERSION.str() << " -> "
//                 << latest << newLine;
//            updateShuffle();
//        } else {
//            term << "You are using the latest version of Shuffle." << newLine;
//        }
//    } else if (args[1] == "credits") {
//        term << createCreditText();
//    }
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
    string snippetName = optionValues["name"];
    string cmd;
//    for (int i = 3; i < args.size(); ++i) {
//        cmd += args[i] + " ";
//    }

    addSnippet(snippetName, cmd);
    term << "Snippet Created: " << snippetName << " => " << cmd << newLine;
}

void BuiltinCommand::run(Workspace &ws, map<string, string> &optionValues) const {
    cmd(ws, optionValues);
}
