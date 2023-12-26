#include "cmd/job.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#endif

#include "shfl.h"
#include "utils/console.h"

namespace job {
    Job::Job(const shared_ptr<Command>&app) : jobType(COMMAND), command(app) {
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

        const std::shared_future resultFuture = resultPromise.get_future();

        jobThread = std::thread([&](std::promise<string>&p) {
            p.set_value(command->run(ws, options, backgroundMode, id));
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
        return jobType == COMMAND;
    }

    bool Job::isSuccessed() const {
        return jobType != EMPTY;
    }
}
