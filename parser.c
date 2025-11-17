#include "parser.h"
#include "string.h"

// Simple table stored in the format of ID and the name of command (using pointer )
char* commandList[NUM_COMMANDS][2] = {

    {(char*) CLEAR_SCREEN,  "clear"     },
    {(char*) LIST_FILES,    "ls"        },
    {(char*) CREATE_FILE,   "create"    },
    {(char*) DELETE_FILE,   "rm"        },
    {(char*) PRINT_STR,     "print"     },
    {(char*) VERSION,       "info"      },
    {(char*) HELP,          "help"      }

};

// Choose the command that is meant to be given
enum cmd ChooseCommand(char* command) {

    // Loop over every possible command
    unsigned char i, found;
    char* candidate;
    char* pos;

    // Look through list of commands
    for (i = 0; i < NUM_COMMANDS; i++) {

        // Compare the first word of the command to the command in the table
        found = 1;
        candidate = commandList[i][1];
        pos = command;
        while (*pos != 0 && *pos != ' ') {
            if (*candidate != *pos) {
                found = 0;
                break;
            }
            candidate++;
            pos++;
            if ((*pos == 0 || *pos == ' ') && *candidate != 0) {
                found = 0;
                break;
            }      
        }

        // If there's a matching command, return the corresponding command ID
        if (found) return (enum cmd)((unsigned char) commandList[i][0]);
        else continue;
    }

    // If it goes through table with no matches, display unknown command
    return UNKNOWN;
}
