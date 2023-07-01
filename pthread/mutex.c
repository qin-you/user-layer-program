#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t locka;
pthread_mutex_t lockb;
pthread_mutex_t lockc;
int count = 3;

void *funa()
{
        while (count > 0) {
                pthread_mutex_lock(&locka);
                printf("A ");
                pthread_mutex_unlock(&lockb);
        }
        pthread_mutex_destroy(&locka);
        return NULL;
}

void *funb()
{
        while (count > 0) {
                pthread_mutex_lock(&lockb);
                printf("B ");
                pthread_mutex_unlock(&lockc);
        }
        pthread_mutex_destroy(&lockb);
        return NULL;
}

void *func()
{
        while (count > 0) {
                count --;
                pthread_mutex_lock(&lockc);
                printf("C\n");
                sleep(1);
                pthread_mutex_unlock(&locka);
        }
        pthread_mutex_destroy(&lockc);
        return NULL;
}

int main()
{
        pthread_mutex_init(&locka, NULL);
        pthread_mutex_init(&lockb, NULL);
        pthread_mutex_init(&lockc, NULL);
        pthread_mutex_lock(&locka);
        pthread_mutex_lock(&lockb);
        pthread_mutex_lock(&lockc);
        pthread_t pthreadida, pthreadidb, pthreadidc;
        pthread_create(&pthreadida, NULL, funa, NULL);
        pthread_create(&pthreadidb, NULL, funb, NULL);
        int ret = pthread_create(&pthreadidc, NULL, func, NULL);
        if (ret != 0){
                strerror(ret);
                exit(1);
        }
        pthread_mutex_unlock(&locka);

        void **tmp;
        pthread_join(pthreadidc, tmp);
        
        printf("over!\n");
        return 0;


        
}
