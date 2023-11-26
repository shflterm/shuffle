#include "utils/task.h"

#include <iostream>
#include <utility>

#include "utils/console.h"
#include "utils/utils.h"
#include "workspace/workspace.h"

using std::cout, std::endl;

Task::Task(string command): command(std::move(command)) {
}

void Task::start(Workspace* ws) {
    job = ws->createJob(command);

    status = RUNNING;
    taskId = generateRandomString(10);
    taskThread = thread([this, ws]() -> string {
        if (job == nullptr) {
            status = FINISHED;
            info("CMD_NOT_FOUND");
            return "CMD_NOT_FOUND";
        }

        string result = job->start(ws, true);
        status = FINISHED;

        return result;
    });
    taskThread.detach();
}

TaskStatus Task::getStatus() const {
    return status;
}

string Task::getId() {
    return taskId;
}

vector<Task> tasks;
