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
Start your main function with parsing the program arguments. For this use the `Parser::parse(argc, argv)` method.

Additional options for parsing:
 - `bool noReaminder = true`: Comes after argv, use this to decide wether to throw an error on unrecignized commands or not. (Possibly getting moved to commands in the near future)
 - `bool splitFlags = false`: Comes after noRemainder, use this to decide wither to split multi character short flags or not. For example `-abc` would be handled as `-a -b -c` if enabled.

## Getting option values
For getting option values there are two options: get a single value or a multi value.

To get a single value use the `Parser::getConverted<T>("-option")` method.

Additional options:
 - `const std::string& longOption = ""`: Comes after the option, use it to specify the long version (the one starting with --) of the option if there is one.
 - `T defaultValue = T()`: Comes after longOption, use it to specify a default value to be used when the option is not set. If the the group policy of the option is required then there is no need for this.

To get a multi value use the `Parser::getMultiConverted<T>("-option")` method. This will return a vector of type T.

Additional options:
- `const std::string& longOption = ""`: Comes after the option, use it to specify the long version (the one starting with --) of the option if there is one.
- `std::initializer_list<T> defaultInit = {}`: Comes after longOption, use it to specify a default initializer_list to be used when the option is not set. If the the group policy of the option is required then there is no need for this.