#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
// varibles and matrixes to hold  value
int no_of_process = 0;
int type_of_resources = 0;
//as these resources will be shared by all threads this must be in global scope of the program
// as no of process and type of resources are uncertain we allocate them manually using dynamic memory allocation
// here double pointer method is used as other methods require additional info
int *avalible;
int **max;                 // max needed by process
int **currently_allocated; // currently the resource in hold by process
int **need;                //max - currently calculated
// 1d array  which contains safe sequence if found
int *safe_sequence;

//mutex lock variables
pthread_mutex_t mutexlock;
pthread_cond_t mutexcondition;
int total_processes_completed = 0;

//function for process that will be run by each new thread
void *thread_function(void *);
int main()
{
    printf("\n Enter total no of processes:- ");
    scanf("%i", &no_of_process);

    printf("\n Enter total no of resources:- ");
    scanf("%i", &type_of_resources);
    //r = row (no of process) c= column (type of processes)
    //this is done to make it easy to assign memory to matrixes
    int r = no_of_process;
    int c = type_of_resources;
    // allocating memory space to matrixes dynamically
    avalible = (int *)malloc(c * sizeof(*avalible));
    safe_sequence = (int *)malloc(r * sizeof(*safe_sequence));
    max = (int **)malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++)
        max[i] = (int *)malloc(c * sizeof(int));
    currently_allocated = (int **)malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++)
        currently_allocated[i] = (int *)malloc(c * sizeof(int));
    need = (int **)malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++)
        need[i] = (int *)malloc(c * sizeof(int));

    //end of memory allocation
    int no_of_processz = 5;
    int type_of_resourcesz = 3;
    int avaliblez[3] = {3, 3, 2};
    int currently_allocatedz[5][3] = {{0, 1, 0},  // P0    // Allocation Matrix
                                      {2, 0, 0},  // P1
                                      {3, 0, 2},  // P2
                                      {2, 1, 1},  // P3
                                      {0, 0, 2}}; // P4

    int maxz[5][3] = {{7, 5, 3},  // P0    // MAX Matrix
                      {3, 2, 2},  // P1
                      {9, 0, 2},  // P2
                      {2, 2, 2},  // P3
                      {4, 3, 3}}; // P4
    for (int i = 0; i < no_of_process; i++)
    {
        safe_sequence[i] = -1;
        printf("%d ", safe_sequence[i]);
    }
    //allocating values to arrays and matrixes
    for (int i = 0; i < c; i++)
    {
        // printf("\n Enter Avalible instance of R%d", i);
        avalible[i] = avaliblez[i]; //scanf("%i", &avalible[i]);
        //printf("%i", avalible[i]);
    }
    printf("\n Max required :- ");
    for (int i = 0; i < r; i++)
    {
        //printf("\n FOR P%i :- ", i);
        for (int j = 0; j < c; j++)
        {
            //printf("\n R%i:- ", j);
            max[i][j] = maxz[i][j]; //scanf("%i", &max[i][j]);
        }
    }
    printf("\n Currently Allocated required :- ");
    for (int i = 0; i < r; i++)
    {
        //printf("\n FOR P%i :- ", i);
        for (int j = 0; j < c; j++)
        {
            //printf("\n R%i:- ", j);
            currently_allocated[i][j] = currently_allocatedz[i][j]; //scanf("%i", &currently_allocated[i][j]);
        }
    }
    // making all safe sequence -1

    //calculating need matrix
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            need[i][j] = max[i][j] - currently_allocated[i][j];
        }
    }
    // testing if dynamically allocated memory worked for need matrix
    printf("\n \n");
    for (int i = 0; i < r; i++)
    {

        for (int j = 0; j < c; j++)
        {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
    //banker's algorithm to find safe sequence and check if state is safe
    bool safe_sequence_not_found = true;
    //here we use a temporary avalible array as original one will be updated by threads
    int t_avalible[type_of_resources]; // just to check for safe state
    for (int i = 0; i < type_of_resources; i++)
    {
        t_avalible[i] = avalible[i];
    }

    bool finished[no_of_process];
    for (int i = 0; i < no_of_process; i++)
    {
        // means processes are not finished
        finished[i] = false;
    }
    int count = 0; // putting processes into safe sequence
    for (int i = 0; i < no_of_process; i++)
    {
        safe_sequence[i] = -1;
    }
    int current_process; //this variable keeps value of current process
    do
    {
        current_process = -1; // -1 means currently no process is being evaluated
        for (int i = 0; i < no_of_process; i++)
        {
            //cheching if the current process is finished
            if (!finished[i])
            {
                current_process = i;
                // checking if the avalibility of resources for current process
                for (int j = 0; j < type_of_resources; j++)
                {
                    if (t_avalible[j] < need[i][j])
                    {
                        // if this condition is true this process cannot be executed right now.
                        current_process = -1;
                        break;
                    }
                }
            }
            // this conditions checks if we have found
            if (current_process != -1)
            {
                break;
            }
        }
        if (current_process != -1)
        {
            //current process is being put into safe sequence
            safe_sequence[count] = current_process;
            count++;
            //updating availible resource count
            for (int k = 0; k < type_of_resources; k++)
            {
                //updating availible resource count
                t_avalible[k] = t_avalible[k] + currently_allocated[current_process][k];
                // marking current process finished
                finished[current_process] = true;
            }
        }

    } while (count != no_of_process && current_process != -1);
    // checking if safe sequence was generated
    if (count == no_of_process)
    {
        safe_sequence_not_found = false;
    }
    if (safe_sequence_not_found)
    {
        printf("\n Unable to Generate Safe Sequence :(");
        return 0;
    }
    printf("\n Safe Sequence found:- ");
    for (int i = 0; i < r; i++)
    {
        if (i != r - 1)
            printf(" P%d -> ", safe_sequence[i]);
        else
            printf(" P%d", safe_sequence[i]);
    } /*
    for (int i = 0; i < type_of_resources; i++)
    {
        printf("\nR%d :- %d", i, avalible[i]);
    }
    for (int i = 0; i < type_of_resources; i++)
    {
        printf("\ntR%d :- %d", i, t_avalible[i]);
    }*/
}
