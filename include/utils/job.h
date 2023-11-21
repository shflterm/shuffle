//
// Created by 08sih on 2023-11-21.
//

#ifndef JOB_H
#define JOB_H

#include <ostream>
#include <string>
#include <utility>
#include <thread>
#include "workspace.h"

using std::string, std::thread;

enum job_status {
    WAITING,
    RUNNING,
    STOPPED,
    FINISHED
};

class Job {
    string jobId;
    job_status status = WAITING;
    thread jobThread;

public:
    string command;

    explicit Job(string command);

    Job(Job&&other) noexcept : jobThread(std::move(other.jobThread)) {
        jobId = other.jobId;
        status = other.status;
        command = other.command;
    }

    Job& operator=(Job&&other) noexcept {
        if (this != &other) {
            jobId = other.jobId;
            status = other.status;
            command = other.command;

            jobThread = std::move(other.jobThread);
        }
        return *this;
    }

    void start(Workspace*);

    void stop();

    [[nodiscard]] job_status getStatus() const;

    string getId();
};

extern vector<Job> jobs;

#endif //JOB_H
