#include "cmd/job.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#endif

#include "shfl.h"
#include "utils/console.h"

using std::cout;

namespace job {
    Job::Job(const shared_ptr<Command>&app) : jobType(COMMAND), command(app) {
    }

    Job::Job(const JobType commandType) : jobType(commandType) {
    }

    Job::Job(const JobType commandType, string content) : jobType(commandType), content(std::move(content)) {
    }

    Job::Job() = default;

    string Job::start(Workspace* ws, const bool backgroundMode) {
        if (!isCommand()) {
            error("Cannot run non-command job!");
            return "IT_IS_NOT_COMMAND";
        }

        if (jobType == COMMAND && command == nullptr) {
            error("Invalid command!");
            return "INVALID_COMMAND";
        }

        const std::shared_future resultFuture = resultPromise.get_future();

        jobThread = std::thread([&](std::promise<string>&p) {
            if (jobType == EXECUTABLE_COMMAND) {
                p.set_value(execute_program(content) ? "success" : "failed");
            }
            else if (jobType == COMMAND) {
                p.set_value(command->run(ws, options, backgroundMode, id));
            }
        }, std::ref(resultPromise));

        auto result = resultFuture.get();
        if (result != "cancelled") jobThread.detach();
        return result;
    }

    bool Job::stop() {
#ifdef _WIN32
        const HANDLE threadHandle = jobThread.native_handle();
        if (TerminateThread(threadHandle, 0)) {
            jobThread.detach();

            resultPromise.set_value("cancelled");
            return true;
        }
        CloseHandle(threadHandle);
        return false;
#elif defined(__linux__) || defined(__APPLE__)
        if (const pthread_t threadHandle = jobThread.native_handle();
            pthread_cancel(threadHandle) == 0) {
            jobThread.detach();

            resultPromise.set_value("cancelled");
            return true;
        }
        return false;
#endif
    }

    bool Job::isCommand() const {
        return jobType == COMMAND || jobType == EXECUTABLE_COMMAND;
    }

    bool Job::isSuccessed() const {
        return jobType != EMPTY;
    }

    bool Job::isEmpty() const {
        return jobType == EMPTY || jobType == EMPTY_CAUSED_BY_ARGUMENTS || jobType == EMPTY_CAUSED_BY_NO_SUCH_COMMAND;
    }

    bool Job::isEmptyCausedByArguments() const {
        return jobType == EMPTY_CAUSED_BY_ARGUMENTS;
    }

    bool Job::isEmptyCausedByNoSuchCommand() const {
        return jobType == EMPTY_CAUSED_BY_NO_SUCH_COMMAND;
    }
}
