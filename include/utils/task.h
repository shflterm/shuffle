//
// Created by 08sih on 2023-11-21.
//

#ifndef JOB_H
#define JOB_H

#include <string>
#include <utility>
#include <thread>

#include "workspace.h"
#include "job.h"

using std::string, std::thread;

enum TaskStatus {
    WAITING,
    RUNNING,
    STOPPED,
    FINISHED
};

class Task {
    string taskId;
    TaskStatus status = WAITING;
    thread taskThread;

public:
    string command;
    shared_ptr<Job> job;

    explicit Task(string command);

    Task(Task&&other) noexcept : taskThread(std::move(other.taskThread)) {
        taskId = other.taskId;
        status = other.status;
        command = other.command;
        job = other.job;
    }

    Task& operator=(Task&&other) noexcept {
        if (this != &other) {
            taskId = other.taskId;
            status = other.status;
            command = other.command;

            taskThread = std::move(other.taskThread);
        }
        return *this;
    }

    void start(Workspace*);

    void stop();

    [[nodiscard]] TaskStatus getStatus() const;

    string getId();
};

extern vector<Task> tasks;

#endif //JOB_H
