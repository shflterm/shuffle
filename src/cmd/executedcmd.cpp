#include "cmd/executedcmd.h"

#include "sapp/sapp.h"

#include <utility>

ExecutedCommand::ExecutedCommand(Command *app) : app(app) {}

void ExecutedCommand::executeApp(Workspace ws) {
    auto *sappCommand = dynamic_cast<SAPPCommand *>(app);
    if (sappCommand != nullptr) {
        sappCommand->run(ws, options);
    } else {
        app->run(ws, options);
    }
}
