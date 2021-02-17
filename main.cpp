#include <CliLib.hpp>
#include <iostream>

void test1(std::vector<std::string> a) {
    for (std::string i : a) {
        std::cout << i << "\t";
    }
}

void test2() {
    std::cout << "Success! 2!";
}

void test3(int b) {
    std::cout << b;
}

int main(int argc, char** argv) {
    Parser::parse(argc, argv);

    std::vector<std::string> a = Parser::getMultiConverted<std::string>("-a", "--abc", {"asd"});;
    Command def ("Desc1", test1, a);

    Command tst ("Desc2", test2);

    int b = Parser::getConverted<int>("-c", "--bb");
    Command fsf ("Desc3", test3, b);
    def.setAsDefault();
    def.addSubCommand("tst", &tst);

    OptionGroup defReq (Policy::OPTIONAL, "test group");
    defReq.addOption("-a", "description", "--abc");
    def.addOptionGroup(defReq);

    OptionGroup defOpt (Policy::ONEOF, "OPTIONAL group");
    defOpt.addOption("-opt", "a nice description");
    defOpt.addOption("-oopt", "an other nice description");
    defOpt.addOption("-ooopt", "an other nice description");
    def.addOptionGroup(defOpt);

    OptionGroup nestedTest (Policy::REQUIRED, "ayyy");
    nestedTest.addOption("-b", "an alright description", "--bb");
    fsf.addOptionGroup(nestedTest);

    tst.addSubCommand("fsf", &fsf);

    Parser::run();
}