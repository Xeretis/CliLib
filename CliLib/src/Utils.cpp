//
// Created by user on 2/2/2021.
//

#include "../include/Utils.h"

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string Utils::getWorkingDir() { //could use filesystem but it's completely broken with my compiler so I suppose it is with others as well
    char buff[FILENAME_MAX]; //create string buffer to hold path
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    return current_working_dir;
}
