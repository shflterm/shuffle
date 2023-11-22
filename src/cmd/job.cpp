#include "job.h"

#include "appmgr.h"

Job::Job(const shared_ptr<Command>&app) : command(app), jobType(COMMAND) {
}

Job::Job(const JobType commandType) : jobType(commandType) {
}

Job::Job() = default;

string Job::start(Workspace* ws, const bool backgroundMode) {
    if (jobType == SNIPPET) return "IT_IS_SNIPPET";
    if (jobType == VARIABLE) return "IT_IS_VARIABLE";
    if (jobType == EMPTY) return "CMD_NOT_FOUND";
    if (command == nullptr) return "APP_IS_NULL";

    return command->run(ws, options, backgroundMode, id);
}

bool Job::isCommand() const {
    return jobType == COMMAND;
}

bool Job::isSuccessed() const {
    return jobType != EMPTY;
}
