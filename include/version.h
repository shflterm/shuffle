#ifndef SHUFFLE_INCLUDE_VERSION_H_
#define SHUFFLE_INCLUDE_VERSION_H_

#include <string>
#include <sstream>

#define SHUFFLE_VERSION Version(1, 0, 0, 0)

class Version {
    int major, minor, patch, hotfix;

public:
    Version(int major, int minor, int patch, int hotfix) : major(major), minor(minor), patch(patch), hotfix(hotfix) {}

    [[nodiscard]] string str() const {
        stringstream ss;
        ss << "v";
        ss << major << ".";
        ss << minor << ".";
        ss << patch << ".";
        ss << hotfix;

        return string(ss.str());
    }
};

#endif //SHUFFLE_INCLUDE_VERSION_H_