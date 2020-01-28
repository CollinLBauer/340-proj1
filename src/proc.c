#include <stdio.h>
#include <dirent.h> 
#include <stdlib.h>
#include <string.h>



struct process {
    int pid;
    int ppid;
    char comm[255];
    unsigned long int vsize;

};

struct process* initProc(char fileName[]) {

    struct process *proc = malloc(sizeof(struct process));
    FILE *inFile = fopen(fileName, "r");
    int dummy;
    
    // test for files not existing. 
    if (inFile == NULL) 
    {   
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


int main(int argc, char *argv[]) 
{ 
    
    char name[255];
    strcat(name, "/proc/");
    strcat(name, argv[1]);
    strcat(name, "/stat");
    struct process* testProc = initProc(name);
    printf("pid <%d>, ppid <%d>, vsize <%ld>, comm <%s>\n", testProc->pid, testProc->ppid, testProc->vsize, testProc->comm);
};