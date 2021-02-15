#include <CliLib.hpp>
#include <iostream>

void test1() {
    std::cout << "Success!";
}

void test2() {
    std::cout << "Success! 2!";
}

void test3() {
    std::cout << "Success! 3!";
}

int main(int argc, char** argv) {
    Command def (test1);
    Command tst (test2);
    Command fsf (test3);
    def.setAsDefault();
    def.addSubCommand("tst", &tst);
    tst.addSubCommand("fsf", &fsf);

    Parser::parse(argc, argv);
}