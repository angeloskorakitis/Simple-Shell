#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <glob.h>
#include <map>
#include <signal.h>
#include <cstring>
#include <set>


#include "../include/parser.hpp"
#include "../include/builtins.hpp"

typedef std::string String;
typedef std::vector<String> StringVector;
typedef std::vector<StringVector>::iterator StringVectorIterator;

// A map to store the aliases.
extern AliasMap alias_map;

extern pid_t child_pid;
extern pid_t suspended_pid;

// Keep the exit status of the last command for the echo $? builtin.
extern int last_command_exit_status;

// A struct to store the commands for the history function.
struct Command{
    String command_str;
    StringVector command_vector;
};

// A function to handle the SIGINT and SIGTSTP signals.
void sig_handler(int sig);

// A vector to store the commands for the history function.
extern std::vector<Command> history_vector;

// A function to clean up the child processes.
void cleanup_processes();

// A function to expand the wildcard characters in a command.
bool wildcard_expansion(std::vector<char*> c_args,std::vector<char*> &glob_args);

// A function to execute a command as it is given in the promt.
void execute_command(String input_str,TokenVector &tokens);

