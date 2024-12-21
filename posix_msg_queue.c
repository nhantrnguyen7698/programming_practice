#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

struct user{
    char name[100];
    int age;
    char phone_num[20];
    char file_name[100];
};
pthread_t thrd1;
pthread_t thrd2;

void* scan_and_send_data(void* arg){
    struct user x ;
    
    printf("New data...\n");
    printf("Input the name\n");
    fgets(x.name, sizeof(x.name), stdin);
    printf("How old are you?\n");
    scanf("%d", &x.age);
    printf("What is your phone number?\n");
    scanf("%s", x.phone_num);

}

void* recv_and_print_out(void* arg){

}

int main(){
    pthread_create(&thrd1, NULL, scan_and_send_data, NULL);
    pthread_create(&thrd2, NULL, recv_and_print_out, NULL);
}