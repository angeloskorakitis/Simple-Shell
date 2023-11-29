#include "../include/builtins.hpp"
#include "../include/utils.hpp"

void echo_builtin(String args_str)
{
    if(args_str[0] == '$')
    {
        if(args_str[1] == '?') std::cout << last_command_exit_status << std::endl;
        else
        {
            std::string var = args_str.substr(1, args_str.size()-1);;
            std::cout << getenv(var.c_str()) << std::endl;
        }
    }
    else
    {
        std::cout << args_str << std::endl;
    }
}

void history_builtin(String str, StringVector &commands)
{
    if(str == ""){
        for (int i = 0; i < history_vector.size(); i++)
            std::cout << i+1 << " " << history_vector[i].command_str << std::endl;
        last_command_exit_status = 0;
    }
    else
    {
        int num = std::stoi(commands[1]);
        
        if(num > 20 || num > history_vector.size()){
            std::cerr << "Invalid history number." << std::endl;
            last_command_exit_status = 1;
        }
        else{ 
        commands = history_vector[num-1].command_vector;
        last_command_exit_status = 0;
        }

    }
}

void createalias(String alias, StringVector command)
{
    // Erase "createalias" and "alias" from the command.
    command.erase(command.begin());
    command.erase(command.begin());

    // Add the alias to the map.
    alias_map[alias] = command;

    last_command_exit_status = 0;
}

void destroyalias(String alias)
{
    alias_map.erase(alias);
    
    last_command_exit_status = 0;
}

void checkalias(std::vector<StringVector> &commands, int i)
{
    // Check if the command is an alias.
    for(AliasMap::iterator it=alias_map.begin(); it!=alias_map.end(); it++)
    {
        // if yes just replace the specific command with the alias
        if(commands[i][0] == it->first)
            commands[i].swap(it->second);
    }
    last_command_exit_status = 0;
}

void cd_builtin(std::vector<StringVector> &commands, int i)
{
    // If there is a seccod argument, use it as target directory.
    if (commands[i].size() >= 2)
    {
        if (chdir(commands[i][1].c_str()) != 0) {
            std::cerr << "cd: " << commands[i][1] << ": No such file or directory" << std::endl;
          last_command_exit_status = 1;
        }
    } 
    else 
    {
        // If there are no arguments, change to the home directory.
        const char *home_dir = getenv("HOME");
        if (home_dir != nullptr){
            if (chdir(home_dir) != 0)
            {
                last_command_exit_status = 1;
                std::cerr << "cd: " << home_dir << ": No such file or directory" << std::endl;
            }
        }
        else 
        {
            std::cerr << "Error: {HOME} environment variable not set" << std::endl;
            last_command_exit_status = 1;
        }
    }
}