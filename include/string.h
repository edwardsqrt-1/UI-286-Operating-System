#ifndef STRING_H
#define STRING_H 

// Function to return a character value from the keyboard
char GetChar();

// Function to halt until a character is read from the keyboard
char GetChar_H();

// Function to get the length of a string
unsigned short StrLen(char* str);

// Function to compare strings
// Note: 0 = different and 1 = same
unsigned char StrCompare(char* str1, char* str2);

#endif
