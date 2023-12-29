#include "suggestion/proponent.h"

namespace suggestion {
    const string& Proponent::getTypeName() const {
        return typeName;
    }

    const prop_maker_t& Proponent::getMaker() const {
        return maker;
    }

    string Proponent::makeProp(Workspace* ws, const cmd::CommandOption&option, const vector<string>&args,
                               const size_t cur) const {
        string result = maker(ws, option, args, cur);
        if (result.empty() && args[cur].empty()) return "<" + option.name + ">";
        return result;
    }

    vector<Proponent> proponents;

    Proponent findProponent(const string&typeName) {
        for (const Proponent&proponent: proponents) {
            if (proponent.getTypeName() == typeName) {
                return proponent;
            }
        }
        return {
            "", [](Workspace* ws, const cmd::CommandOption&option, const vector<string>&args,
                   const size_t cur) {
                return "";
            }
        };
    }

    void registerProponent(const Proponent&proponent) {
        proponents.push_back(proponent);
    }
}
