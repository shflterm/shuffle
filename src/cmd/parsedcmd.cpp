#include "parsedcmd.h"

#include <utility>

#include "appmgr.h"

ParsedCommand::ParsedCommand(Command* app) : app(app), commandType(COMMAND) {
}

ParsedCommand::ParsedCommand(const CommandType commandType) : commandType(commandType) {
}

ParsedCommand::ParsedCommand() : commandType(EMPTY) {
}

string ParsedCommand::executeApp(Workspace* ws) {
    if (commandType == SNIPPET) return "SNIPPET";
    if (commandType == VARIABLE) return "VARIABLE";
    if (commandType == EMPTY) return "EMPTY";
    if (app == nullptr) return "APP_IS_NULL";

    return app->run(ws, options);
}

bool ParsedCommand::isCommand() {
    return commandType == COMMAND;
}
