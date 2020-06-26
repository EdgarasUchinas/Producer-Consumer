#include <stdio.h>
# include <pthread.h>
# include <stdlib.h>
# include <unistd.h>


# define BufferSize 10

void *Producer();
void *Consumer();
char *BUFFER;
int next_in = 0;
int next_out = 0;
pthread_t P_th1;
pthread_t C_th1;
pthread_t C_th2;
int count = 0;
pthread_cond_t Buffer_Not_Full=PTHREAD_COND_INITIALIZER;
pthread_cond_t Buffer_Not_Empty=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mVar=PTHREAD_MUTEX_INITIALIZER;

void append(char x) {
    pthread_mutex_lock(&mVar);

    if (count == BufferSize) {
        pthread_cond_wait(&Buffer_Not_Full, &mVar);
    }
    BUFFER[next_in] = x;
    next_in = (next_in + 1) % BufferSize;
    count++;
    pthread_mutex_unlock(&mVar);
    pthread_cond_signal(&Buffer_Not_Empty);
}


void take (char x) {
    pthread_mutex_lock(&mVar);
    if (count == 0) {
        pthread_cond_wait(&Buffer_Not_Empty,&mVar);
    }

    next_out = (next_out + 1) % BufferSize;
    count--;
    pthread_mutex_unlock(&mVar);
    pthread_cond_signal(&Buffer_Not_Full);


}

void *Consumer() {
    char x;
    int check = 0;

    while (check == 0) {
        usleep(((float) rand() / RAND_MAX) * (2000000 - 500000) + 500000);
        x = BUFFER[next_out];
        take(x);
        printf("Consume item [ %c ]\tqueue size: %d\n", x, count);
        if (x == 126) {
            check = 1;
            pthread_cancel(C_th1);
            pthread_cancel(C_th2);
        }
    }
    return 0;
}
void *Producer() {
    char x = 0;
    int check = 0;

    while (check == 0) {
        usleep(500000);
        x++;
        append(x);
        printf("Produce item [ %c ]\tqueue size: %d\n", x, count);
        if (x == 126) {
            check = 1;
            pthread_cancel(P_th1);
        }
    }
    return 0;
}






int main() {


    BUFFER=(char *) malloc(sizeof(char) * BufferSize);

    pthread_create(&P_th1,NULL,Producer,NULL);
    pthread_create(&C_th1,NULL,Consumer,NULL);
    pthread_create(&C_th2,NULL,Consumer,NULL);


    pthread_join(P_th1,NULL);
    pthread_join(C_th1,NULL);
    pthread_join(C_th2,NULL);

    sleep(3);
    printf("\nThreads have been exited...\n");
    sleep(2);
    printf("Program is Closing...\n");
    sleep(1);
    printf("Programmed By Edgaras Uchinas");

    return 0;
}











