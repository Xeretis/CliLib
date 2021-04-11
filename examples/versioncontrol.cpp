#include <CliLib.hpp>
#include <iostream>

void commitFunc(const std::string& messsage) {
    std::cout << "Commited " << "with message: " << messsage << "\n";
}

void stageFunc(const std::vector<std::string>& toStage) {
    std::cout << "Staged: ";
    for (const auto& element : toStage)
        std::cout << element << " ";
    std::cout << "\n";
}

void pushFunc(const std::string& branch) {
    std::cout << "Pushed to " << branch << "\n";
}

void removeFunc(bool cached, const std::string& file) {
    if (cached)
        std::cout << "Removed " << file << " from remote repository\n";
    else
        std::cout << "Removed " << file << " from local and remote repository\n";
}

void removeCommitFunc(int number) {
    std::cout << "Removed commit number " << number << "\n";
}

int main(int argc, char** argv) {
    Parser::parse(argc, argv);

    Command defaultCommand("The default command", [&](){defaultCommand.printHelp("Usage");});

    //args
    std::string message = Parser::getConverted<std::string>("-m", "--message");
    //command
    Command commit ("Allows you to commit the staged changes", commitFunc, message);
    //options
    OptionGroup commitReq("Required options");
    commitReq.addOption(new FlagOption("-m", "The commit message itself", "--message"));

    commit.addOptionGroup(&commitReq);

    //args
    std::vector<std::string> toStage = Parser::getMultiConverted<std::string>(0, 1);
    //command
    Command stage("Allows you to stage changes for commiting", stageFunc, toStage);
    //options
    OptionGroup stageReq("Required options");
    stageReq.addOption( new PositionalOption(0, "The items to stage"));

    stage.addOptionGroup(&stageReq);

    //args
    std::string branch = Parser::getConverted<std::string>(0, 1);
    //command
    Command push("Allows you to push the commited stages", pushFunc, branch);
    //options
    OptionGroup pushReq("Required options");
    pushReq.addOption(new PositionalOption(0, "The branch to push to"));

    push.addOptionGroup(&pushReq);

    //args
    bool cached = Parser::getConverted<bool>("-c", "--cached", false);
    std::string file = Parser::getConverted<std::string>("-f", "--file");
    //command
    Command remove("Allows you to remove files", removeFunc, cached, file);
    //options
    OptionGroup removeOpt("Optional options", FlagPolicy::OPTIONAL);
    removeOpt.addOption(new FlagOption("-c", "If set it will only remove the file from the remote repository", "--cahced"));

    OptionGroup removeReq("Required options");
    removeReq.addOption(new FlagOption("-f", "The file to remove", "--file"));

    remove.addOptionGroup(&removeOpt);
    remove.addOptionGroup(&removeReq);

    //args
    int number = Parser::getConverted<int>(0, 2);
    //command
    Command removeCommit("Allows you to remove a commit (will revert changes)", removeCommitFunc, number);
    //options
    OptionGroup removeCommitReq("Required options");
    removeCommitReq.addOption(new PositionalOption(0, "The number of the commit to remove"));

    removeCommit.addOptionGroup(&removeCommitReq);

    //command structure
    defaultCommand.addSubCommand(&commit, "commit");
    defaultCommand.addSubCommand(&stage, "stage");
    defaultCommand.addSubCommand(&push, "push");
    defaultCommand.addSubCommand(&remove, "remove", "rm");

    remove.addSubCommand(&removeCommit, "commit");

    defaultCommand.run();
}