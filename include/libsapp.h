//
// Created by winch on 5/12/2023.
//

#ifndef SHUFFLE_INCLUDE_LIBSAPP_H_
#define SHUFFLE_INCLUDE_LIBSAPP_H_

#include <string>
#include <vector>

#include "workspace.h"

using namespace std;

#ifdef _WIN32
#define SAPPENTRYPOINT extern "C" __declspec(dllexport)
#else
#define SAPPENTRYPOINT extern "C"
#endif

#endif //SHUFFLE_INCLUDE_LIBSAPP_H_
