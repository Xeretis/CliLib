# CliLib
A library aiming to be an easy to use and simple argument parser for CLI applications.

This project was heavily inspired by a repository called [MiniCommander](https://github.com/MichaelGrupp/MiniCommander) and it uses the same parsing algorithm (for now at least). Looking back it might have been appropriate to fork that repository but anyways, check it out if you are interested in it.
# Features
 - [x] Default option and unlimited suboptions with descriptions
 - [x] Both flag options and positional options
 - [x] Command aliases
 - [x] Option to ignore unrecognised flags or throw an error
 - [x] Single and multiple (variadic) parameters of any type
 - [x] Split short flags (unix flags) option
 - [x] Parameter groups with a name, a flag policy, a positional policy and a description
 - [x] Required, optional, anyof and oneof flag policy and required or optional positional policy for option groups
 - [x] Dynamic help command that uses the description and name of options and option groups (+ policies) to generate a help message with custom flag
 - [x] Only C++11 required
# TODO
 - [ ] Add option validators (Ex.: Option can only be set type or it can only be an odd number...)
 - [ ] Maybe add windows type flag support (?)
 - [ ] Maybe improve option parsing (?)
 - [ ] More informative error messages
 - [ ] Make it so options are set when running commands (see experimental branch)
# Usage
Start your main function with parsing the program arguments. For this use the `Parser::parse(const int& argc, char const*const* argv, bool splitFlags = false)` method.

The splitFlags parameter if enabled will split flags like `-abc` into `-a -b -c` when parsing.

## Getting option values
There are two ways to get the value of an option: get a single value or a multi value.

To get a single value use the `Parser::getConverted<T>(const std::string& option, const std::string& longOption = "", const T& defaultValue = T())` method for flag options. Use long option to specify a long version (the one starting with --) of the option if there is need for one.

For positional options use the `Parser::getConverted<T>(const unsigned int& pos, const unsigned int& indent = 0, const T& defaultValue = T())` method. The indent parameter represents how many "commands it's indented". (If it's on the default command this value should be 0, if it's on a subcommand of it then 1, if it's on a subcommand of a subcommand of the default command the 2 ...)

To get a multi value use the `Parser::getMultiConverted<T>(const std::string& option, const std::string& longOption = "", std::initializer_list<T> defaultInit = {})` method with flag options.

With positional options use `Parser::getMultiConverted(const unsigned int& pos, const unsigned int& indent = 0, std::initializer_list<T> defaultInit = {})`

These will return a vector of type T.

*Note: If noReaminder is false and we are trying to get the value from an option that deos not belong to the command but is set, it will still return it's value. I do not consider this a bug as all that has to be done for it to not happen is only using getConverted and getMultiConverted on options that belong to the command.*
## Making commands and option groups
### Commands
To make a command make an instance of type `Command`. Each command requires a description, a corresponding function, and all arguments of the function (mainly variables previously assigned a value using either `Parser::getConverted()` or `Parser::getMultiConverted()`). To use a command as the default command (the one that gets called when no command is provided) use the `.run()` method on that command at the end of your code. Every program must have a default command, but can't have more than one.

To add a subcommand use the `.addSubCommand(Command* newSubCommand, Names... names)` method. newSubCommand must be the address of a stack allocated command and names have to be strings (or a type thet can initialize a string).

To add an option group use the `.addOptionGroup(OptionGroup* newOptionGroup)` or `.addOptionGroup(Groups... groups)` Do not use a dynamically allocated pointer.

Each command uses a "noRemainder" policy by default, meaning that unrecognized options will throw an error. To turn this off use the `.setNoRemainder(bool newNoRemainder)` method with `false` as an argument. This change will not apply to subcommands.

One other thing that commands have is their help flag (`-h` and `--help`). This property can also be set. Use the `.setHelpCommand(const std::string& shortOption, const std::string& longOption = "")` method to do it.

### Option groups

To make an option group make an instance of type `OptionGroup`. Each option group requires a description but also has two policies. These policy can be any value of `enum class FlagPolicy` or `enum class PositionalPolicy`.

The constructor looks something like this: `explicit OptionGroup(std::string description, FlagPolicy fp = FlagPolicy::REQUIRED, PositionalPolicy pp = PositionalPolicy::REQUIRED);`

To add an option use the `.addOption(FlagOption* single)` or the `.addOption(FlagOption* first, Opts... opts)` method. (In the second case dynamically allocated option pointers have to be provided) Needless to say, there are two identical overloads for positional options as well.
#### Options
The constructor of the `FlagOption` class: `FlagOption(std::string opt, std::string desc, std::string longOption = "");`

And the `PositionalOption` class: `PositionalOption(const unsigned int& pos, std::string desc);`

All is pretty self explanatory.

*Note: Technically flag options can be anything that starts with '-' so option and longOption could be swithed up, or there could even be two options with '-', but they are originally meant to be used with a short and a long version. (Doing otherwise may cause problems in the future)*
## Examples
Examples can be found in the `./examples` folder. Take a look at them to get a deeper understanding of how things are done in action.

*Disclaimer: This projact was made and tested with the MinGW compiler. Some other compilers may work just as well, but by default the MSVC compiler will not work. To get it working with MSVC as well you have to add some extra imports at the start of CliLib.hpp. (Your IDE will probably tell you which)*
