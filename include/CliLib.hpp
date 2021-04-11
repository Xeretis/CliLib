#ifndef CLIAPP_CLILIB_HPP
#define CLIAPP_CLILIB_HPP

#include <regex>
#include <iostream>
#include <utility>

enum class FlagPolicy {
    REQUIRED,
    OPTIONAL,
    ANYOF,
    ONEOF
};

enum class PositionalPolicy {
    REQUIRED,
    OPTIONAL
};

struct FlagOption {
    FlagOption(std::string opt, std::string desc, std::string longOption = "");

    std::string opt;
    std::string desc;
    std::string longOption;
};

struct PositionalOption {
    PositionalOption(const unsigned int& pos, std::string desc);

    unsigned int pos;
    std::string desc;
};

class OptionGroup {
public:
    explicit OptionGroup(std::string description, FlagPolicy fp = FlagPolicy::REQUIRED, PositionalPolicy pp = PositionalPolicy::REQUIRED);

    //? flag options
    void addOption(FlagOption* single);
    template<typename... Opts>
    void addOption(FlagOption* first, Opts... opts);

    //? positional options
    void addOption(PositionalOption* single);
    template<typename... Opts>
    void addOption(PositionalOption* first, Opts... opts);


    ~OptionGroup();

    FlagPolicy flagPolicy;
    PositionalPolicy positionalPolicy;
    std::vector<FlagOption*> flagOptions;
    std::vector<PositionalOption*> positionalOptions;
    std::string groupDescription;
};

class Command {
public:
    template<typename Func, typename... Args>
    explicit Command(std::string description, Func function, Args&... args);

    template<typename... Names>
    void addSubCommand(Command* newSubCommand, Names... names);
    void addOptionGroup(OptionGroup* group);
    template<typename... Groups>
    void addOptionGroup(Groups... groups);
    void setNoReaminder(bool newNoRemainder);

    void run();

    bool validateOptions() const;
    void printHelp(const std::string& title = "") const;
    const std::string &getDescription() const;

private:
    std::map<std::vector<std::string>, Command*> subCommands;
    std::vector<OptionGroup*> optionGroups;
    std::function<void()> commandFunction;
    std::string description;
    bool noRemainder = true;

    bool isOption(const std::string& str) const;
};

class Parser {
public:
    static void parse (const int& argc, char const*const* argv, bool splitFlags = false);

    //FlagOption
    template<typename T>
    static T getConverted(const std::string& option, const std::string& longOption = "", const T& defaultValue = T());
    template<typename T>
    static std::vector<T> getMultiConverted(const std::string& option, const std::string& longOption = "", std::initializer_list<T> defaultInit = {});

    //PositionalOption
    template<typename T>
    static T getConverted(const unsigned int& pos, const unsigned int& indent = 0, const T& defaultValue = T());
    template<typename T>
    static std::vector<T> getMultiConverted(const unsigned int& pos, const unsigned int& indent = 0, std::initializer_list<T> defaultInit = {});

    static bool isSet(const std::string &option);
    static bool hasOptionSyntax(const std::string& str);

    static std::vector<std::string> tokens;
private:

    static std::string getFlagRaw (const std::string& option, const std::string& longOption = "");
    static std::vector<std::string> getMultiFlagRaw(const std::string& option, const std::string& longOption = "");

    static std::string getPositionalRaw (const unsigned int& pos, const unsigned int& indent);
    static std::vector<std::string> getMultiPositionalRaw (const unsigned int& pos, const unsigned int& indent);
};

//FlagOption

FlagOption::FlagOption(std::string opt, std::string desc, std::string longOption) : opt(std::move(opt)), desc(std::move(desc)), longOption(std::move(longOption)) { }

//PositionalOption

PositionalOption::PositionalOption(const unsigned int& pos, std::string desc) : pos(pos), desc(std::move(desc)) { }

//OptionGroup
OptionGroup::OptionGroup(std::string description, FlagPolicy fp, PositionalPolicy pp) : groupDescription(std::move(description)), flagPolicy(fp), positionalPolicy(pp) {}

void OptionGroup::addOption(FlagOption* single) {
    flagOptions.emplace_back(single);
}

template<typename... Opts>
void OptionGroup::addOption(FlagOption* first, Opts... opts) {
    flagOptions.emplace_back(first);
    for (const auto& opt : {opts...})
        flagOptions.emplace_back(opt);
}

void OptionGroup::addOption(PositionalOption* single) {
    positionalOptions.emplace_back(single);
}

template<typename... Opts>
void OptionGroup::addOption(PositionalOption* first, Opts... opts) {
    positionalOptions.emplace_back(first);
    for (const auto& opt : {opts...})
        flagOptions.emplace_back(opt);
}

OptionGroup::~OptionGroup() {
    for (FlagOption* option : flagOptions)
        delete option;

    for (PositionalOption* positionalOption : positionalOptions)
        delete positionalOption;
}

//Command
template<typename Func, typename... Args>
Command::Command(std::string description, Func function, Args&... args) : description(std::move(description)), commandFunction([function, &args...](){function(args...);}) { }

template<typename... Names>
void Command::addSubCommand(Command* newSubCommand, Names... names) {
    subCommands.emplace(std::vector<std::string>{names...}, newSubCommand);
}

void Command::addOptionGroup(OptionGroup* group) {
    optionGroups.emplace_back(group);
}

template<typename... Groups>
void Command::addOptionGroup(Groups... groups) {
    for (const auto& group : {groups...})
        optionGroups.emplace_back(group);
}

void Command::setNoReaminder(bool newNoRemainder) {
    noRemainder = newNoRemainder;
}

void Command::run() {
    if (!Parser::tokens.empty()) {
        for (const auto& command : subCommands)
            for (const auto& name : command.first)
                if (Parser::tokens[0] == name) {
                    Parser::tokens.erase(Parser::tokens.begin());
                    command.second->run();
                    return;
                }

        bool hasFirstPositional = false;
        for (const auto& group : optionGroups)
            for (const auto& positionalOption : group->positionalOptions)
                if (positionalOption->pos == 0)
                    hasFirstPositional = true;

        if (!Parser::hasOptionSyntax(Parser::tokens[0]) && !hasFirstPositional) {
            std::cerr << "\"" << Parser::tokens[0] << "\" is not a valid command\n";
            exit(0);
        }
    }

    if (Parser::isSet("--help") || Parser::isSet("-h")) {
        printHelp("Command usage");
        exit(0);
    }

    if (!validateOptions()) {
        std::cerr << "No/Invalid parameters provided (Use --help for more information)\n";
        exit(0);
    }

    commandFunction();
}

bool Command::validateOptions() const {
    if (noRemainder)
        for (const std::string& token : Parser::tokens)
            if (Parser::hasOptionSyntax(token) && !isOption(token))
                return false;

    bool valid = true;

    for (const auto& group : optionGroups) {
        bool wasOne = false;
        for (const auto& option : group->flagOptions) {
            valid = Parser::isSet(option->opt) || Parser::isSet(option->longOption);

            if ((group->flagPolicy == FlagPolicy::REQUIRED && !valid) || (group->flagPolicy == FlagPolicy::ANYOF && valid))
                break;
            else if (group->flagPolicy == FlagPolicy::ONEOF && valid && !wasOne)
                wasOne = true;
            else if (group->flagPolicy == FlagPolicy::ONEOF && valid && wasOne) {
                valid = false;
                break;
            } else if ((group->flagPolicy == FlagPolicy::OPTIONAL) || (group->flagPolicy == FlagPolicy::ONEOF && !valid && wasOne))
                valid = true;
        }
        if (!valid)
            break;

        for (const auto& positionalOption : group->positionalOptions) {
            valid = (positionalOption->pos < Parser::tokens.size());

            if (group->positionalPolicy == PositionalPolicy::REQUIRED && !valid)
                break;
            else if (group->positionalPolicy == PositionalPolicy::OPTIONAL)
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
            std::cout << "\t";
            for (const auto& name : command.first)
                std::cout << name << (name != *--command.first.end() ? ", " : "");

            std::cout << " - " << command.second->getDescription() << "\n";
        }

        std::cout << "\n";
    }

    if (!optionGroups.empty()) {
        std::string policyNames[] = {"REQUIRED", "OPTIONAL", "ANYOF", "ONEOF"};

        std::cout << "Options:";

        for (const auto& group : optionGroups) {
            std::cout << "\n[" << group->groupDescription << "] | (Flag Policy: " << policyNames[static_cast<int>(group->flagPolicy)] << ") (Positional Policy: " << policyNames[static_cast<int>(group->positionalPolicy)] << ")\n";

            for (const auto& option : group->flagOptions)
                std::cout << "\t" << option->opt << (option->longOption.empty() ? "" : ", " + option->longOption) << " - " << option->desc << std::endl;

            for (const auto& positinalOption : group->positionalOptions)
                std::cout << "\tPosition: " << positinalOption->pos << " - " << positinalOption->desc << std::endl;
        }
    }
}

const std::string &Command::getDescription() const {
    return description;
}

bool Command::isOption(const std::string &str) const {
    for (const auto& group : optionGroups)
        for (const auto& option : group->flagOptions)
            if (str == option->opt || str == option->longOption)
                return true;

    return false;
}

//Parser
void Parser::parse(const int& argc, const char* const* argv, bool splitFlags) {
    for (int i = 1; i < argc; ++i) {
        std::string current = argv[i];
        if (splitFlags && std::regex_match(current, std::regex("^(-[a-zA-Z]{2,})(=.*$|$)")))
            for (int j = 1; j < current.size() && current[j - 1] != '='; ++j)
                tokens.emplace_back((current[j] != '=') ? std::string{'-', current[j]} : current.substr(j + 1));
        else {
            size_t equal_pos = current.find_first_of('=');
            if (equal_pos == std::string::npos)
                tokens.emplace_back(current);
            else {
                tokens.emplace_back(current.substr(0, equal_pos));
                tokens.emplace_back(current.substr(equal_pos + 1));
            }
        }
    }
}

std::string Parser::getFlagRaw(const std::string &option, const std::string &longOption) {
    auto itr = std::find(tokens.begin(), tokens.end(), option);
    auto itrLong = std::find(tokens.begin(), tokens.end(), longOption);

    if (itr != tokens.end() && ++itr != tokens.end() && !hasOptionSyntax(*itr))
        return *itr;
    else if (itrLong != tokens.end() && ++itrLong != tokens.end() && !hasOptionSyntax(*itrLong))
        return *itrLong;

    return "";
}

std::vector<std::string> Parser::getMultiFlagRaw(const std::string &option, const std::string &longOption) {
    std::vector<std::string> values;

    auto itr = std::find(tokens.begin(), tokens.end(), option);
    auto itrLong = std::find(Parser::tokens.begin(), tokens.end(), longOption);

    while (itr != tokens.end() && ++itr != tokens.end() && !hasOptionSyntax(*itr))
        values.emplace_back(*itr);

    while (itrLong != tokens.end() && ++itrLong != tokens.end() && !hasOptionSyntax(*itrLong))
        values.emplace_back(*itrLong);

    return values;
}

std::string Parser::getPositionalRaw(const unsigned int& pos, const unsigned int& indent) {
    if (tokens.size() - 1 < (indent + pos) || tokens.empty())
        return "";

    return tokens[indent + pos];
}

std::vector<std::string> Parser::getMultiPositionalRaw(const unsigned int& pos, const unsigned int& indent) {
    std::vector<std::string> values;

    for (unsigned int i = (indent + pos); i < tokens.size(); ++i)
        values.emplace_back(tokens[i]);

    return values;
}

//FlagOption "getters"
template<typename T>
T Parser::getConverted(const std::string &option, const std::string &longOption, const T& defaultValue) {
    std::string rawValue = getFlagRaw(option, longOption);

    if (rawValue.empty() && !(isSet(option) || isSet(longOption)))
        return defaultValue;
    else if (rawValue.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"\n";
        exit(0);
    }

    std::stringstream sBuffer;
    T convertedValue;

    sBuffer << rawValue;
    sBuffer >> convertedValue;

    return convertedValue;
}

template<>
std::string Parser::getConverted(const std::string &option, const std::string& longOption, const std::string& defaultValue) {
    std::string rawValue = getFlagRaw(option, longOption);

    if (rawValue.empty() && !(isSet(option) || isSet(longOption)))
        return defaultValue;
    else if (rawValue.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"\n";
        exit(0);
    }

    return rawValue;
}

template<>
bool Parser::getConverted(const std::string &option, const std::string& longOption, const bool& defaultValue) {
    if (!(isSet(option) || isSet(longOption)))
        return defaultValue;

    return true;
}

template<typename T>
std::vector<T> Parser::getMultiConverted(const std::string &option, const std::string &longOption, std::initializer_list<T> defaultInit) {
    std::vector<std::string> rawValues = getMultiFlagRaw(option, longOption);
    if (rawValues.empty() && !(isSet(option) || isSet(longOption)))
        return defaultInit;
    else if (rawValues.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"\n";
        exit(0);
    }

    std::stringstream sBuffer;
    std::vector<T> values;


    T value;
    for (const auto& rawValue : rawValues) {
        sBuffer << rawValue;
        sBuffer >> value;
        values.emplace_back(value);
    }

    return values;
}

template<>
std::vector<std::string> Parser::getMultiConverted(const std::string &option, const std::string &longOption, std::initializer_list<std::string> defaultInit) {
    std::vector<std::string> rawValues = getMultiFlagRaw(option, longOption);

    if (rawValues.empty() && !(isSet(option) || isSet(longOption)))
        return defaultInit;
    else if (rawValues.empty()) {
        std::cerr << "No value provided for \"" << option << "/" << longOption << "\"\n";
        exit(0);
    }

    return rawValues;
}

template<>
std::vector<bool> Parser::getMultiConverted(const std::string &option, const std::string &longOption, std::initializer_list<bool> defaultInit) {
    std::vector<std::string> rawValues = getMultiFlagRaw(option, longOption);

    if (rawValues.empty() && !(isSet(option) || isSet(longOption)))
        return defaultInit;
    else if (rawValues.empty())
        return {true};

    std::stringstream sBuffer;
    std::vector<bool> values;

    sBuffer.setf(std::ios_base::boolalpha);

    bool value;
    for (const auto& rawValue : rawValues) {
        sBuffer << rawValue;
        sBuffer >> value;
        values.emplace_back(value);
    }

    return values;
}

//PositionalOption "getters"
template<typename T>
T Parser::getConverted(const unsigned int& pos, const unsigned int& indent, const T& defaultValue) {
    std::string rawValue = getPositionalRaw(pos, indent);

    if(rawValue.empty())
        return defaultValue;

    std::stringstream sBuffer;
    T value;

    sBuffer << rawValue;
    sBuffer >> value;

    return value;
}

template<>
std::string Parser::getConverted(const unsigned int& pos, const unsigned int& indent, const std::string& defaultValue) {
    std::string rawValue = getPositionalRaw(pos, indent);

    if (rawValue.empty())
        return defaultValue;

    return rawValue;
}

template<>
bool Parser::getConverted(const unsigned int& pos, const unsigned int& indent, const bool& defaultValue) {
    std::string rawValue = getPositionalRaw(pos, indent);

    if (rawValue.empty())
        return defaultValue;

    std::stringstream sBuffer;
    bool value;

    sBuffer << rawValue;
    sBuffer >> std::boolalpha >> value;

    return value;
}

template<typename T>
std::vector<T> Parser::getMultiConverted(const unsigned int& pos, const unsigned int& indent, std::initializer_list<T> defaultInit) {
    std::vector<std::string> rawValues = getMultiPositionalRaw(pos, indent);

    if (rawValues.empty())
        return defaultInit;

    std::stringstream sBuffer;
    std::vector<T> values;

    T value;
    for (const auto& rawValue : rawValues) {
        sBuffer << rawValue;
        sBuffer >> value;
        values.emplace_back(value);
    }

    return values;
}

template<>
std::vector<std::string> Parser::getMultiConverted(const unsigned int& pos, const unsigned int& indent, std::initializer_list<std::string> defaultInit) {
    std::vector<std::string> rawValues = getMultiPositionalRaw(pos, indent);

    if (rawValues.empty())
        return defaultInit;

    return rawValues;
}

template<>
std::vector<bool> Parser::getMultiConverted(const unsigned int& pos, const unsigned int& indent, std::initializer_list<bool> defaultInit) {
    std::vector<std::string> rawValues = getMultiPositionalRaw(pos, indent);

    if (rawValues.empty())
        return defaultInit;

    std::stringstream sBuffer;
    std::vector<bool> values;

    sBuffer.setf(std::ios_base::boolalpha);

    bool value;
    for (const auto& rawValue : rawValues) {
        sBuffer << rawValue;
        sBuffer >> value;
        values.emplace_back(value);
    }

    return values;
}

bool Parser::isSet(const std::string &option) {
    return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
}

bool Parser::hasOptionSyntax(const std::string& str) {
    return std::regex_match(str, std::regex("^(-{1,2}[a-zA-Z0-9_]{1,})"));
}

std::vector<std::string> Parser::tokens;

#endif //CLIAPP_CLILIB_HPP