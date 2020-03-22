#include <stdio.h>
#include <stdbool.h>

int main()
{
    /*
    // test case 1 which gives us a safe sequence :-
    int no_of_process = 5;
    int type_of_resources = 3;
    int avalible[3] = {3, 3, 2};
    int currently_allocated[5][3] = {{0, 1, 0},  // P0    // Allocation Matrix
                                     {2, 0, 0},  // P1
                                     {3, 0, 2},  // P2
                                     {2, 1, 1},  // P3
                                     {0, 0, 2}}; // P4

    int max[5][3] = {{7, 5, 3},  // P0    // MAX Matrix
                     {3, 2, 2},  // P1
                     {9, 0, 2},  // P2
                     {2, 2, 2},  // P3
                     {4, 3, 3}}; // P4
*/
    /*
    // test case 2 where instance of resources less than max of process:-
    int no_of_process = 5;
    type_of_resources = 3;
    int avalible[3] = {3, 3, 2};
    int currently_allocated[5][3] = {{0, 1, 0},  // P0    // Allocation Matrix
                                     {2, 0, 0},  // P1
                                     {3, 0, 2},  // P2
                                     {2, 1, 1},  // P3
                                     {0, 0, 2}}; // P4

    int max[5][3] = {{7, 5, 3},  // P0    // MAX Matrix
                     {3, 2, 2},  // P1
                     {9, 0, 2},  // P2
                     {2, 2, 8},  // P3
                     {4, 3, 3}}; // P4
*/
    /*
    // test case 3 which gives us an unsafe sequence :-
    int no_of_process = 5;
    int type_of_resources = 3;
    int avalible[3] = {3, 3, 0};
    int currently_allocated[5][3] = {{0, 1, 0},  // P0    // Allocation Matrix
                                     {2, 0, 0},  // P1
                                     {3, 0, 2},  // P2
                                     {2, 1, 1},  // P3
                                     {0, 0, 2}}; // P4

    int max[5][3] = {{7, 5, 3},  // P0    // MAX Matrix
                     {3, 2, 2},  // P1
                     {9, 0, 2},  // P2
                     {2, 2, 2},  // P3
                     {4, 3, 3}}; // P4
*/

    //custom test case
    int no_of_process, type_of_resources;

    printf("\n Enter total no of processes:- ");
    scanf("%i", &no_of_process);

    printf("\n Enter total no of resources:- ");
    scanf("%i", &type_of_resources);

    //avalible 1d matrix int avalible[type_of_resources];
    int avalible[type_of_resources];
    for (int i = 0; i < type_of_resources; i++)
    {
        printf("\n Enter avalible for R%i:-", (i));
        scanf("%i", &avalible[i]);
    }
    //max needed by processes 2d matrix
    int max[no_of_process][type_of_resources];
    printf("\n Enter the max allocation matrix for the processes:- ");
    for (int i = 0; i < no_of_process; i++)
    {
        printf("\n Max requirement for P%i:- ", (i));
        for (int j = 0; j < type_of_resources; j++)
        {
            printf("\n Max requirement of  R%i:-", (j));
            scanf("%d", &max[i][j]);
        }
    }
    //2d matrix of currently allocated
    int currently_allocated[no_of_process][type_of_resources];
    printf("\n Enter the allocation matrix for the processes:- ");
    for (int i = 0; i < no_of_process; i++)
    {
        printf("\n Current allocated for P%i:-", (i));
        for (int j = 0; j < type_of_resources; j++)
        {
            printf("\n Allocated instance of  R%i:-", (j));
            scanf("%d", &currently_allocated[i][j]);
        }
    }

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
                printf("Max of P%d for Resource R%d is greater than total instances of Resource R%d", j, i, i);
                return 0;
            }
        }
    }
    //building need matrix from max and currently allocated
    int need[no_of_process][type_of_resources];
    for (int i = 0; i < no_of_process; i++)
    {
        for (int j = 0; j < type_of_resources; j++)
        {
            //need = max -aloocated
            need[i][j] = max[i][j] - currently_allocated[i][j];
        }
    }
    bool finished[no_of_process];
    for (int i = 0; i < no_of_process; i++)
    {
        // means processes are not finished
        finished[i] = false;
    }
    int safe_sequence[no_of_process];
    int count = 0; // putting processes into safe sequence
    for (int i = 0; i < no_of_process; i++)
    {
        safe_sequence[i] = 0;
    }

    int current_process; //this variable keeps value of current process
    do
    {
        printf("\n\n Avalible :- ");
        for (int i = 0; i < type_of_resources; i++)
        {
            printf(" %d ", avalible[i]);
        }

        printf("\nMax matrix:\t\tAllocation matrix:\t\tNeed matrix:\n");
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
            printf("\n");
        }
        current_process = -1;
        for (int i = 0; i < no_of_process; i++)
        {
            //cheching if the current process is finished
            if (!finished[i])
            {
                current_process = i;
                // checking if the avalibility of resources for current process
                for (int j = 0; j < type_of_resources; j++)
                {
                    if (avalible[j] < need[i][j])
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
            printf("\n Process P%d is being executed.....", current_process);
            //current process is being put into safe sequence
            safe_sequence[count] = current_process;
            count++;
            //updating availible resource count
            for (int k = 0; k < type_of_resources; k++)
            {
                //updating availible resource count
                avalible[k] = avalible[k] + currently_allocated[current_process][k];
                // removing all allocated resources to current process
                currently_allocated[current_process][k] = 0;
                max[current_process][k] = 0;
                need[current_process][k] = 0;
                // marking current process finished
                finished[current_process] = true;
            }
        }

    } while (count != no_of_process && current_process != -1);
    // checking if safe sequence was generated
    if (count == no_of_process)
    {
        printf("\n Safe Sequence found:- ");
        for (int i = 0; i < count; i++)
        {
            if (i != count - 1)
                printf(" P%d -> ", safe_sequence[i]);
            else
                printf(" P%d", safe_sequence[i]);
        }
    }
    else
    {
        printf("\n : ( NO SAFE SEQUENCE FOUND ");
    }
}