#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>

// defines the process structure
struct process {
    int pid;                    // process ID
    int ppid;                   // parent process ID
    char comm[255];             // name of executable
    unsigned long int vsize;    // size of virtual address space
    struct process *next;       // next node in linked list

};

// initialize the process structure
struct process *initProc(char *pid) {
    // declare process file path
    char statPath[255];
    strcpy(statPath, "/proc/");
    strcat(statPath, pid);
    strcat(statPath, "/stat");
    FILE *inFile = fopen(statPath, "r");

    // check if file path exists
    if (inFile == NULL) {
        printf("Error! Could not open file\n");
        exit(-1);
    }
    
    struct process *proc = malloc(sizeof(struct process));
    char dummy[255];

    // scan the file for proc elements
    fscanf(inFile, "%d", &(proc->pid));     // process ID

    // comm is the process name
    // This rats nest of code guarantees that the entire name is grabbed,
    // regardless of spaces. It also removes leading and tailing parentheses.
    char temp1[255];
    char temp2[255];
    fscanf(inFile, "%s", (temp1));     
    while (temp1[strlen(temp1)-1] != ')') {
        temp2[0] = '\0';
        fscanf(inFile, "%s", (temp2));
        strcat(temp1, " ");
        strcat(temp1, temp2);
    }
    char finalComm[strlen(temp1)-2];
    for (int i = 0; i < strlen(temp1)-2; i++){
        finalComm[i] = temp1[i+1];
    }
    finalComm[sizeof(finalComm)/sizeof(char)] = '\0';
    strcpy(proc->comm, finalComm);

    fscanf(inFile, "%s", dummy);            // dummy - skips over single entry
    fscanf(inFile, "%d", &(proc->ppid));    // Parent process ID
    for (int i = 0; i < 18; i++)            // dummy - skips over multiple entries
        fscanf(inFile, "%s", dummy);
    fscanf(inFile, "%ld", &(proc->vsize));  // Process vsize

    // close file and return process structure
    fclose(inFile);
    return proc;
};

// Recursively finds and prints all child processes of a given process.
void findChildren(int pid, struct process *proc, int recLayer) {


    while (proc != NULL) {

        if (proc->ppid == pid) {

            // Prints spaces for tree-like viewing of processes
            for (int i = 0; i < recLayer; i++)
                printf("  ");

            
            printf("(%d) %s, %lu kb\n", proc->pid, proc->comm, (proc->vsize)/1000);

            // Finds all of the children, grandchildren, etc., for the current process
            findChildren(proc->pid, proc, recLayer + 1);
        }


        // Goes to the next process in the singly linked list 
        proc = proc->next;

    }

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

    // Declares a singly linked list of processes and walker node
    struct process *head;
    struct process *curr;

    // compare regex to each file in directory
    while ((de = readdir(dr)) != NULL) {
        reti = regexec(&regex, de->d_name, 0, NULL, 0);
        if (!reti){ // if file is a process


            // Creates the head of a linked list
            if (head == NULL) {
                head = initProc(de->d_name);
                curr = head;
            }


            // Adds new processes to linked list
            // and sets the walker node to the new end of the list.
            else {
                curr->next = initProc(de->d_name);
                curr = curr->next;
            }

            


            
        }
    }


    // Recursively finds all processes with ppid 0
    findChildren(0, head, 0);


    // free memory, close files and end program
    regfree(&regex);
    closedir(dr);
    while (head->next != NULL) {
        curr = head->next;
        head->next = curr->next;
        free(curr);
    }
    free(head);
    printf("Done.\n");
    return 0;
    
}