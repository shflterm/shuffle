//
// Created by 08sih on 7/6/2023.
//

#ifndef SHFL_PARSEDCMD_H
#define SHFL_PARSEDCMD_H

#include <map>
#include <string>
#include <memory>

namespace job {
    class Job;
}

#include "utils/utils.h"
#include "workspace/workspace.h"
#include "commandmgr.h"

using std::shared_ptr, std::map, std::string;

namespace job {
    enum JobType {
        COMMAND,
        SNIPPET,
        VARIABLE,
        EMPTY
    };

    class Job {
        JobType jobType = EMPTY;

    public:
        shared_ptr<cmd::Command> command;
        map<string, string> options;
        string id = generateRandomString(16);

        string start(Workspace* ws, bool backgroundMode = false);

        [[nodiscard]] bool isCommand() const;

        [[nodiscard]] bool isSuccessed() const;

        explicit Job(const shared_ptr<cmd::Command>&app);

        explicit Job(JobType commandType);

        Job();
    };
}

#endif //SHFL_PARSEDCMD_H
