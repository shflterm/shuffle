#include "parsedcmd.h"

#include "sapp.h"

ParsedCommand::ParsedCommand(Command *app) : app(app) {}

void ParsedCommand::executeApp(Workspace ws) {
    auto *sappCommand = dynamic_cast<SAPPCommand *>(app);
    if (sappCommand != nullptr) {
        sappCommand->run(ws, options);
    } else {
        app->run(ws, options);
    }
}