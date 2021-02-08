//
// Created by user on 2/2/2021.
//


#ifndef CLIAPP_CLI_H
#define CLIAPP_CLI_H

#include <vector>
#include <string>

#include "Command.h"

class Cli {
public:
    template<typename... T>
    Cli(T&&...t) : commands({t...}) {}

    void addCommand(Command* newCommand);
    void setDefaultFunction(const std::function<void()> &newDefaultFunction);
    static const std::vector<std::string> &getCommandArgs();

    void operator() (int argc, char** argv);

    ~Cli();

private:
    static std::vector<std::string> commandArgs;
    static std::string command;
    std::vector<Command*> commands;
    std::function<void()> defaultFunction = nullptr;

    static void initArgs(int argc, char ** argv);
    void checkCommand();
};

#endif //CLIAPP_CLI_H