//
// Created by user on 2/2/2021.
//

#include "../include/Command.h"

Command::Command(std::string name, std::function<void()> toExecute) : name(std::move(name)), toExecute(std::move(toExecute)) { }

const std::string &Command::getName() const {
    return name;
}

void Command::run() {
    toExecute();
}