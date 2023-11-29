#include "../include/parser.hpp"

void lexer(String input_string, TokenVector &tokens)
{
    
    // Keep track of the current word-string
    String current_string;
    
    //  Iterate over the input string and create tokens
    for(int i = 0; i < input_string.size(); i++){

        char c = input_string[i];
        Token token;

        if (c == ' ' ){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
        }
        else if (c == '<'){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
            token.type = REDIRECTION_IN;
            token.value = "<";
            tokens.push_back(token);
        }
        else if (c == '>'){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
            if (i+1 < input_string.size() && input_string[i+1] == '>'){
                token.type = REDIRECTION_OUT_APPEND;
                token.value = ">>";
                tokens.push_back(token);
                i++;
            }
            else{
                token.type = REDIRECTION_OUT;
                token.value = ">";
                tokens.push_back(token);
            }
        }
        else if (c == '|'){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
            token.type = PIPE;
            token.value = "|";
            tokens.push_back(token);
        }
        else if (c == '&'){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
            token.type = BACKGROUND;
            token.value = "&";
            tokens.push_back(token);
        }
        else if (c == ';'){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
            token.type = SEMICOLON;
            token.value = ";";
            tokens.push_back(token);
        }
        else if (c == '"'){
            if(!current_string.empty()){
                Token token;
                token.type = STRING;
                token.value = current_string;
                tokens.push_back(token);
                current_string.clear();
            }
            token.type = QUOTATION_MARK;
            token.value = '"';
            tokens.push_back(token);
        }
        else{
            current_string.push_back(c);
        }

    }
    // If there is any string left in the buffer, add it to the tokens.
    if(!current_string.empty()){
        Token token;
        token.type = STRING;
        token.value = current_string;
        tokens.push_back(token);
        current_string.clear();
    }

}

void parser(TokenVector &tokens, std::vector<std::vector<StringVector> > &command_groups, std::vector<bool> &is_background, int &fdin, int &fdout)
{
    StringVector command;
    std::vector<StringVector> commands;
    bool is_command_background = false;
    int quotation_mark_count = 0;

    for (int i = 0; i < tokens.size(); i++) {
        
        switch (tokens[i].type) {
            case STRING:
                command.push_back(tokens[i].value);
                break;
            case REDIRECTION_IN:
                fdin = open(tokens[++i].value.c_str(), O_RDONLY);
                if (fdin < 0) {
                    std::cerr << "open() could not open file." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case REDIRECTION_OUT:
                fdout = open(tokens[++i].value.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fdout < 0) {
                    std::cerr << "open() could not open file." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case REDIRECTION_OUT_APPEND:
                fdout = open(tokens[++i].value.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
                if (fdout < 0) {
                    std::cerr << "open() could not open file." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case PIPE:
                commands.push_back(command);
                command.clear();
                break;
            case SEMICOLON:
                if(!command.empty())
                {
                    commands.push_back(command);
                    is_background.push_back(is_command_background);
                    command.clear();
                    is_command_background = false;
                }
                command_groups.push_back(commands);
                commands.clear();
                break;
            case BACKGROUND:
                is_command_background = true;
                break;
            case QUOTATION_MARK:
                quotation_mark_count++;

                if (quotation_mark_count % 2 == 0) {
                    commands.push_back(command);
                    is_background.push_back(is_command_background);
                    command.clear();
                }
                break;
            default:
                break;
        }
    }

    if(!command.empty())
    {
        commands.push_back(command);
        is_background.push_back(is_command_background);
    }
    command_groups.push_back(commands);

}
