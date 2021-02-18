# CliLib
A library aiming to be an easy to use and simple argument parser for CLI applications.

Some the code is from a repository called [MiniCommander](https://github.com/MichaelGrupp/MiniCommander). Looking back it might have been appropriate to fork that repository but anyways, check it out if you are interested in it.
# Features
 - [x] Default option and unlimited suboptions with descriptions
 - [x] Option to ignore unrecognised commands or throw an error
 - [x] Single and multiple parameters of any type
 - [x] Split short flags (unix flags) option
 - [x] Parameter groups with a name, a policy and a description
 - [x] Optional, required, anyof and oneof policy for parameter groups
 - [x] Dynamic help command that uses the description and name of options and option groups to generate a help message
 - [x] Only C++11 required
# TODO
 - [ ] Make examples
 - [ ] Add windows type flag support
 - [ ] Add option validators
 - [ ] Add alternative ways to create and use option groups
 - [ ] Maybe improve option detection (?)
# Usage
Start your main function with parsing the program arguments. For this use the `Parser::parse(const int& argc, char const*const* argv, bool noRemainder = true, bool splitFlags = false)` method.

Optional parameters:
 - `bool noReaminder = true`: Use this to decide wether to throw an error on unrecignized commands or not. (Possibly getting moved to commands in the near future)
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
To make a command make an instance of type `Command`. Each command requires a description, a corresponding function, and all arguments of the function (mainly variables previously assigned a value using either `Parser::getConverted()` or `Parser::getMultiConverted()`). To set a command as the default command (the one that gets called when no command is provided) use the `.setAsDefault()` method. Every program must have a default command.

To add a subcommand use the `.addSubCommand(const std::string& name, Command* newSubCommand)` method. newSubCommand must be the address of a stack allocated command.

To add an option group use the `.addOptionGroup(const OptionGroup& newOptionGroup)` Only add an option group, that is already set up with all of it's options.

### Option groups

To make an option group make an instance of type `OptionGroup`. Each option group requires a policy and a description. The policy can be any value of `enum class Policy`.

To add an option use the `.addOption(const std::string& option, const std::string& desc, const std::string& longOption="")` method.

Optional parameters:
 - `const std::string& longOption=""`: Use this to set the long version of the flag (the one the starts with --).
   
*Note: Technically the options can be anything that starts with '-' so option and longOption could be swithed up, or there could even be two options with '-', but they are originally meant to be used with a short and a long version. (Doing otherwisew may cause problems in the future)*
## Starting the program
Starting the program can be done with `Parser::run()`. This will decide wether the program arguments are valid and wich command is to be used and it will run it too.