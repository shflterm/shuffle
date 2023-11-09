#include "parsedcmd.h"

#include "sapp.h"

ParsedCommand::ParsedCommand(Command *app) : app(app) {}

void ParsedCommand::executeApp(Workspace *ws) {
    app->run(ws, options);
}