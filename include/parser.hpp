#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <vector>

enum TokenType{
    REDIRECTION_IN,
    REDIRECTION_OUT,
    REDIRECTION_OUT_APPEND,
    PIPE,
    BACKGROUND,
    END,
    SEMICOLON,
    QUOTATION_MARK,
    STRING
};

typedef std::string String;
typedef std::vector<String> StringVector;

struct Token{
    TokenType type;
    String value;
};

typedef std::vector<Token> TokenVector;

void lexer(String input_string, TokenVector &tokens);

// Parses the tokens from the lexer, creates the commands groups (;), determines the fdin and fdout.
void parser(TokenVector &tokens, std::vector<std::vector<StringVector> > &command_groups, std::vector<bool> &is_background, int &fdin, int &fdout);
