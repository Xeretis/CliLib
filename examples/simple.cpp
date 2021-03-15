#include <CliLib.hpp>
#include <iostream>

enum class Mode {FIRST, SECOND, THIRD};

void cliFunc(Mode mode, std::vector<std::string> args) {
    switch (mode) {
        case Mode::FIRST:
            std::cout << "First mode called with arguments: ";
            for (const auto& arg : args)
                std::cout << arg << " ";
            break;
        case Mode::SECOND:
            std::cout << "Second mode called with arguments: ";
            for (const auto& arg : args)
                std::cout << arg << " ";
            break;
        case Mode::THIRD:
            std::cout << "Third mode called with arguments: ";
            for (const auto& arg : args)
                std::cout << arg << " ";
            break;
    }
}

int main(int argc, char** argv) {
    Parser::parse(argc, argv);

    Mode mode = Parser::getConverted<bool>("-f") ? Mode::FIRST : (Parser::getConverted<bool>("-s") ? Mode::SECOND : Mode::THIRD);
    std::vector<std::string> args = Parser::getMultiConverted<std::string>("-a", "--args");

    Command cliCommand ("A simple example with only a single command", cliFunc, mode, args);
    OptionGroup modeGroup (Policy::ONEOF, "Program mode", new Option("-f", "The first mode"), new Option("-s", "The second mode"), new Option("-t", "The third mode"));
    OptionGroup argsGroup (Policy::REQUIRED, "Program args", new Option("-a", "The actual arguments given to the program", "--args"));
    cliCommand.addOptionGroup(&modeGroup, &argsGroup);

    cliCommand.setAsDefault();

    Parser::run();
}