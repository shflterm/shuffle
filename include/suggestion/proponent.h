#ifndef PROPONENT_H
#define PROPONENT_H

#include <functional>
#include <map>
#include <string>
#include <utility>

#include "workspace/workspace.h"

using std::string, std::function, std::map;

typedef std::function<string(Workspace* ws, cmd::CommandOption option, const vector<string>& args, size_t cur)> prop_maker_t;

namespace suggestion {
    class Proponent {
        string typeName;
        prop_maker_t maker;

    public:
        Proponent(string type_name, prop_maker_t maker)
            : typeName(std::move(type_name)), maker(std::move(maker)) {
        }

        [[nodiscard]] const string& getTypeName() const;

        [[nodiscard]] const prop_maker_t& getMaker() const;

        string makeProp(Workspace* ws, const cmd::CommandOption&option, const vector<string>&args, size_t cur) const;
    };

    extern vector<Proponent> proponents;

    Proponent findProponent(const string&typeName);

    void registerProponent(const Proponent&proponent);
}

#endif //PROPONENT_H
