#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>    // For O_* constants
#include <sys/stat.h> // For mode constants
#include <unistd.h>   // For sleep

#define QUEUE_NAME "/example_queue"
#define MAX_SIZE 1024

void sender() {

    struct mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = 1024;

    // if(mq_unlink(QUEUE_NAME) == 0)
    //     fprintf(stdout, "Message queue %s removed from system.\n", QUEUE_NAME);
    // Open or create a message queue
    
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, mqAttr);
    if (mq == -1) {
        perror("Sender: mq_open");
        exit(1);
    }

    // Prepare the message
    char message[MAX_SIZE] = "Hello from Sender!";
    
    // Send the message
    if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
        perror("Sender: mq_send");
        exit(1);
    }

    printf("Sender: Message sent: %s\n", message);

    // Close the message queue
    mq_close(mq);
}

void receiver() {
    // Open the message queue
    mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == -1) {
        perror("Receiver: mq_open");
        exit(1);
    }

    // Buffer to store the received message
    char buffer[MAX_SIZE];

    // Receive the message
    if (mq_receive(mq, buffer, MAX_SIZE, NULL) == -1) {
        perror("Receiver: mq_receive");
        exit(1);
    }

    printf("Receiver: Message received: %s\n", buffer);

    // Close and delete the message queue
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
}

// static void serverDown (mqd_t svrHndl) {
//     printf ("Bringing down server with mqd_t %d.\n", svrHndl);
//     mq_close (svrHndl);
// }

int main() {
    // Simulate two threads using sequential function calls
    sender();
    sleep(1); // Simulate some delay between sender and receiver
    receiver();

    return 0;
}
