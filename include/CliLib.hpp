#ifndef CLIAPP_CLILIB_HPP
#define CLIAPP_CLILIB_HPP

#include <string>
#include <regex>
#include <iostream>

class Command {
public:
    template<typename Func, typename... Args>
    explicit Command(Func function, Args&... args);
    void setAsDefault();
    void run(std::vector<std::string>& rawArgs) const;
    void addSubCommand(const std::string& name, const Command& newSubCommand);
private:
    std::map<std::string, Command> subCommands;
    std::function<void()> callback;
};

class Parser {
public:
    static void parse (const int argc, char const*const* argv, bool splitFlags = false) {
        for (int i = 1; i < argc; ++i) {
            std::string current = argv[i];
            if (splitFlags && std::regex_match(current, std::regex("^(-[a-zA-Z]{2,})(=.*$|$)"))) {
                for (int j = 1; j < current.size() && current[j - 1] != '='; ++j)
                    tokens.push_back((current[j] != '=') ? std::string{'-', current[j]} : current.substr(j + 1));
            } else {
                size_t equal_pos = current.find_first_of('=');
                if (equal_pos == std::string::npos)
                    tokens.push_back(current);
                else {
                    tokens.push_back(current.substr(0, equal_pos));
                    tokens.push_back(current.substr(equal_pos + 1));
                }
            }
        }

        defaultCommand->run(tokens);
    }

    static std::vector<std::string> tokens;
    static Command* defaultCommand;
};

std::vector<std::string> Parser::tokens;
Command* Parser::defaultCommand;

void Command::run(std::vector<std::string> &rawArgs) const {
    if (!rawArgs.empty()) {
        for (const auto& command : subCommands) {
            if (rawArgs[0] == command.first) {
                rawArgs.erase(rawArgs.begin());
                command.second.run(rawArgs);
                return;
            }
        }
    }

    callback();
}

void Command::addSubCommand(const std::string& name, const Command& newSubCommand) {
    subCommands.emplace(name, newSubCommand);
}

template<typename Func, typename... Args>
Command::Command(Func function, Args&... args) : callback([function, &args...](){function(args...);}) { }

void Command::setAsDefault() {
    Parser::defaultCommand = this;
}

#endif //CLIAPP_CLILIB_HPP
