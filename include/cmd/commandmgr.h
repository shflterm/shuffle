//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <any>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace cmd {
    class Command;
}

#include "workspace/workspace.h"

using std::pair, std::shared_ptr, std::string, std::vector, std::map, std::any;

typedef std::function<string(Workspace* ws, map<string, string>&options, bool bgMode, string id)> cmd_t;

string do_nothing(Workspace* ws, map<string, string>&options, bool bgMode, const string&id);

namespace cmd {
    class CommandOption {
    public:
        string name, description;
        string type;
        vector<string> aliases;

        CommandOption(string name, string description, string type);

        CommandOption(string name, string description, string type, const vector<string>&aliases);
    };

    class Command {
    protected:
        string name;
        string description;
        string usage;
        vector<shared_ptr<Command>> subcommands;
        vector<CommandOption> options;
        vector<string> aliases;
        vector<string> examples;

        cmd_t cmd;

    public:
        [[nodiscard]] const string& getName() const;

        [[nodiscard]] const string& getDescription() const;

        [[nodiscard]] const string& getUsage() const;

        [[nodiscard]] const vector<shared_ptr<Command>>& getSubcommands() const;

        [[nodiscard]] const vector<CommandOption>& getOptions() const;

        [[nodiscard]] const vector<string>& getAliases() const;

        [[nodiscard]] const vector<string>& getExamples() const;

        void setCmd(cmd_t cmd);

        string run(Workspace* ws, map<string, string>&optionValues, bool backgroundMode, const string&taskId) const;

        [[nodiscard]] string createHint() const;

        Command(string name, string description, string usage,
                const vector<shared_ptr<Command>>&subcommands, const vector<CommandOption>&options,
                const vector<string>&aliases, const vector<string>&examples, cmd_t cmd);

        Command(string name, string description, string usage, const vector<shared_ptr<Command>>&subcommands,
                const vector<CommandOption>&options, const vector<string>&aliases, const vector<string>&examples);

        Command(string name, string description, const vector<Command>&subcommands, const vector<CommandOption>&options,
                const vector<string>&examples,
                cmd_t cmd);

        Command(string name, string description, const vector<Command>&subcommands, const vector<string>&examples,
                cmd_t cmd);

        Command(string name, string description, const vector<CommandOption>&options, const vector<string>&examples,
                cmd_t cmd);

        Command(string name, string description, const vector<string>&examples, cmd_t cmd);

        explicit Command(string name);

        shared_ptr<Command> parent;
    };

    shared_ptr<Command> findCommand(const string&name, const vector<shared_ptr<Command>>&DICTIONARY);

    shared_ptr<Command> findCommand(const string&name);
}

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
