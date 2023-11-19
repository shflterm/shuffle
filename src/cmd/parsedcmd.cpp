#include "parsedcmd.h"

#include <utility>

#include "appmgr.h"

ParsedCommand::ParsedCommand(Command* app) : app(app), commandType(COMMAND) {
}

ParsedCommand::ParsedCommand(const CommandType commandType) : commandType(commandType) {
}

ParsedCommand::ParsedCommand() : commandType(EMPTY) {
}

string ParsedCommand::executeApp(Workspace* ws, const bool backgroundMode) {
    if (commandType == SNIPPET) return "IT_IS_SNIPPET";
    if (commandType == VARIABLE) return "IT_IS_VARIABLE";
    if (commandType == EMPTY) return "CMDTYPE_EMPTY";
    if (app == nullptr) return "APP_IS_NULL";

    return app->run(ws, options, backgroundMode);
}

bool ParsedCommand::isCommand() const {
    return commandType == COMMAND;
}

bool ParsedCommand::isSuccessed() const {
    return commandType != EMPTY;
}
