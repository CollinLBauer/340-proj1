#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>

struct process {
    int pid;                    // process ID
    int ppid;                   // parent process ID
    char comm[255];             // ???
    unsigned long int vsize;    // ???

};

// initialize the process structure
struct process *initProc(char fileName[]) {
    struct process *proc = malloc(sizeof(struct process));
    FILE *inFile = fopen(fileName, "r");
    int dummy;

    // test for files not existing.
    if (inFile == NULL) {
        printf("Error! Could not open file\n");
        exit(-1);
    }

    fscanf(inFile, "%d", &(proc->pid));
    fscanf(inFile, "%s", (proc->comm));
    fscanf(inFile, "%ls", &dummy);
    fscanf(inFile, "%d", &(proc->ppid));
    for (int i = 0; i < 18; i++) 
        fscanf(inFile, "%d", &dummy);
    fscanf(inFile, "%ld", &(proc->vsize));

    fclose(inFile);

    return proc;
 
};

// build a process structure and print out its status
int getProcStatus(char *pid) { 
    // declare process file path
    char name[255];
    strcpy(name, "/proc/");
    strcat(name, pid);
    strcat(name, "/stat");

    // build process structure
    struct process* testProc = initProc(name);

    // print process status
    printf("pid <%d>, ppid <%d>, vsize <%ld>, comm <%s>\n", testProc->pid, testProc->ppid, testProc->vsize, testProc->comm);

    free(testProc);
    return 0;
};

// list the files inside the root folder of a process
int listProcData(char *name) {
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
};


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
            getProcStatus(de->d_name);
        }
    }
    printf("Cleared while\n");

    // free memory, close files and end program
    regfree(&regex);
    closedir(dr);
    printf("Done.\n");
    return 0;
}
