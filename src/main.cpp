#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <vector>

#include "../include/utils.hpp"

pid_t child_pid = -1;
pid_t suspended_pid = -1;

int main() 
{

    // Set up signal handlers.
    struct sigaction sigint_action;
    struct sigaction sigtstp_action;

    sigint_action.sa_handler = sig_handler;
    sigemptyset(&sigint_action.sa_mask);
    sigint_action.sa_flags = 0;

    sigtstp_action.sa_handler = sig_handler;
    sigemptyset(&sigtstp_action.sa_mask);
    sigtstp_action.sa_flags = 0;

    sigaction(SIGINT, &sigint_action, nullptr);
    sigaction(SIGTSTP, &sigtstp_action, nullptr);
    
    while(true)
    {
        // Cleanup processes that have not been terminated.
        cleanup_processes();

        child_pid= -1;
        suspended_pid = -1;

        String input;
        std::cout << "in-mysh-now:> ";
        std::getline(std::cin, input);

        if (std::cin.eof() || std::cin.fail()) {
            std::cin.clear();
        }

        TokenVector tokens;
        
        execute_command(input, tokens);

    }
}