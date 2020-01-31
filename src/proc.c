#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>

struct process {
    int pid;                    // process ID
    int ppid;                   // parent process ID
    char comm[255];             // name of executable
    unsigned long int vsize;    // size of virtual address space
    struct process *next;       // next node in linked list

};

struct linkedList {
    struct process *head;
};



// initialize the process structure
struct process *initProc(char fileName[]) {
    struct process *proc = malloc(sizeof(struct process));
    FILE *inFile = fopen(fileName, "r");
    char dummy[255];

    // test for files not existing.
    if (inFile == NULL) {
        printf("Error! Could not open file\n");
        exit(-1);
    }

    fscanf(inFile, "%d", &(proc->pid));
    fscanf(inFile, "%s", (proc->comm));
    fscanf(inFile, "%s", dummy);
    fscanf(inFile, "%d", &(proc->ppid));
    for (int i = 0; i < 18; i++) 
        fscanf(inFile, "%s", dummy);
    fscanf(inFile, "%ld", &(proc->vsize));

    fclose(inFile);

    return proc;
 
};

// build a process structure and print out its status
int getProcStatus(struct process *proc) { 

    // print process status
    printf("pid <%d>, ppid <%d>, vsize <%ld>, comm <%s>\n", proc->pid, proc->ppid, proc->vsize, proc->comm);

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

    struct process *head;
    struct process *curr;
    // compare regex to each file in directory
    while ((de = readdir(dr)) != NULL) {
        reti = regexec(&regex, de->d_name, 0, NULL, 0);
        if (!reti){ // if file is a process

            struct process *node = initProc(de->d_name);
            if (node->pid == 1) {
                head = node;
                curr = head;
            }

            else {
                while (curr->next != NULL) {
                    curr = curr->next;
                }
            }
            curr->next = node;
        }
    }

    curr = head;
    for (int i = 0; i <100; i++) {
        getProcStatus(curr);
        curr = curr->next;
    }


    // free memory, close files and end program
    regfree(&regex);
    closedir(dr);
    printf("Done.\n");
    return 0;
}