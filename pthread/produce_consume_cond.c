#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock;
pthread_cond_t cond;
int food = 0;

void *consumer(void *arg)
{
        while (1) {
                pthread_mutex_lock(&lock);
                pthread_cond_wait(&cond, &lock);
                if (food > 0){
                        usleep(1000);
                        food--;
                        printf("pthid:%lu consume food, now:%d\n", pthread_self(), food);
                }
                pthread_mutex_unlock(&lock);
                

                usleep(100000);           // exchange cpu
        }
}

void *producer(void *arg)
{
        while (1) {
                pthread_mutex_lock(&lock);
                usleep(100000);
                food++;
                printf("produce food,now:%d\n", food);
                pthread_mutex_unlock(&lock);

                pthread_cond_signal(&cond);

                usleep(1000);
        }
}

int main(int argc, char *argv[])
{
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&cond, NULL);
        pthread_t consumer_pthrd_id, producer_pthrd_id;
        pthread_create(&consumer_pthrd_id, NULL, consumer, (void*)NULL);
        pthread_create(&consumer_pthrd_id, NULL, consumer, (void*)NULL);
        pthread_create(&producer_pthrd_id, NULL, producer, (void*)NULL);
        
        while (1);
        return 0;
}
