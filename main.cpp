#include <iostream>
#include <Cli.h>
#include <Utils.h>
#include <Args.h>

void test(int i = Arg<int>({"-a", "--a"}, 5), int a = Arg<int>(1)) {
    std::cout << "test\n" << i;
}

void nottest() {
    std::cout << "nottest\n";
}

int main(int argc, char** argv) {
//    Cli mc;
    Cli mc = {new Command("test", [](){test();}), new Command("nottest", nottest)};
    mc.setDefaultFunction([](){nottest();});
//    mc.addCommand(new Command("test", COMMANDFUNCTION(test)));
//    mc.addCommand(new Command("nottest", nottest));

    mc(argc, argv);
}