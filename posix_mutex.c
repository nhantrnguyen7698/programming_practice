#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
struct user{
    char name[100];
    char age[5];
    char phone_num[20];
    char file_name[100];
};
pthread_t thrd1;
pthread_t thrd2;

// Shared data and synchronization primitives
struct user shared_data;
int data_ready = 0; // Flag to indicate data availability
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* scan_and_send_data(void* arg){

    while (1)
    {
        pthread_mutex_lock(&mutex);
        // char send_msg[MAX_SIZE];
        
        printf("New data...\n");
        printf("Input the name\n");
        scanf("%s", shared_data.name);
        // fgets(x.name, sizeof(x.name), stdin);
        printf("How old are you?\n");
        scanf("%s", shared_data.age);
        printf("What is your phone number?\n");
        scanf("%s", shared_data.phone_num);
        printf("What is the directory of photo folder?\n");
        scanf("%s", shared_data.file_name);

        if (strlen(shared_data.name) == 0 || strlen(shared_data.phone_num) == 0 || strlen(shared_data.file_name) == 0) {
            printf("Error: All fields must be filled.\n");
            continue;
        }

        // Mark data as ready and signal the consumer
        data_ready = 1;
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

void* recv_and_print_out(void* arg){
    while (1)
    {
        pthread_mutex_lock(&mutex);
        
        // Wait for the producer to signal new data
        while (data_ready == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Process the shared data
        printf("Received Data:\n");
        printf("Name: %s\n", shared_data.name);
        printf("Age: %s\n", shared_data.age);

        // Validate phone number
        if (strlen(shared_data.phone_num) == 10) {
            printf("Phone Number: %s\n", shared_data.phone_num);
        } else {
            printf("Invalid phone number!\n");
        }
        data_ready = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&thrd1, NULL, scan_and_send_data, NULL);
    pthread_create(&thrd2, NULL, recv_and_print_out, NULL);

    // Wait for user input to terminate

    pthread_join(thrd1, NULL);
    pthread_join(thrd2, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}