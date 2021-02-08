//
// Created by user on 2/2/2021.
//

#include <iostream>

#include "../include/Cli.h"
#include "../include/Utils.h"

std::vector<std::string> Cli::commandArgs;
std::string Cli::command;

void Cli::addCommand(Command* newCommand) {
    commands.push_back(newCommand);
}

void Cli::setDefaultFunction(const std::function<void()> &newDefaultFunction) {
    defaultFunction = newDefaultFunction;
}

const std::vector<std::string> &Cli::getCommandArgs() {
    return commandArgs;
}

void Cli::operator()(int argc, char** argv) {
    Cli::initArgs(argc, argv);
    this->checkCommand();
}

Cli::~Cli() {
    for (Command* element : commands)
        delete element;
}

void Cli::initArgs(int argc, char** argv) {
    if (argc > 1)
        command = argv[1];
    for(int i = 2; i < argc; ++i) {
        commandArgs.emplace_back(argv[i]);
    }
}

void Cli::checkCommand() {
    for(Command* element : commands) {
        if(element->getName() == command) {
            element->run();
            return;
        }
    }
    if (defaultFunction) {
        commandArgs.emplace_back(command);
        defaultFunction();
    } else if (!command.empty())
        std::cerr << "\"" << command << "\" is not a valid command.";
    else
        std::cerr << "No valid command provided.";

}