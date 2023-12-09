#include "cmd/job.h"

#include "utils/console.h"

namespace job {
    Job::Job(const shared_ptr<cmd::Command>&app) : jobType(COMMAND), command(app) {
    }

    Job::Job(const JobType commandType) : jobType(commandType) {
    }

    Job::Job() = default;

    string Job::start(Workspace* ws, const bool backgroundMode) {
        if (jobType == SNIPPET) {
            error("Cannot run snippet directly!");
            return "IT_IS_SNIPPET";
        }
        if (jobType == VARIABLE) {
            error("Cannot run variable directly!");
            return "IT_IS_VARIABLE";
        }
        if (jobType == EMPTY) {
            error("Cannot run empty job!");
            return "IT_IS_EMPTY";
        }
        if (command == nullptr) {
            error("Invalid command!");
            return "INVALID_COMMAND";
        }

        return command->run(ws, options, backgroundMode, id);
    }

    bool Job::isCommand() const {
        return jobType == COMMAND;
    }

    bool Job::isSuccessed() const {
        return jobType != EMPTY;
    }
}
