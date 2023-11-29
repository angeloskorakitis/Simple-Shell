#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <vector>

#include "../include/utils.hpp"

std::set<pid_t> child_processes;

void sig_handler(int sig) {
    if (child_pid > 0){
      kill(child_pid, sig);
      suspended_pid = child_pid;
      std::cout << std::endl;
    }
    else std::cout << std::endl;
}

void cleanup_processes()
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        child_processes.erase(pid);
    }
}

AliasMap alias_map;
int last_command_exit_status;
std::vector<Command> history_vector;


bool wildcard_expansion(std::vector<char*> c_args,std::vector<char*> &glob_args)
{
  glob_t paths;
  bool wildcard = false;

  for (int j = 0; j < c_args.size() - 1; j++) {
      if (strchr(c_args[j], '*') != nullptr || strchr(c_args[j], '?') != nullptr) {
          wildcard = true;
          break;
      }
  }

  if(wildcard) 
  {
    // // For every character, if it is a wildcard, expand it.
    glob(c_args[1], GLOB_NOCHECK | GLOB_TILDE, NULL, &paths);

    for (int i = 2; i < c_args.size()-1; i++)
        glob(c_args[i], GLOB_NOCHECK | GLOB_TILDE | GLOB_APPEND, NULL, &paths);

    glob_args.push_back(c_args[0]);
    for (int i=0; i<paths.gl_pathc; i++)
        glob_args.push_back(paths.gl_pathv[i]);

    glob_args.push_back(nullptr);

    return true;
  }
  
  return false;
}


void execute_command(String input_str, TokenVector &tokens) 
{
  std::vector<bool> is_background;
  std::vector<std::vector<StringVector> > commands_groups;

    // I/O file descriptors
    int fdin = -1;
    int fdout = -1;

    // Pipe file descriptors
    int pipe_fds[2];
    int temp_pipe_fd = -1;

    lexer(input_str, tokens);

    // Parse the tokens into commands and return I/O file descriptors.
    parser(tokens, commands_groups, is_background, fdin, fdout);

    // For every command group...
    for (int j=0; j<commands_groups.size(); j++)
    {
      // Reset temp_pipe_fd for each command group
      temp_pipe_fd = -1;

      // Get the commands in the command group.
      std::vector<StringVector> &commands = commands_groups[j];

      // For commands in a command_group...
      for (int i = 0; i < commands.size(); i++) 
      {
        // Update history vector.
        Command command;
        command.command_str = input_str;
        command.command_vector = commands[i];

        history_vector.push_back(command);
        
        if(history_vector.size() > 20)
          history_vector.erase(history_vector.begin());
        
        // Check if the command is an alias.
        checkalias(commands, i);

        // Check some builtins - those that don't run in child process.
        if (commands[i][0] == "history")
        {
          if (commands[i].size() == 1)
          {
            history_builtin("", commands[i]);
            continue;
          }
          else 
          {
            history_builtin(input_str, commands[i]);
          }
        }
        
        if (commands[i][0] == "exit") {
          exit(EXIT_SUCCESS);
        }
        else if (commands[i][0] == "cd")
        {
          cd_builtin(commands, i);
          continue;
        }
        else if(commands[i][0] == "createalias")
        {
          createalias(commands[i][1], commands[i]);
          continue;
        }
        else if(commands[i][0] == "destroyalias")
        {
          destroyalias(commands[i][1]);
          continue;
        }
        

        // If we are not on the last command, create a pipe. It may means that there is only one command (no pipe).
        if (i != commands.size() - 1)
        {
          if(pipe(pipe_fds) < 0) 
          {
            std::cerr << "Error: pipe() failed." << std::endl;
            exit(EXIT_FAILURE);
          }
        }
          
        pid_t pid = fork();
        child_pid = pid;

        // Child process
        if (pid == 0) 
        {
            // Builtin commands
            // Check if the command is a builtin...
            if (commands[i][0] == "echo") {
              echo_builtin(commands[i][1]);
              exit(EXIT_SUCCESS);
            }

            // Pipes

            // If not the first command, redirect stdin to pipe. Else, redirect to file.
            if (fdin != -1 && i == 0) 
            {
                dup2(fdin, STDIN_FILENO);
                close(fdin);
            } 
            else if (temp_pipe_fd != -1) 
            {
                dup2(temp_pipe_fd, STDIN_FILENO);
                close(temp_pipe_fd);
            }

            // If not the last command, redirect stdout to pipe. Else, redirect to file.
            if (i != commands.size() - 1) {
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            } else if (fdout != -1) {
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
            }

            if (temp_pipe_fd != -1) 
              close(temp_pipe_fd);


            // Convert StringVector to char*.
            std::vector<char*> c_args;
            for (int z=0; z < commands[i].size(); z++)
                c_args.push_back(const_cast<char*>(commands[i][z].c_str()));

            // It needs to end with the nullptr
            c_args.push_back(nullptr);

            // Check for wildcard using glob().
            std::vector<char*> glob_args;

            // If wildcard is found, expand it and execute the command. Else, execute the command as is.
            if(wildcard_expansion(c_args, glob_args))
              execvp(glob_args[0], glob_args.data());
            else
              execvp(c_args[0], c_args.data());
            
            std::cerr << "mysh: command not found: "<< input_str << std::endl;
            exit(EXIT_FAILURE);
          }
          // Parent process
          else if (pid > 0) 
          {
          
          child_processes.insert(pid);
          
          if (!is_background[i]) 
          {
              int status;

              // Wait for the child process.
              do waitpid(pid, &status, WUNTRACED);
              while (!WIFSTOPPED(status) && !WIFEXITED(status)&& !WIFSIGNALED(status));

              if (WIFEXITED(status))  last_command_exit_status = WEXITSTATUS(status);
              else last_command_exit_status = 1; // Exit error

              // If the buffer is not empty, flush it.
              std::flush(std::cout);

              // Remove child process from the set after it finishes
              child_processes.erase(pid);
          }
          // Fork error
          } 
          else
          {
              std::cerr << "mysh: fork() failed!" << std::endl;
              exit(EXIT_FAILURE);
          }

          // If not the last command, close the pipe and set the input file descriptor to the pipe.
          if (i!= commands.size()-1) 
          {
              if (temp_pipe_fd !=-1)
                close(temp_pipe_fd);
    
              // Set the input file descriptor to the pipe.
              temp_pipe_fd = pipe_fds[0];
              close(pipe_fds[1]);
          }

      }

      // Close the input and output file descriptors.
      if (fdin != -1) close(fdin);
      if (fdout != -1) close(fdout);

  }

}