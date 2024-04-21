#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
 
#include "p5_helper.h"
 
#define BUFFER_MAX_SIZE 4
 
unsigned int data_space[BUFFER_MAX_SIZE];
circ_buff_t buffer = { 
    data_space,
    BUFFER_MAX_SIZE,
    0,
    0
};

pthread_mutex_t mutex;
pthread_cond_t consumidor;
pthread_cond_t produtor;

/*void *producer(void *arg)
{
    unsigned int push_value;
    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (circ_buff_current_size(&buffer) == BUFFER_MAX_SIZE)
        {                                         // Wait until the buffer is not full
            pthread_cond_wait(&produtor, &mutex); // Wait for the consumer to signal that there is space
        }

        push_value = (rand() % 1000); // random [0,999]
        if (circ_buff_push(&buffer, push_value) == 0)
        {
            printf("Producer: %u\n", push_value);
            pthread_cond_broadcast(&consumidor); // Sinaliza todas as threads consumidoras que estão esperando
        }
        else
            printf("Producer: buffer is full\n");

        pthread_mutex_unlock(&mutex);
        usleep(100 * 1000); // 100 ms
    }

    return NULL;
}

void *consumer(void *arg)
{
    unsigned int pop_value;
    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (circ_buff_current_size(&buffer) == 0)
        {                                           // Wait until the buffer is not empty
            pthread_cond_wait(&consumidor, &mutex); // Wait for the producer to signal that there is data
        }

        if (circ_buff_pop(&buffer, &pop_value) == 0)
        {
            printf("                              Consumer: returned %u\n", pop_value);
            pthread_cond_signal(&produtor); // Signal the producer that there is space
        }
        else
        {
            printf("                              Consumer: buffer is empty\n");
        }

        pthread_mutex_unlock(&mutex);
        usleep(150 * 1000); // 150 ms
    }

    return NULL;
}*/

void* producer(void *arg)
{
    unsigned int push_value;
    while (1) {
        pthread_mutex_lock(&mutex);

        push_value = (rand() % 1000); //random [0,999]
        if (circ_buff_push(&buffer, push_value ) == 0){
            printf("Producer: %u\n", push_value);
            //pthread_cond_signal(&consumidor);
            pthread_cond_broadcast(&consumidor);
        }
        else
            printf("Producer: buffer is full\n"); 
        
        pthread_mutex_unlock(&mutex);
        usleep(100*1000); //100 ms
    }
    
    return NULL;
}
 
void* consumer(void *arg)
{
    unsigned int pop_value;
    pthread_t tid = pthread_self(); // Get the thread ID

    while (1) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&consumidor, &mutex);
        if (circ_buff_pop(&buffer, &pop_value)==0)
            printf("                              Consumer ID %lu: returned %u\n", (unsigned long)tid, pop_value);
        else
            printf("                              Consumer ID %lu: buffer is empty\n", (unsigned long)tid);

        pthread_mutex_unlock(&mutex);
        usleep(150*1000); //150 ms
    }
    
    return NULL;
}

int main(void)
{
    //Seeding...
    srand(time(NULL));
 
    pthread_t tid[4];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&consumidor, NULL);
    //pthread_cond_init(&produtor, NULL);
    
    pthread_create(&(tid[0]), NULL, &producer, NULL);
    pthread_create(&(tid[1]), NULL, &consumer, NULL);
    pthread_create(&(tid[2]), NULL, &consumer, NULL);
    pthread_create(&(tid[3]), NULL, &consumer, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);

    pthread_cond_destroy(&consumidor);
    //pthread_cond_destroy(&produtor);
    pthread_mutex_destroy(&mutex);
    return 0;
}
