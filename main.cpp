#include <CliLib.hpp>
#include <iostream>

void test1() {
    std::cout << "Success!";
}

void test2() {
    std::cout << "Success! 2!";
}

int main(int argc, char** argv) {
    Command def (test1);
    Command tst (test2);

    def.setAsDefault();
    def.addSubCommand("tst", tst);

    Parser::parse(argc, argv);
}