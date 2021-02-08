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
    Arg(std::initializer_list<std::string> aliases) : identifier(aliases), defaultValue(T()) {}
    Arg(std::initializer_list<std::string> aliases, T defaultValue) : identifier(aliases), defaultValue(defaultValue) {}
    Arg(int pos) : identifier(pos), defaultValue(T()) {}
    Arg(int pos, T defaultValue) : identifier(pos), defaultValue(defaultValue) {}

    operator T() {
        findValue();

        if (!rawValue.empty()) {
            std::stringstream valueStream(rawValue);
            T value;
            if(valueStream >> value)
                return value;
            else {
                std::cerr << "\"" << rawValue << "\" is not a valid value for \"" << name << "\"";
                exit(0);
            }
        } else if (defaultValue == T()){
            std::cerr << "No value provided for \"" << name <<"\"";
            exit(1);
        } else {
            return defaultValue;
        }
    }
private:
    std::variant<std::vector<std::string>, int> identifier;
    T defaultValue;
    std::string name;
    std::string rawValue;

    void findValue() {
        switch (identifier.index()) {
            case 0: {
                std::vector<std::string> aliases = std::get<0>(identifier);
                std::vector<std::string> args = Cli::getCommandArgs();
                for (std::string arg : args) {
                    for (const std::string& alias : aliases) {
                        if (alias.rfind(arg, 0)) {
                            int equalsPos = arg.find_last_of('=');
                            if (equalsPos == -1) {
                                name = alias;
                                rawValue = T();
                                goto END;
                            }
                            name = arg.substr(0, equalsPos);
                            rawValue = arg.erase(0, equalsPos + 1);
                            goto END;
                        } else if (alias == arg) {
                            name = arg;
                            goto END;
                        }
                    }
                }
                END: ;
                break;
            }
            case 1:
                int pos = std::get<1>(identifier);
                std::vector<std::string> args = Cli::getCommandArgs();

                name = std::to_string(pos);
                if (pos < args.size())
                    rawValue = args[pos];
                break;
        }
    }
};


#endif //CLIAPP_ARGS_H
