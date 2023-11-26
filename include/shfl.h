//
// Created by 08sih on 2023-11-26.
//

#ifndef SHFL_H
#define SHFL_H

#include "app/appmgr.h"
#include "app/downloader.h"

#include "cmd/cmdparser.h"
#include "cmd/commandmgr.h"
#include "cmd/job.h"

#include "suggestion/suggestion.h"

#include "utils/console.h"
#include "utils/credit.h"
#include "utils/i18n.h"
#include "utils/task.h"
#include "utils/utils.h"

#include "workspace/crashreport.h"
#include "workspace/snippetmgr.h"
#include "workspace/workspace.h"

#ifdef _WIN32
#define COMMAND_MAIN extern "C" __declspec(dllexport) string entrypoint
#else
#define COMMAND_MAIN extern "C" string entrypoint
#endif

#endif //SHFL_H
