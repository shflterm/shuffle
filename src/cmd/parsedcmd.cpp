#include "parsedcmd.h"

#include "appmgr.h"

ParsedCommand::ParsedCommand(const shared_ptr<Command>&app) : app(app), commandType(COMMAND) {
}

ParsedCommand::ParsedCommand(const CommandType commandType) : commandType(commandType) {
}

ParsedCommand::ParsedCommand() = default;

string ParsedCommand::executeApp(Workspace* ws, const bool backgroundMode) {
    if (commandType == SNIPPET) return "IT_IS_SNIPPET";
    if (commandType == VARIABLE) return "IT_IS_VARIABLE";
    if (commandType == EMPTY) return "CMD_NOT_FOUND";
    if (app == nullptr) return "APP_IS_NULL";

    return app->run(ws, options, backgroundMode);
}

bool ParsedCommand::isCommand() const {
    return commandType == COMMAND;
}

bool ParsedCommand::isSuccessed() const {
    return commandType != EMPTY;
}
