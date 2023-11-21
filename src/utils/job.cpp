#include "job.h"

#include <iostream>
#include <utils.h>

#include "console.h"

using std::cout, std::endl;

Job::Job(string command): command(std::move(command)) {
}

void Job::start(Workspace* ws) {
    status = RUNNING;
    jobId = generateRandomString(10);
    jobThread = thread([this, ws]() -> string {
        const shared_ptr<ParsedCommand> parsed = ws->parse(command);
        if (parsed == nullptr) {
            status = FINISHED;
            return "CMD_NOT_FOUND";
        }

        string result = parsed->executeApp(ws, true);
        status = FINISHED;

        return result;
    });
    jobThread.detach();
}

job_status Job::getStatus() const {
    return status;
}

string Job::getId() {
    return jobId;
}

vector<Job> jobs;
