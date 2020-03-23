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
pthread_cond_t mutexcondition;     // for making process execute in safe sequence this condion is implemented
int total_processes_completed = 0; // this makes sure while threads run they get in to critical section in safe sequence

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
    // marking all safe sequence positions -1
    for (int i = 0; i < no_of_process; i++)
    {
        safe_sequence[i] = -1;
    }
    //allocating values to arrays and matrixes
    for (int i = 0; i < c; i++)
    {
        printf("\n Enter Avalible instance of R%d:- ", i);
        scanf("%i", &avalible[i]);
    }
    printf("\n Max required :- ");
    for (int i = 0; i < r; i++)
    {
        printf("\n FOR P%i :- ", i);
        for (int j = 0; j < c; j++)
        {
            printf("\n R%i:- ", j);
            scanf("%i", &max[i][j]);
        }
    }
    printf("\n Currently Allocated required :- ");
    for (int i = 0; i < r; i++)
    {
        printf("\n FOR P%i :- ", i);
        for (int j = 0; j < c; j++)
        {
            printf("\n R%i:- ", j);
            scanf("%i", &currently_allocated[i][j]);
        }
    }
    //calculating need matrix
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            need[i][j] = max[i][j] - currently_allocated[i][j];
        }
    }
    /*
    // testing if dynamically allocated memory worked for need matrix
    printf("\n \n");
    for (int i = 0; i < r; i++)
    {

        for (int j = 0; j < c; j++)
        {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }*/

    //checking if max need of any process exceeds total avalible instances of a resource
    for (int i = 0; i < type_of_resources; i++)
    {
        int total = avalible[i];
        for (int j = 0; j < no_of_process; j++)
        {
            total += currently_allocated[j][i];
        }
        for (int j = 0; j < no_of_process; j++)
        {
            //if total instances of resource less than max required of any process
            if (total < max[j][i])
            {
                printf("\n Max of P%d for Resource R%d is greater than total instances of Resource R%d", j, i, i);
                return 0;
            }
        }
    }
    //banker's algorithm to find safe sequence and check if state is safe
    bool safe_sequence_not_found = true;
    //here we use a temporary avalible array as original one will be updated by threads
    int t_avalible[type_of_resources]; // just to check for safe state this is tempory avalible array
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
    int count = 0;       // putting processes into safe sequence
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
    }
    //all matrixes and avalible array before starting to make threads.
    printf("\n\n Value Of All Matrixes Before Creating Multiple Threads :-  ");
    printf("\n\n Avalible :- ");
    for (int i = 0; i < type_of_resources; i++)
    {
        printf(" R%d :-  %d | ", i, avalible[i]);
    }
    printf("\n");
    printf("\n Max matrix:\t\tAllocation matrix:\t\tNeed matrix:\n ");
    for (int i = 0; i < no_of_process; i++)
    {
        for (int j = 0; j < type_of_resources; j++)
            printf("%d  ", max[i][j]);
        printf("\t\t");
        for (int j = 0; j < type_of_resources; j++)
            printf("%d  ", currently_allocated[i][j]);
        printf("\t\t");
        for (int j = 0; j < type_of_resources; j++)
            printf("%d  ", need[i][j]);
        printf("\n ");
    }
    printf("\n\n Creating and Executing %d Threads One for Each Process In Safe Sequence ", no_of_process);
    pthread_t processes[no_of_process]; // threads ids
    //pthread_attr_t attr;
    //pthread_attr_init(&attr);
    //creating threads
    int process_numbers[no_of_process];
    for (int i = 0; i < no_of_process; i++)
    {
        process_numbers[i] = i;
    }
    for (int i = 0; i < no_of_process; i++)
    {
        pthread_create(&processes[i], NULL, thread_function, &process_numbers[i]);
    }
    //waiting for all threads to finish
    for (int i = 0; i < no_of_process; i++)
    {
        pthread_join(processes[i], NULL);
    }
    printf("\n All Programs Successefully Executed :) ");
    sleep(4);
}
void *thread_function(void *args)
{
    //type casting and de refrencing the recieved parameter
    int process_number = *((int *)args);
    //printf("\n HELLO FROM P %d", process_number);//testing

    pthread_mutex_lock(&mutexlock); // getting lock
    //ensuring safe sequence by checking the condition
    while (process_number != safe_sequence[total_processes_completed])
        pthread_cond_wait(&mutexcondition, &mutexlock);
    // code entering critical section
    printf("\n\n PROCESS %d is being executed ", process_number);
    sleep(1);
    printf("\n Max required :- ");
    for (int i = 0; i < type_of_resources; i++)
    {
        printf(" %d ", max[process_number][i]);
    }
    printf("\n Currently Allocated :- ");
    for (int i = 0; i < type_of_resources; i++)
    {
        printf(" %d ", currently_allocated[process_number][i]);
    }
    printf("\n Needed :- ");
    for (int i = 0; i < type_of_resources; i++)
    {
        printf(" %d ", need[process_number][i]);
    }
    printf("\n Avalible :- ");
    for (int i = 0; i < type_of_resources; i++)
    {
        printf(" %d ", avalible[i]);
    }
    sleep(2);
    printf("\n Resource Allocated to P%d", process_number);
    //releasing resource
    for (int i = 0; i < type_of_resources; i++)
    {
        avalible[i] += currently_allocated[process_number][i];
    }
    printf("\n Resource Released \n");
    sleep(2);
    //incrementing process completed
    total_processes_completed++;

    //releasing mutex lock and updating condition
    pthread_cond_broadcast(&mutexcondition);
    pthread_mutex_unlock(&mutexlock);
    return NULL;
}
