/*
 * CS 232 Project 5 - Vaccination Clinic
 * Ben DeWeerd
 * 3.17.2022
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_VIALS 30
#define SHOTS_PER_VIAL 6
#define NUM_CLIENTS (NUM_VIALS * SHOTS_PER_VIAL)
#define NUM_NURSES 10
#define NUM_STATIONS NUM_NURSES
#define NUM_REGISTRATIONS_SIMULTANEOUSLY 4

/* global variables */
int num_vials_left = NUM_VIALS;
sem_t num_vials_sem;
sem_t client_assignment_sem;
sem_t nurse_assignment_sem;
sem_t registration_desk_sem;

sem_t client_ready_sems[NUM_NURSES];
sem_t vacc_complete_sems[NUM_NURSES];

pthread_t nurses[NUM_NURSES];
pthread_t clients[NUM_CLIENTS];

typedef struct{
    int nurseId;
} Station;

/* nurse-client producer-consumer bounded buffer */
Station assignmentQueue[NUM_NURSES];
int assignIn = 0;
int assignOut = 0;
int count = 0;

int get_rand_in_range(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

char *time_str;
char *curr_time_s() {
    time_t t;
    time(&t);
    time_str = ctime(&t);
    // replace ending newline with end-of-string.
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

// lower and upper are in seconds, usleep() expects microseconds
void walk(int lower, int upper) {
    usleep(get_rand_in_range(lower, upper) * 1000000);
}

// nurse thread, arg defines ID number
void *nurse(void *arg) {

    long int id = (long int)arg;
    bool has_vial = false;
    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id);

    do{
        // walk to get vial of vaccine, 1-3 seconds
        fprintf(stderr, "%s: nurse %ld is walking to get a vial of vaccine\n", curr_time_s(), id);
        walk(1, 3);

        has_vial = false;
        sem_wait(&num_vials_sem);
        // check for vials
        if(num_vials_left > 0){
            num_vials_left--;
            has_vial = true;
            fprintf(stderr, "%s: nurse %ld got a new vial, there are %d vials left\n", curr_time_s(), id, num_vials_left);
        } else {
            fprintf(stderr, "%s: nurse %ld sees there are no more vials of vaccine\n", curr_time_s(), id);
        }
        sem_post(&num_vials_sem);

        if(has_vial){
            // walk back to station, 1-3 seconds
            walk(1,3);
            fprintf(stderr, "%s: nurse %ld back at station\n", curr_time_s(), id);

            // vaccinate 6 clients per vial
            for(int shots = 5; shots >= 0; shots--){

                // indicate nurse is available
                sem_wait(&nurse_assignment_sem);
                fprintf(stderr, "%s: nurse %ld tells the waiting queue they are available\n", curr_time_s(), id);
                assignmentQueue[assignIn].nurseId = id;
                assignIn = (assignIn + 1) % NUM_STATIONS;
                count++;
                sem_post(&nurse_assignment_sem);

                fprintf(stderr, "%s: nurse %ld waiting for a client to arrive\n", curr_time_s(), id);

                // wait for client to indicate readiness
                sem_wait(&client_ready_sems[id]);
                fprintf(stderr, "%s: nurse %ld sees client is ready, starts vaccination\n", curr_time_s(), id);
                // vaccinate the client, 5 seconds
                sleep(5);
                fprintf(stderr, "%s: nurse %ld gave client the shot. %i shot(s) left in vial\n", curr_time_s(), id, shots);
                sem_post(&vacc_complete_sems[id]);
            }
        }
         
    } while(has_vial);

    // if no more vials, leave clinic
    fprintf(stderr, "%s: nurse %ld is done\n", curr_time_s(), id);
    pthread_exit(NULL);
}

// client thread, arg defines ID number
void *client(void *arg) {
    long int id = (long int)arg;
    long int stationNum;

    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n",
            curr_time_s(), id);

    // walk to registration desk, 3-10 seconds
    walk(3, 10);
    // wait for opening at registration desk (4 slots)
    fprintf(stderr, "%s: client %ld is waiting to register\n", curr_time_s(), id);
    sem_wait(&registration_desk_sem);
    fprintf(stderr, "%s: client %ld is registering\n", curr_time_s(), id);
    // take between 3 and 10 seconds to register
    walk(3, 10);
    sem_post(&registration_desk_sem);

    // walk to station-assignment queue, 3-10 seconds
    fprintf(stderr, "%s: client %ld is walking to the station-assignment queue\n", curr_time_s(), id);
    walk(3, 10);
    // wait for station assignment
    sem_wait(&client_assignment_sem);
    while(count == 0){
        ;
    }
    stationNum = assignmentQueue[assignOut].nurseId;
    assignOut = (assignOut + 1) % NUM_STATIONS;
    count--; 
    sem_post(&client_assignment_sem);

    // walk to assigned nurse's station, 1-2 seconds
    fprintf(stderr, "%s: client %ld got assigned to station %ld: walking there now\n", curr_time_s(), id, stationNum);
    walk(1,2);
    fprintf(stderr, "%s: client %ld arrived at station %ld\n", curr_time_s(), id, stationNum);
    
    // indicate to nurse that client is ready for vaccination
    fprintf(stderr, "%s: client %ld is ready for the shot from nurse %ld\n", curr_time_s(), id, stationNum);
    sem_post(&client_ready_sems[stationNum]);
    // receive vaccination
    sem_wait(&vacc_complete_sems[stationNum]);
    fprintf(stderr, "%s: client %ld got the shot. Take that, Rona!\n", curr_time_s(), id);
    fprintf(stderr, "%s: client %ld leaves the clinic\n", curr_time_s(), id);
    // leave clinic
    pthread_exit(NULL);
}

int main() {
    srand(time(0));

    sem_init(&num_vials_sem, 0, 1);
    sem_init(&client_assignment_sem, 0, 1);
    sem_init(&nurse_assignment_sem, 0, 1);
    sem_init(&registration_desk_sem, 0, 4);

    // create nurse threads
    for(long int i = 0; i < NUM_NURSES; i++){
        pthread_create(&nurses[i], NULL, nurse, (void *)i);
        // two semaphores for each nurse/station - used for rendezvous
        sem_init(&client_ready_sems[i], 0, 0);
        sem_init(&vacc_complete_sems[i], 0, 0);
    }

    // create client threads
    for(long int i = 0; i < NUM_CLIENTS; i++){
        pthread_create(&clients[i], NULL, client, (void *)i);
        // random 0-1 second delay between creation of each client
        walk(0, 1);
    }

    pthread_exit(0);
}
