//******* I USED "THREADS" instead of "PROCESSES" *******//
//******* I USED "THREADS" instead of "PROCESSES" *******//
#include <stdio.h>
#include "stdbool.h"
#include "stdlib.h"


#define MAX 100//MAX number of resources and threads
int num_of_threads = 0;
int num_of_resources = 0;

int allocation_matrix[MAX][MAX];
int request_matrix[MAX][MAX];
int available_vector[MAX];
int work_vector[MAX];
bool finish[MAX];

void read_allocation_matrix(char *file_name[]) {
    FILE *file;

    file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening Allocation file");
        exit(0);
    }

    // skip the header line
    fscanf(file, "%*[^\n]\n");

    // read allocation matrix
    num_of_threads = 0;
    num_of_resources = 0;
    int temp;
    while (fscanf(file, "%*[^,],") != EOF) {
        num_of_resources = 0;
        while (fscanf(file, "%d,", &allocation_matrix[num_of_threads][num_of_resources]) == 1) {
            num_of_resources++;
        }

        if (num_of_threads != 0 && temp != num_of_resources) {
            printf("error: DIMENSIONS ARE NOT CONSISTENT!\n(resources number is not the same!)");
            exit(0);
        }

        temp = num_of_resources;
        fscanf(file, "\n");
        num_of_threads++;
    }

    fclose(file);
}

void read_request_matrix(char *file_name[]) {
    FILE *file;

    file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening Request file");
        exit(0);
    }

    // skip the header line
    fscanf(file, "%*[^\n]\n");

    // read request matrix
    num_of_threads = 0;
    int temp;
    while (fscanf(file, "%*[^,],") != EOF) {
        num_of_resources = 0;
        while (fscanf(file, "%d,", &request_matrix[num_of_threads][num_of_resources]) == 1) {
            num_of_resources++;
        }
        if (num_of_threads != 0 && temp != num_of_resources) {
            printf("error: DIMENSIONS ARE NOT CONSISTENT!\n(resources number is not the same!)");
            exit(0);
        }

        temp = num_of_resources;
        fscanf(file, "\n");
        num_of_threads++;
    }

    fclose(file);
}

void read_available_vector(char *file_name[]) {
    FILE *file;

    file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening Available file");
        exit(0);
    }

    // skip the header line
    fscanf(file, "%*[^\n]\n");

    // read available vector and determine the number of resources
    num_of_resources = 0;
    while (fscanf(file, "%d,", &available_vector[num_of_resources]) == 1) {
        num_of_resources++;
    }

    fclose(file);
}


bool work_request_detector(int request[num_of_resources]) {
    //function to detect if the request<=work or not
    for (int i = 0; i < num_of_resources; ++i) {
        if (work_vector[i] < request[i])
            return false;
    }

    return true;
}


int main() {

    //array to print the execution of threads in the right order and its index
    int current_series_index = 0;
    int safe_series[MAX];

    //the paths or names of input file
    char request_file[] = "C:\\Users\\hp\\Desktop\\untitled\\Request.csv";
    char allocation_file[] = "C:\\Users\\hp\\Desktop\\untitled\\Allocation.csv";
    char available_file[] = "C:\\Users\\hp\\Desktop\\untitled\\Available.csv";

    //in these arrays I save every file how many resources and threads it is has
    int consistent_num_threads[2];
    int consistent_num_resources[3];

    //read available file and get how many resources its has and save it
    read_available_vector(available_file);
    consistent_num_resources[0] = num_of_resources;

    //read allocation file and get how many resources and threads its has and save it
    read_allocation_matrix(allocation_file);
    consistent_num_resources[1] = num_of_resources;
    consistent_num_threads[0] = num_of_threads;

    //read request file and get how many resources and threads its has and save it
    read_request_matrix(request_file);
    consistent_num_resources[2] = num_of_resources;
    consistent_num_threads[1] = num_of_threads;



    //if the number of resources is not the same in the three files then the input it is not consistent
    if (consistent_num_resources[0] != consistent_num_resources[1] ||
        consistent_num_resources[0] != consistent_num_resources[2]) {
        printf("error: DIMENSIONS ARE NOT CONSISTENT!\n(resources number is not the same!)");
        return 0;
    }

    //if the number of threads is not the same in the three files then the input it is not consistent
    if (consistent_num_threads[0] != consistent_num_threads[1]) {
        printf("error: DIMENSIONS ARE NOT CONSISTENT!\n(threads number is not the same!)");
        return 0;
    }

    //commented code used to make sure that the read process done without errors
//    for (int i = 0; i < num_of_threads; ++i) {
//        for (int j = 0; j < num_of_resources; ++j) {
//            printf("%d\t",request_matrix[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n"); printf("\n"); printf("\n");
//    for (int i = 0; i < num_of_threads; ++i) {
//        for (int j = 0; j < num_of_resources; ++j) {
//            printf("%d\t",allocation_matrix[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n"); printf("\n"); printf("\n");
//    for (int i = 0; i < num_of_resources; ++i) {
//        printf("%d\t",available_vector[i]);
//    }
//    printf("\n"); printf("\n"); printf("\n");

    printf("THE DIMENSIONS ARE CONSISTENT\n");
    printf("\tNUMBER OF THREADS = %d\n", num_of_threads);
    printf("\tNUMBER OF RESOURCES = %d\n\n", num_of_resources);

    //initialise the work vector to available
    for (int i = 0; i < num_of_resources; ++i) {
        work_vector[i] = available_vector[i];
    }

    //initialise the finish vector to false
    for (int i = 0; i < num_of_threads; ++i) {
        finish[i] = false;
    }

    //int to know the current thread ID I check
    int current_thread = 0;

    //flag for the deadlock
    bool deadlock_flag = true;

    //counter that if its excess number of threads this mean the deadlock situation
    int counter = 0;

    while (1) {
        if (current_series_index == num_of_threads) {//means that all threads finished
            printf("THE SYSTEM IS NOT DEADLOCKED\n");
            printf("There is series of threads executions that are possible without deadlock :\n");
            for (int i = 0; i < num_of_threads; ++i) {//print a safe series
                printf("T%d==>", safe_series[i]);
            }
            printf("finish\n");
            return 0;
        }
        if (finish[current_thread] == false && work_request_detector(request_matrix[current_thread])) {
            //if there is thread didn't finish and request<=work then do it
            for (int i = 0; i < num_of_resources; ++i) {
                //add the allocation resources of this thread to the work
                work_vector[i] += allocation_matrix[current_thread][i];
            }
            safe_series[current_series_index] = current_thread + 1;//used index 0 so I add 1
            finish[current_thread] = true;//change the flag
            current_series_index++;//new thread done
            if (current_thread < num_of_threads - 1)//go to the next thread
                current_thread++;//in case that the current thread < 9 (9 not 10 because index 0)
            else current_thread = 0;//back to the first thread
            counter = 0;//we're done thread then the streak of threads that's deadlocked = 0
            continue;
        } else {
            counter++;//new thread without execute
            if (current_thread < num_of_threads - 1)//go to the next thread
                current_thread++;//in case that the current thread < 9 (9 not 10 because index 0)
            else current_thread = 0;//back to the first thread
        }
        if (counter >
            num_of_threads) {//that's mean that we check all the threads and no one of them executable (DEADLOCK)
            deadlock_flag = true;
            break;
        }
    }


    printf("DEADLOCK SYSTEM !!!\n");
    printf("DEADLOCKED THREADS :\n");
    for (int i = 0; i < num_of_threads; ++i) {
        if (!finish[i])//print threads that are not finished (caused the deadlock)
            printf("\tT%d\n", i + 1);
    }


    return 0;
}
