#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex.h>

int main(void) {
    struct dirent *de;  // Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir("/proc");

    if (dr == NULL) { // opendir returns NULL if couldn't open directory
        printf("Could not open current directory" );
        return 0;
    } 

    // compile regular expression
    regex_t regex;
    int reti = regcomp(&regex, "^[0-9][0-9]*$", 0);
    if(reti){
        printf("Could not compile regex.\n");
        exit(1);
    }

    while ((de = readdir(dr)) != NULL) {
        reti = regexec(&regex, de->d_name, 0, NULL, 0);
        if (!reti){
            printf("%s\n", de->d_name);
        }
    }

    regfree(&regex);
    closedir(dr);
    return 0;
}
