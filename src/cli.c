#include "cli.h"
#include <stdio.h>
#include <string.h>

int
match_arg(const char* arg) {
    if (arg[0] != '-')                                     return INPUT;
    if (!strcmp(arg, "-q")  || !strcmp(arg, "--question")) return QUESTION;
    if (!strcmp(arg, "-c")  || !strcmp(arg, "--code"))     return CODE;
    if (!strcmp(arg, "-h")  || !strcmp(arg, "--help"))     return HELP;
    if (!strcmp(arg, "-p")  || !strcmp(arg, "--price"))    return PRICE;
    if (!strcmp(arg, "-t")  || !strcmp(arg, "--token"))    return TOKEN;
    if (!strcmp(arg, "-cp") || !strcmp(arg, "--copy"))     return COPY;
    if (!strcmp(arg, "-m")  || !strcmp(arg, "--model"))    return MODEL;
    return ERROR;
}

void
print_help(void) {
    puts(
"Usage: ./commando [FLAGS] \"your input\"\n"
"\n"
"FLAGS:\n"
"  -q, --question   Treat the input as a question. The AI gives a short,\n"
"                   direct answer without anything complex.\n"
"  -c, --code       Treat the input as a coding request. The AI responds\n"
"                   with code.\n"
"  -m, --model      Choose the model to use. Takes the model name as the\n"
"                   next argument. Supported models: astra, sol, luna.\n"
"  -p, --price      Show the cost of the last conversation.\n"
"  -t, --token      Show token usage / status for the API key. Ignores\n"
"                   all other options except --help.\n"
"  -cp, --copy      Copy the AI's output to the clipboard.\n"
"                   (not implemented yet)\n"
"  -h, --help       Show this message.\n"
"\n"
"Examples:\n"
"  ./commando -q \"what is a pointer?\"\n"
"  ./commando -c \"function that reverses a string\"\n"
"  ./commando -m sol -q \"what is a pointer?\"\n"
"  ./commando --token"
    );
}
