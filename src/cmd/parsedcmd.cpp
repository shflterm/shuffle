#include "parsedcmd.h"

#include "appmgr.h"

ParsedCommand::ParsedCommand(Command *app) : app(app) {}

void ParsedCommand::executeApp(Workspace *ws) {
    app->run(ws, options);
}