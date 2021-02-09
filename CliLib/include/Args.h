//
// Created by user on 2/2/2021.
//

#ifndef CLIAPP_ARGS_H
#define CLIAPP_ARGS_H

#include <initializer_list>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <iostream>

#include "Cli.h"

template<typename T>
class Arg {
public:
    Arg(std::initializer_list<std::string> aliases) : identifier(aliases) {}
    Arg(std::initializer_list<std::string> aliases, T defaultValue) : identifier(aliases), defaultValue(defaultValue) {}
    Arg(int pos) : identifier(pos) {}
    Arg(int pos, T defaultValue) : identifier(pos), defaultValue(defaultValue) {}

    operator T() {
        return findValue();
    }
private:
    std::variant<std::vector<std::string>, int> identifier;
    T defaultValue = T();
    std::string name;
    std::string rawValue;

    T findValue() {
        switch (identifier.index()) {
            case 0: {
                std::vector<std::string> aliases = std::get<0>(identifier);
                std::reverse(aliases.begin(), aliases.end()); // so an error is thrown with the first alias if needed

                std::vector<std::string> args = Cli::getCommandArgs();

                for (const std::string& alias : aliases) {
                     for (std::string arg : args) {
                        if (alias.rfind(arg, 0) || alias == arg) {
                            int equalsPos = arg.find_last_of('=');
                            if (equalsPos == -1) {
                                name = alias;
                            } else {
                                name = arg.substr(0, equalsPos);
                                rawValue = arg.erase(0, equalsPos + 1);
                            }
                        } else {
                            name = arg;
                        }
                    }
                }

                if (rawValue.empty() && defaultValue == T()) {
                    std::cerr << "No value provided for \"" << name <<"\"";
                    exit(0);
                }

                if (!rawValue.empty()) {
                    std::stringstream valueStream(rawValue);
                    T value;
                    if(valueStream >> value)
                        return value;
                    else {
                        std::cerr << "\"" << rawValue << "\" is not a valid value for \"" << name << "\"";
                        exit(0);
                    }
                } else {
                    return defaultValue;
                }
            }
            case 1:
                int pos = std::get<1>(identifier);
                std::vector<std::string> args = Cli::getCommandArgs();

                name = std::to_string(pos + 1); //pos + 1 so it doesnt start with 0
                if (pos < args.size())
                    rawValue = args[pos];
                if (rawValue.empty() && defaultValue == T()) {
                    std::cerr << "No value provided for argument number " << name;
                    exit(0);
                }

                if (!rawValue.empty()) {
                    std::stringstream valueStream(rawValue);
                    T value;
                    if(valueStream >> value)
                        return value;
                    else {
                        std::cerr << "\"" << rawValue << "\" is not a valid argument number " << name;
                        exit(0);
                    }
                } else {
                    return defaultValue;
                }
        }
        return T(); //just for the compilers sake so I don't get warnings
    }
};


#endif //CLIAPP_ARGS_H
