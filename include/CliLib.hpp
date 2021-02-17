#ifndef CLIAPP_CLILIB_HPP
#define CLIAPP_CLILIB_HPP

#include <regex>
#include <iostream>

enum class Policy {
    REQUIRED,
    ANYOF,
    OPTIONAL,
    ONEOF
};

class OptionGroup {
public:
    OptionGroup(Policy p, std::string description);

    void addOption(const std::string& flag, const std::string& desc, const std::string& alternativeFlag="");

    Policy policy;
    std::string groupDescription;
    std::map<std::string, std::pair<std::string, std::string>> options;
};

class Command {
public:
    template<typename Func, typename... Args>
    explicit Command(std::string description, Func function, Args&... args);

    void setAsDefault();
    void addSubCommand(const std::string& name, Command* newSubCommand);
    void addOptionGroup(const OptionGroup& group);

    void run(std::vector<std::string>& rawArgs) const;
    bool validateOptions() const;
    void printHelp(const std::string& title = "") const;
    const std::string &getDescription() const;

private:
    std::map<std::string, Command*> subCommands;
    std::vector<OptionGroup> optionGroups;
    std::function<void()> callback;
    std::string description;

    bool isOption(const std::string& str) const;
};

class Parser {
public:
    static void parse (const int& argc, char const*const* argv, bool noRemainder = true, bool splitFlags = false);
    static void run();

    template<typename T>
    static T getConverted(const std::string& option, const std::string& longOption = "", T defaultValue = T());
    template<typename T>
    static std::vector<T> getMultiConverted(const std::string& option, const std::string& longOption = "", std::initializer_list<T> defaultInit = {});

    static bool isSet(const std::string &option);
    static bool hasOptionSyntax(const std::string& str);

    static bool noRemainder;
    static bool splitFlags;
    static std::vector<std::string> tokens;
    static Command* defaultCommand;
private:

    static std::string getRaw (const std::string& option, const std::string& longOption = "");
    static std::vector<std::string> getMultiRaw(const std::string& option, const std::string& longOption = "");
};

//OptionGroup
OptionGroup::OptionGroup(Policy p, std::string description) : policy(p), groupDescription(std::move(description)) {}

void OptionGroup::addOption(const std::string &flag, const std::string &desc, const std::string &alternativeFlag) {
    options[flag] = std::make_pair(desc, alternativeFlag);
}

//Command
template<typename Func, typename... Args>
Command::Command(std::string description, Func function, Args&... args) : description(description), callback([function, &args...](){function(args...);}) { }

void Command::setAsDefault() {
    Parser::defaultCommand = this;
}

void Command::addSubCommand(const std::string& name, Command* newSubCommand) {
    subCommands.emplace(name, newSubCommand);
}

void Command::addOptionGroup(const OptionGroup& group) {
    optionGroups.push_back(group);
}

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

    if (Parser::isSet("--help") || Parser::isSet("-h")) {
        printHelp("Command usage");
        exit(0);
    }

    if (!validateOptions()) {
        std::cerr << "No/Invalid parameters provided (Use --help for more information)";
        exit(0);
    }

    callback();
}

bool Command::validateOptions() const {
    if (Parser::noRemainder)
        for (const std::string& token : Parser::tokens)
            if (Parser::hasOptionSyntax(token) && !isOption(token))
                return false;

    bool valid = true;

    for (const auto& group : optionGroups) {
        bool wasOne = false;
        for (const auto& option : group.options) {
            valid = Parser::isSet(option.first) || Parser::isSet(option.second.second);
            if ((group.policy == Policy::REQUIRED && !valid) || (group.policy == Policy::ANYOF && valid))
                break;
            else if (group.policy == Policy::ONEOF && valid && !wasOne)
                wasOne = true;
            else if (group.policy == Policy::ONEOF && valid && wasOne)
                valid = false;
            else if ((group.policy == Policy::OPTIONAL) || (group.policy == Policy::ONEOF && !valid && wasOne))
                valid = true;
        }
        if (!valid)
            break;
    }

    return valid;
}

void Command::printHelp(const std::string &title) const {
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

        for (const auto& group : optionGroups) {
            std::cout << "\n[" + group.groupDescription + "]\n";
            for (auto& option : group.options)
                std::cout << "\t" << option.first << (option.second.second.empty() ? "" : ", " + option.second.second) << " - " << option.second.first << std::endl;
        }
    }
}

const std::string &Command::getDescription() const {
    return description;
}

bool Command::isOption(const std::string &str) const {
    for (const auto& group : optionGroups) {
        for (const auto& o : group.options)
            if (str==o.first || str==o.second.second || (Parser::splitFlags && str[0] == '-')) return true;
    }
    return false;
}

//Parser
void Parser::parse(const int& argc, const char* const* argv, bool noRemainder, bool splitFlags) {
    Parser::splitFlags = splitFlags;
    Parser::noRemainder = noRemainder;

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
}

void Parser::run() {
    defaultCommand->run(tokens);
}

std::string Parser::getRaw(const std::string &option, const std::string &longOption) {
    auto itr = std::find(tokens.begin(), tokens.end(), option);
    auto itrLong = std::find(tokens.begin(), tokens.end(), longOption);
    if (itr != tokens.end() && ++itr != tokens.end() && !hasOptionSyntax(*itr))
        return *itr;
    else if (itrLong != tokens.end() && ++itrLong != tokens.end() && !hasOptionSyntax(*itrLong))
        return *itrLong;
    else
        return "";
}

std::vector<std::string> Parser::getMultiRaw(const std::string &option, const std::string &longOption) {
    std::vector<std::string> params;
    auto itr = std::find(tokens.begin(), tokens.end(), option);
    auto itrLong = std::find(Parser::tokens.begin(), tokens.end(), longOption);
    while (itr != tokens.end() && ++itr != tokens.end() && !hasOptionSyntax(*itr)) {
        params.push_back(*itr);
    }
    while (itrLong != tokens.end() && ++itrLong != tokens.end() && !hasOptionSyntax(*itrLong)) {
        params.push_back(*itrLong);
    }
    return params;
}

template<typename T>
T Parser::getConverted(const std::string &option, const std::string &longOption, T defaultValue) {
    std::string rawValue = getRaw(option, longOption);

    if (rawValue.empty() && !(isSet(option) || isSet(longOption)))
        return defaultValue;
    else if (rawValue.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"";
        exit(0);
    }

    std::stringstream sBuffer;
    sBuffer << rawValue;

    T convertedValue;
    sBuffer >> convertedValue;

    return convertedValue;
}

template<>
std::string Parser::getConverted(const std::string &option, const std::string& longOption, std::string defaultValue) {
    std::string rawValue = getRaw(option, longOption);

    if (rawValue.empty() && !(isSet(option) || isSet(longOption)))
        return defaultValue;
    else if (rawValue.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"";
        exit(0);
    }

    return rawValue;
}

template<>
bool Parser::getConverted(const std::string &option, const std::string& longOption, bool defaultValue) {
    std::string rawValue = getRaw(option, longOption);

    if (rawValue.empty() && !(isSet(option) || isSet(longOption)))
        return defaultValue;
    else if (rawValue.empty())
        return true;

    std::stringstream sBuffer;
    sBuffer << rawValue;

    bool convertedValue;
    sBuffer >> std::boolalpha >> convertedValue;

    return convertedValue;
}

template<typename T>
std::vector<T> Parser::getMultiConverted(const std::string &option, const std::string &longOption, std::initializer_list<T> defaultInit) {
    std::vector<std::string> params = getMultiRaw(option, longOption);
    std::vector<T> convertedParams;

    if (params.empty() && !(isSet(option) || isSet(longOption)))
        return defaultInit;
    else if (params.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"";
        exit(0);
    }

    std::stringstream sBuffer;

    T value;
    for (const auto& param : params) {
        sBuffer << param << " ";
        sBuffer >> value;
        convertedParams.push_back(value);
    }

    return convertedParams;
}

template<>
std::vector<std::string> Parser::getMultiConverted(const std::string &option, const std::string &longOption, std::initializer_list<std::string> defaultInit) {
    std::vector<std::string> convertedParams = getMultiRaw(option, longOption);

    if (convertedParams.empty() && !(isSet(option) || isSet(longOption)))
        return defaultInit;
    else if (convertedParams.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"";
        exit(0);
    }

    return convertedParams;
}

template<>
std::vector<bool> Parser::getMultiConverted(const std::string &option, const std::string &longOption, std::initializer_list<bool> defaultInit) {
    std::vector<std::string> params = getMultiRaw(option, longOption);
    std::vector<bool> convertedParams;

    if (params.empty() && !(isSet(option) || isSet(longOption)))
        return defaultInit;
    else if (params.empty())
        return {true};

    std::stringstream sBuffer;

    bool value;
    for (const auto& param : params) {
        sBuffer << param << " ";
        sBuffer >> std::boolalpha >> value;
        convertedParams.push_back(value);
    }

    return convertedParams;
}

bool Parser::isSet(const std::string &option) {
    return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
}

bool Parser::hasOptionSyntax(const std::string& str) {
    return std::regex_match(str, std::regex("^(-{1,2}[a-zA-Z0-9]{1,})"));
}

bool Parser::noRemainder;
bool Parser::splitFlags;
std::vector<std::string> Parser::tokens;
Command* Parser::defaultCommand;

#endif //CLIAPP_CLILIB_HPP