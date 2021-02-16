#ifndef CLIAPP_CLILIB_HPP
#define CLIAPP_CLILIB_HPP

#include <string>
#include <regex>
#include <utility>
#include <iostream>

enum class Policy {
    required,
    anyOf,
    optional
};

struct OptionGroup {
    Policy policy;
    std::string groupDescription;
    std::map<std::string, std::pair<std::string, std::string>> options;
    OptionGroup(Policy p, std::string description) : policy(p), groupDescription(std::move(description)) {}
    void addOption(const std::string& flag, const std::string& desc, std::string alternativeFlag="") {
        options[flag] = std::make_pair(desc, alternativeFlag);
    }
};

class Command {
public:
    template<typename Func, typename... Args>
    explicit Command(std::string description, Func function, Args&... args);
    void setAsDefault();
    void run(std::vector<std::string>& rawArgs) const;
    void addSubCommand(const std::string& name, Command* newSubCommand);
    void addOptionGroup(const OptionGroup& group);
    bool checkFlags() const;
    void prinftHelp(const std::string& title = "") const;

private:
    std::map<std::string, Command*> subCommands;
    std::vector<OptionGroup> optionGroups;
    std::function<void()> callback;
    std::string description;
public:
    const std::string &getDescription() const;

private:

    bool isOption(const std::string& str) const;
};

class Parser {
public:
    static void parse (const int argc, char const*const* argv, bool splitFlags = false) {
        Parser::splitFlags = splitFlags;

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

    static bool optionExists(const std::string &option) {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

    static bool splitFlags;
    static std::vector<std::string> tokens;
    static Command* defaultCommand;
};

bool Parser::splitFlags;
std::vector<std::string> Parser::tokens;
Command* Parser::defaultCommand;

void Command::run(std::vector<std::string> &rawArgs) const {
    if (!rawArgs.empty()) {
        for (const auto& command : subCommands) {
            if (rawArgs[0] == command.first) {
                rawArgs.erase(rawArgs.begin());
                command.second->run(rawArgs);
                return;
            }
        }
    }

    if (Parser::optionExists("--help") || Parser::optionExists("-h")) {
        prinftHelp("Command usage");
        exit(0);
    }

    if (!checkFlags()) {
        prinftHelp("No/Invalid parameters provided");
        exit(0);
    }

    callback();
}

void Command::addSubCommand(const std::string& name, Command* newSubCommand) {
    subCommands.emplace(name, newSubCommand);
}

template<typename Func, typename... Args>
Command::Command(std::string description, Func function, Args&... args) : description(description), callback([function, &args...](){function(args...);}) { }

void Command::setAsDefault() {
    Parser::defaultCommand = this;
}

bool Command::checkFlags() const {
    bool valid = true;
    for (auto& group : optionGroups) {
        for (auto& option : group.options) {
            valid = Parser::optionExists(option.first) || Parser::optionExists(option.second.second);
            if (group.policy == Policy::required && !valid)
                break;
            else if (group.policy == Policy::anyOf && valid)
                break;
            else if (group.policy == Policy::optional)
                valid = true;
        }
        if (!valid)
            break;
    }
    return valid;
}

bool Command::isOption(const std::string &str) const {
    for (auto& group : optionGroups) {
        for (auto& o : group.options)
            if (str==o.first || str==o.second.second || (Parser::splitFlags && str[0] == '-')) return true;
    }
    return false;
}

void Command::addOptionGroup(const OptionGroup& group) {
    optionGroups.push_back(group);
}

void Command::prinftHelp(const std::string &title) const {
    if (!title.empty()) {
        for (char i : title)
            std::cout << "-";
        std::cout << "\n" << title << "\n";
        for (char i : title)
            std::cout << "-";
        std::cout << "\n";
    }

    std::cout << "Command description: " << description << "\n\n";

    if (!subCommands.empty()) {
        std::cout << "Subcommands: (Use --help on the subcommand for more information)\n";
        for (const auto& command : subCommands) {
            std::cout << "\t" << command.first << " - " << command.second->getDescription() << "\n";
        }
        std::cout << "\n";
    }

    if (!optionGroups.empty()) {
        std::cout << "Options:";

        for (auto& group : optionGroups) {
            std::cout << "\n[" + group.groupDescription + "]\n";
            for (auto& option : group.options)
                std::cout << "\t" << option.first << (option.second.second.empty() ? "" : ", " + option.second.second) << " - " << option.second.first << std::endl;
        }
    }
}

const std::string &Command::getDescription() const {
    return description;
}

#endif //CLIAPP_CLILIB_HPP
