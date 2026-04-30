// todo: if -- or - its a command, otherwise its a parameter input - sort this out and combine into one structure to use instead of the entire terminal input atm
#ifndef zoxm_terminals
#define zoxm_terminals

#ifdef zox_headless
    byte headless = 1;
#else
    byte headless = 0;
#endif

#include "dat/terminal_inputs.c"
#include "fun/_.c"

#endif
