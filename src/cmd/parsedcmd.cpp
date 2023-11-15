#include "parsedcmd.h"

#include "appmgr.h"

ParsedCommand::ParsedCommand(Command *app) : app(app) {}

string ParsedCommand::executeApp(Workspace *ws) {
    return app->run(ws, options);
}