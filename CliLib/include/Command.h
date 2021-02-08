//
// Created by user on 2/2/2021.
//

#ifndef CLIAPP_COMMAND_H
#define CLIAPP_COMMAND_H

#include <functional>

class Command {
public:
    Command(std::string name, std::function<void()> toExecute);
    const std::string &getName() const;

    void run();

private:
    std::function<void()> toExecute;
    std::string name;
};

#endif //CLIAPP_COMMAND_H
