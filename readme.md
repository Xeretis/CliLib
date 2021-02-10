# CliLib
A c++17 library aiming to make creating CLIs easier and more managable.
## Usage
### The main object
In the main function make a Cli object, commands can be added with list initialization or with the `.addCommand(Command* newCommand)` method:

`Cli mainCli = {new Command("test1", test1f), new Command("test2", [](){test2f();)}`

In the above example `"test1"` and `"test2"` are the names of the command, which are the first arguments passed when running. `test1f` and `test2f` are the names of the corresponding functions for these commands. In case the function has any parameters including default ones, a lambda should be passed (see `test2f`).

A cli can also be assigned a default function with the `.setDefaultFunction(std::function<void()> toExecute)` method. Same rules apply as for commands.

At the end of the main function `mainCli(argc, argv)` would have to be called to "run" the cli.
### Adding arguments
Arguments have to be added as a default parameter at a function declaration:

```cpp
void test2f (int a = Arg<int>(0), std::string b = Arg<std::string>({"-b", "-b2"}, "text")) {
    std::cout << a << "\n" << b;
}
```

In this case the first arg (`int a`) will be the arg at the first position after the command, with no default value, meaning it is not optional. The second arg (`std::string b`) can be used anywhere after the command (except for the first spot of course because its assigned already) by using "-b=value" or "-b2=value". String args may be surrounded with qutation marks if they contain a space (ex.: `-a="text with space"`).

Some examples of what this program would do with certain parameters:

`test2 2 -b=b` ->

```
2
b
```

`test2 5 -b2="a b c d""` ->

```
5
a b c d
```

`test2 -b=b` ->

```
"-b=b" is not a valid value for argument number 1
```
### Utils
`Utils::getWorkingDir()` - Returns the absolute path of where the program was run from.
## Known Issues
 - Bool args can only be used with numeric representations of the value. Will soon be fixed.
 - Mixing named args with positioned args can cause in some cases the named arg to get the value from the positioned arg (if the value of the positioned arg would be valid for the named arg) and the positioned arg to get the name of the named arg. I have not yet found a solution to this, partially due to the undefined evaluation order of function parameters. Help with this would be much appreciated.
 - Having multiple spaces after each other in a string arg only results in one space. Only a minor issue and will not be a priority for quite a while.
 - Algorithms used for finding the arg values are inefficient. I'm working on this.
## Todo
 - [ ] More refined bool args. (High priority)
 - [ ] More general purpose utils. (Medium priority)
 - [ ] Variadic arguments. (Medium priority)
 - [ ] A proper example project in main.cpp. (Low priority)