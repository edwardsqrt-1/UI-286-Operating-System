#ifndef PARSER_H
#define PARSER_H
#define NUM_COMMANDS 7 // Number of commands

// Possible commands to be given (does not count applications)
enum cmd {
    UNKNOWN = -1, // Unknown command
    CLEAR_SCREEN, // clear
    LIST_FILES, // ls
    CREATE_FILE, // new
    DELETE_FILE, // rm
    PRINT_STR, // print
    VERSION, // info
    HELP // help
};

// Return what command is being invoked
enum cmd ChooseCommand(char* command);

#endif
