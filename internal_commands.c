//
// Created by ubuntie on 29/05/18.
//

#include <zconf.h>

void cd(char ** args);

char *internals_strings[] = {
        "cd",
};


void (*internal_func[]) (char **) = {
        &cd,
};



void cd(char ** args) {
    chdir(args[1]);
}

int number_of_internals(){
    return sizeof(internal_func) / sizeof(char*);
}
