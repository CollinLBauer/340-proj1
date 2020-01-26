#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>

int listProcData(char *name){
    struct dirent *de;

    // create path to process
    char dirname[255];
    strcpy(dirname, "/proc/");
    strcat(dirname, name);

    // open process directory
    DIR *procDir = opendir(dirname);
    if (procDir == NULL) { // opendir returns NULL if couldn't open directory
        printf("Could not open current process %s.\n", name);
        return 0;
    }

    // list process contents
    printf("Process %s contents:\n", name);
    while ((de = readdir(procDir)) != NULL) {
        printf("  %s\n", de->d_name);
    }

    closedir(procDir);
    return 0;

}

int main(void) {
    struct dirent *de;  // Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir("/proc");

    if (dr == NULL) { // opendir returns NULL if couldn't open directory
        printf("Could not open current directory.\n");
        return 0;
    }

    // compile regular expression
    regex_t regex;
    int reti = regcomp(&regex, "^[0-9][0-9]*$", 0);
    if(reti){
        printf("Could not compile regex.\n");
        exit(1);
    }

    // compare regex to each file in directory
    while ((de = readdir(dr)) != NULL) {
        reti = regexec(&regex, de->d_name, 0, NULL, 0);
        if (!reti){ // if file is a process
            listProcData(de->d_name);
            printf("\n");
        }
    }

    // free memory, close files and end program
    regfree(&regex);
    closedir(dr);
    return 0;
}
