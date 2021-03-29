# CliLib
**Latest commit contains some unfinished stuff. Revert to the commit before positional options to be able to use the library.**

A library aiming to be an easy to use and simple argument parser for CLI applications.

This project was heavily inspired by a repository called [MiniCommander](https://github.com/MichaelGrupp/MiniCommander) and it uses the same parsing algorithm (for now at least). Looking back it might have been appropriate to fork that repository but anyways, check it out if you are interested in it.
# Features
 - [x] Default option and unlimited suboptions with descriptions
 - [x] Command aliases
 - [x] Option to ignore unrecognised commands or throw an error
 - [x] Single and multiple parameters of any type
 - [x] Split short flags (unix flags) option
 - [x] Parameter groups with a name, a policy and a description
 - [x] Optional, required, anyof and oneof policy for parameter groups
 - [x] Dynamic help command that uses the description and name of options and option groups to generate a help message
 - [x] Only C++11 required
# TODO
 - [ ] Add positional options
 - [ ] Add option validators (Ex.: Option can only be set type or it can only be an odd number...)
 - [ ] Maybe add windows type flag support (?)
 - [ ] Maybe improve option detection (?)
 - [ ] Custom help command + add policy to help
 - [ ] Add the option variable to the option itself instead of having to get it's value separately.
# Usage
Start your main function with parsing the program arguments. For this use the `Parser::parse(const int& argc, char const*const* argv, bool noRemainder = true, bool splitFlags = false)` method.

Optional parameters:
 - `bool splitFlags = false`: Use this to decide wither to split multi character short flags or not. For example `-abc` would be handled as `-a -b -c` if enabled.

## Getting option values
There are two ways to get option values: get a single value or a multi value.

To get a single value use the `Parser::getConverted<T>(const std::string& option, const std::string& longOption = "", T defaultValue = T())` method.

Optional parameters:
 - `const std::string& longOption = ""`: Use it to specify the long version (the one starting with --) of the option if there is one.
 - `T defaultValue = T()`: Use it to specify a default value to be used when the option is not set. If the the group policy of the option is required then there is no need for this.

To get a multi value use the `Parser::getMultiConverted<T>(const std::string& option, const std::string& longOption = "", std::initializer_list<T> defaultInit = {})` method. This will return a vector of type T.

Optional parameters:
 - `const std::string& longOption = ""`: Use it to specify the long version (the one starting with --) of the option if there is one.
 - `std::initializer_list<T> defaultInit = {}`: Use it to specify a default initializer_list to be used when the option is not set. If the the group policy of the option is required then there is no need for this.

*Note: If noReaminder is false and we are trying to get the value from an option that deos not belong to the command but is set, it will still return it's value. I do not consider this a bug as all that has to be done for it to not happen is only using getConverted and getMultiConverted on options that belong to the command.*
## Making commands and option groups
### Commands
To make a command make an instance of type `Command`. Each command requires a description, a corresponding function, and all arguments of the function (mainly variables previously assigned a value using either `Parser::getConverted()` or `Parser::getMultiConverted()`). To set a command as the default command (the one that gets called when no command is provided) use the `Parser::setAsDefault(Command* newDefaultCommand)` method. Do not use a dynamically allocated pointer. Every program must have a default command.

To add a subcommand use the `.addSubCommand(Command* newSubCommand, Names... names)` method. newSubCommand must be the address of a stack allocated command and names have to be strings (ar a type thet can initialize a string).

To add an option group use the `.addOptionGroup(OptionGroup* newOptionGroup)` or `.addOptionGroup(Groups... groups)` Only add an option group, that is already set up with all of it's options. Do not use a dynamically allocated pointer.

Each command uses a "noRemainder" policy by default, meaning that unrecognized arguments will throw an error. To turn this off use the `.setNoRemainder(bool newNoRemainder)` method with `false` as an argument. This change will not apply to subcommands. 

### Option groups

To make an option group make an instance of type `OptionGroup`. Each option group requires a policy and a description. The policy can be any value of `enum class Policy`. Dynamically allocated option pointers can also be provided at the end of the constructor call.

To add an option use the `.addOption(const std::string& option, const std::string& desc, const std::string& longOption="")` or the `.addOption(Opts... opts)` method. (In the second case dynamically allocated option pointers have to be provided)

Optional parameters:
 - `const std::string& longOption=""`: Use this to set the long version of the flag (the one the starts with --).
   
*Note: Technically the options can be anything that starts with '-' so option and longOption could be swithed up, or there could even be two options with '-', but they are originally meant to be used with a short and a long version. (Doing otherwise may cause problems in the future)*
#### Options
The constructor of the `FlagOption` class requires the option name (short version), the description and optioanally the long name. (See: first overload of the `.addOption` method in option groups, the parameters are the same)
## Starting the program
Starting the program can be done with `Parser::run()`. This will decide wether the program arguments are valid and wich command is to be used and it will run it too.
