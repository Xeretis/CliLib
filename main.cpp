/*
 *
 *
 * DO NOT USE THIS FILE AS A REFERENCE TO FUNCTIONALITY. IT IS ONLY USED FOR TESTING.
 *
 *
*/

#include <iostream>
#include <Cli.h>
#include <Utils.h>
#include <Args.h>

void test(std::string i = Arg<std::string>({"-a", "--a"}), std::string a = Arg<std::string>(1)) {
    std::cout << "test\n" << i << " " << a;
}

void nottest() {
    std::cout << Utils::getWorkingDir();
    std::cout << "\nnottest\n";
}

int main(int argc, char** argv) {
//    Cli mc;
    Cli mc = {new Command("test", [](){test();}), new Command("nottest", nottest)};
    mc.setDefaultFunction([](){test();});
//    mc.addCommand(new Command("test", COMMANDFUNCTION(test)));
//    mc.addCommand(new Command("nottest", nottest));

    mc(argc, argv);
}