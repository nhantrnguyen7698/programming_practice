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

#define QUEUE_NAME "/ditmemay"
#define MAX_SIZE 1024

volatile int running = 1;

static mqd_t serverUp (void) {
    int rc;
    mqd_t svrHndl;
    struct mq_attr mqAttr;

    printf ("Bringing up server.\n");
    rc = mq_unlink (QUEUE_NAME);
    if (rc < 0) {
        printf ("   Warning %d (%s) on server mq_unlink.\n",
            errno, strerror (errno));
    }

    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = 1024;
    svrHndl = mq_open (QUEUE_NAME, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR, &mqAttr);
    if (svrHndl < 0) {
        printf ("   Error %d (%s) on server mq_open.\n",
            errno, strerror (errno));
        exit (1);
    }
    printf ("   Server opened mqd_t of %d.\n", svrHndl);
    return svrHndl;
}

static void serverDown (mqd_t svrHndl) {
    printf ("Bringing down server with mqd_t %d.\n", svrHndl);
    mq_close (svrHndl);
}

void* scan_and_send_data(void* arg){
    struct user x ;
    mqd_t mq = *(mqd_t*)arg;

    while (running)
    {
        // char send_msg[MAX_SIZE];
        
        printf("New data...\n");
        printf("Input the name\n");
        scanf("%s", x.name);
        // fgets(x.name, sizeof(x.name), stdin);
        printf("How old are you?\n");
        scanf("%s", x.age);
        printf("What is your phone number?\n");
        scanf("%s", x.phone_num);
        printf("What is the directory of photo folder?\n");
        scanf("%s", x.file_name);

        if (strlen(x.name) == 0 || strlen(x.phone_num) == 0 || strlen(x.file_name) == 0) {
            printf("Error: All fields must be filled.\n");
            continue;
        }

        if(mq_send(mq, (const char*)&x, sizeof(x), 0) < 0){
            perror("Sender: mq_send");
            continue; // Skip to the next iteration instead of exiting
        }
        sleep(1);
    }
    mq_close (mq);
}

void* recv_and_print_out(void* arg){
    struct user y;
    const char delimiter[] = "*";
    char *token;
    mqd_t mq = *(mqd_t*)arg;
    while (running)
    {
        if (mq == -1) {
            perror("Receiver: mq_open");
            exit(1);
        }
        if (mq_receive(mq, (char*)&y, MAX_SIZE, NULL)) {
            printf("Name: %s\n", y.name);
            printf("Age: %s\n", y.age);
            printf("Phone: %s\n", y.phone_num);
            printf("File: %s\n", y.file_name);
            if(strlen(y.phone_num) != 10){
                printf("The telephone number is invalid!!!\n");
                continue; // Skip to the next iteration instead of exiting
            }
            char command[256];
            snprintf(command, sizeof(command), "eog \"%s\"", y.file_name);
            system(command);
        }
    }
}

int main(){
    mqd_t svrHndl;
    svrHndl = serverUp ();
    pthread_create(&thrd1, NULL, scan_and_send_data, &svrHndl);
    pthread_create(&thrd2, NULL, recv_and_print_out, &svrHndl);

    // Wait for user input to terminate
    getchar();
    running = 0;

    pthread_join(thrd1, NULL);
    pthread_join(thrd2, NULL);
    serverDown(svrHndl);
    return 0;
}