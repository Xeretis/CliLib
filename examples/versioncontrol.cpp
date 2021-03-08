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
    defaultCommand.setAsDefault();

    //args
    std::string message = Parser::getConverted<std::string>("-m", "--message");
    //command
    Command commit ("Allows you to commit the staged changes", commitFunc, message);
    //options
    OptionGroup commitReq(Policy::REQUIRED, "Required options", new Option("-m", "The commit message itself", "--message"));
    commit.addOptionGroup(&commitReq);

    //args
    std::vector<std::string> toStage = Parser::getMultiConverted<std::string>("-i", "--items");
    //command
    Command stage("Allows you to stage changes for commiting", stageFunc, toStage);
    //options
    OptionGroup stageReq(Policy::REQUIRED, "Required options", new Option("-i", "The items to stage", "--items"));
    stage.addOptionGroup(&stageReq);

    //args
    std::string branch = Parser::getConverted<std::string>("-b", "--branch");
    //command
    Command push("Allows you to push the commited stages", pushFunc, branch);
    //options
    OptionGroup pushReq(Policy::REQUIRED, "Required options", new Option("-b", "The branch to push to", "--branch"));
    push.addOptionGroup(&pushReq);

    //args
    bool cached = Parser::getConverted<bool>("-c", "--cached", false);
    std::string file = Parser::getConverted<std::string>("-f", "--file");
    //command
    Command remove("Allows you to remove files", removeFunc, cached, file);
    //options
    OptionGroup removeOpt(Policy::OPTIONAL, "Optional options", new Option("-c", "If set it will only remove the file from the remote repository", "--cahced"));
    OptionGroup removeReq(Policy::REQUIRED, "Required options", new Option("-f", "The file to remove", "--file"));
    remove.addOptionGroup(&removeOpt);
    remove.addOptionGroup(&removeReq);

    //args
    int number = Parser::getConverted<int>("-n", "--number");
    //command
    Command removeCommit("Allows you to remove a commit (will revert changes)", removeCommitFunc, number);
    //options
    OptionGroup removeCommitReq(Policy::REQUIRED, "Required options", new Option("-n", "The number of the commit to remove", "--number"));
    removeCommit.addOptionGroup(&removeCommitReq);

    //command structure
    defaultCommand.addSubCommand(&commit, "commit");
    defaultCommand.addSubCommand(&stage, "stage");
    defaultCommand.addSubCommand(&push, "push");
    defaultCommand.addSubCommand(&remove, "remove", "rm");

    remove.addSubCommand(&removeCommit, "commit");

    Parser::run();
}