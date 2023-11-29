#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <map>

typedef std::string String;
typedef std::vector<String> StringVector;
typedef std::map<String, StringVector> AliasMap;

void echo_builtin(String args_str);

void history_builtin(String str, StringVector &commands);

void destroyalias(String alias);

void createalias(String alias, StringVector command);

void checkalias(std::vector<StringVector> &commands, int i);

void cd_builtin(std::vector<StringVector> &commands, int i);
