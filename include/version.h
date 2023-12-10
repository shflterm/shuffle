#ifndef SHUFFLE_INCLUDE_VERSION_H_
#define SHUFFLE_INCLUDE_VERSION_H_

#include <string>
#include <sstream>
#include <utility>

using std::stringstream;

#define SHUFFLE_VERSION Version(1, 0, 0, 0, "beta.3-1")

class Version {
    int major, minor, patch, hotfix;
    string tag;

public:
    Version(const int major, const int minor, const int patch, const int hotfix) : major(major), minor(minor),
        patch(patch), hotfix(hotfix) {
    }

    Version(const int major, const int minor, const int patch, const int hotfix, string tag) : major(major),
        minor(minor), patch(patch), hotfix(hotfix), tag(std::move(tag)) {
    }

    [[nodiscard]] string str() const {
        stringstream ss;
        ss << "v";
        ss << major << ".";
        ss << minor << ".";
        ss << patch << ".";
        ss << hotfix;
        if (!tag.empty()) ss << "-" << tag;

        return string(ss.str());
    }
};

#endif //SHUFFLE_INCLUDE_VERSION_H_
