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

void test3() {
    std::cout << "Success! 3!";
}

int main(int argc, char** argv) {
    Parser::parse(argc, argv);

    std::vector<std::string> a;

    Command def ("Desc1", test1, a);
    Command tst ("Desc2", test2);
    Command fsf ("Desc3", test3);
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

    tst.addSubCommand("fsf", &fsf);

    a = def.getMultiConverted<std::string>("-a", "--abc", {"asd"});

    Parser::run();
}