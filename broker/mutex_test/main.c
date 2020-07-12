#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <semaphore.h>

pthread_t thread_a;
pthread_t thread_b;
pthread_mutex_t s_lista;
sem_t s_orden;
sem_t s_enLaSala;

void addElementA(t_list* lista)
{
    for (int i = 0; i < 40; i++)
    {
        sem_wait(&s_enLaSala);
        pthread_mutex_lock(&s_lista);
            list_add(lista, (void*)'a');
            printf("Añadido algo\n");
        pthread_mutex_unlock(&s_lista);
        sem_post(&s_orden);
    }
}

void removeElement(t_list* lista)
{
    for (int i = 0; i < 40; i++)
    {
        sem_wait(&s_orden);
        pthread_mutex_lock(&s_lista);
            list_remove(lista, 0);
            printf("Removio algo\n");
        pthread_mutex_unlock(&s_lista);
        sem_post(&s_enLaSala);
    }
}

int main() 
{
    t_list* lista = list_create();
    sem_init(&s_orden, 0, 0);
    sem_init(&s_enLaSala, 0, 1);

    pthread_create(&thread_a, NULL, (void*)addElementA, lista);
    pthread_create(&thread_b, NULL, (void*)removeElement, lista);

	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

    printf("Tamaño lista: %i", list_size(lista));
}