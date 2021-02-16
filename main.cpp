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
    Command def ("Desc1", test1);
    Command tst ("Desc2", test2);
    Command fsf ("Desc3", test3);
    def.setAsDefault();
    def.addSubCommand("tst", &tst);

    OptionGroup defReq (Policy::required, "test group");
    defReq.addOption("-a", "description", "--abc");
    def.addOptionGroup(defReq);

    OptionGroup defOpt (Policy::optional, "optional group");
    defOpt.addOption("-opt", "a nice description");
    defOpt.addOption("-oopt", "an other nice description");
    def.addOptionGroup(defOpt);

    tst.addSubCommand("fsf", &fsf);

    Parser::parse(argc, argv);
}