/*
 * CS 232 Project 5 - Vaccination Clinic
 * Ben DeWeerd
 * 3.06.2022
 */

// gcc vacc_clinic.c -o vacc_clinic -lpthread

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

typedef struct{
    int nurseId;
} Station;

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

// lower and upper are in seconds.
void walk(int lower, int upper) {
    // TODO: fill in code here.  Use usleep() and get_rand_in_range() from
    // above.
}

// arg is the nurses station number
void *nurse(void *arg) {
    long int id = (long int)arg;
    bool has_vial = false;
    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id);

    do{
        //walk between 1 and 3 seconds to get vial of vaccine
        //if no more vials, leave clinic
        has_vial = false;
        sem_wait(&num_vials_sem);

        if(num_vials_left > 0){
            num_vials_left--;
            has_vial = true;
            fprintf(stderr, "%s: nurse %ld got a new vial, num_vials_left = %d\n", curr_time_s(), id, num_vials_left);
        } else {
            fprintf(stderr, "%s: nurse %ld sees there are no more vials of vaccine\n", curr_time_s(), id);
        }
        
        sem_post(&num_vials_sem);

        if(has_vial){
            for(int shots = 5; shots >= 0; shots--){
                sem_wait(&nurse_assignment_sem);
                fprintf(stderr, "%s: nurse %ld tells the waiting queue they are available\n", curr_time_s(), id);
                assignmentQueue[assignIn].nurseId = id;
                assignIn = (assignIn + 1) % NUM_STATIONS;
                count++;
                sem_post(&nurse_assignment_sem);

                fprintf(stderr, "%s: nurse %ld waiting for a client to arrive\n", curr_time_s(), id);

                sem_wait(&client_ready_sems[id]);
                fprintf(stderr, "%s: nurse %ld sees client is ready. Giving shot now.\n", curr_time_s(), id);
                // giving shot...
                fprintf(stderr, "%s: nurse %ld gave client the shot. %i shots left in vial.\n", curr_time_s(), id, shots);
                sem_post(&vacc_complete_sems[id]);
            }
        }
         
    } while(has_vial);

    //walk back, between 1 and 3 seconds
    //repeat 6 times:
    //  indicate to queue of clients waiting for station that am ready for next client
    //  wait for client to indicate they're ready to be vaccinated
    //  give client vaccination (5 seconds)
    //loop

    fprintf(stderr, "%s: nurse %ld is done\n", curr_time_s(), id);
    pthread_exit(NULL);
}

void *client(void *arg) {
    long int id = (long int)arg;
    long int stationNum;
    int reg_desk_count;


    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n",
            curr_time_s(), id);

    //walk between 3 and 10 seconds to registration desk
    //wait for opening at registration desk
    //take between 3 and 10 seconds to register
    //walk between 3 and 10 seconds to get to station-assignment queue
    //wait for station assignment

    //wait for operning at registration desk (4 slots)
    sem_wait(&registration_desk_sem);
    sem_getvalue(&registration_desk_sem, &reg_desk_count);
    fprintf(stderr, "%s: client %ld: registration_desk_sem: %i\n", curr_time_s(), id, reg_desk_count);
    //register...
    fprintf(stderr, "%s: client %ld is registering\n", curr_time_s(), id);
    sem_post(&registration_desk_sem);

    sem_wait(&client_assignment_sem);
    while(count == 0){
        ;
    }
    stationNum = assignmentQueue[assignOut].nurseId;
    assignOut = (assignOut + 1) % NUM_STATIONS;
    count--; 
    //TODO: consume item somehow?
    sem_post(&client_assignment_sem);

    // assignmentQueue[stationNum].completed = false;
    fprintf(stderr, "%s: client %ld got assigned to station %ld: walking there now\n", curr_time_s(), id, stationNum);
    fprintf(stderr, "%s: client %ld is at station %ld\n", curr_time_s(), id, stationNum);
    
    //indicate to nurse that client is ready for vaccination
    fprintf(stderr, "%s: client %ld is ready for the shot from nurse %ld\n", curr_time_s(), id, stationNum);
    sem_post(&client_ready_sems[stationNum]);
    // waiting for vaccination to complete....
    sem_wait(&vacc_complete_sems[stationNum]);
    fprintf(stderr, "%s: client %ld got the shot! It hurt, but it is a sacrifice they're willing to make!\n", curr_time_s(), id);

    //walk between 1 and 2 seconds to get to assigned nurse's station

    fprintf(stderr, "%s: client %ld leaves the clinic!\n", curr_time_s(), id);
    pthread_exit(NULL);
}

int main() {
    srand(time(0));

    //TODO:
    //  create a bunch of semaphores (2 for each station)
    //  initialize them in a loop using sem_init

    //create semaphores necessary for simulation
    sem_init(&num_vials_sem, 0, 1);
    sem_init(&client_assignment_sem, 0, 1);
    sem_init(&nurse_assignment_sem, 0, 1);
    sem_init(&registration_desk_sem, 0, 4);

    // create nurse threads
    //TODO: make this produce all of the nurses
    //TODO: assign nurses to stations here
    
    for(long int i = 0; i < NUM_NURSES; i++){
        pthread_create(&nurses[i], NULL, nurse, (void *)i);
        //two semaphores for each nurse/station - binary semaphore for a rendezvous
        sem_init(&client_ready_sems[i], 0, 0);
        sem_init(&vacc_complete_sems[i], 0, 0);
    }

    for(long int i = 0; i < NUM_CLIENTS; i++){
        pthread_create(&clients[i], NULL, client, (void *)i);
    }

    // pthread_join(nurse0, NULL);    
    // pthread_join(client0, NULL);
    //create client threads, with 0-1 second delay between each creation

    pthread_exit(0);
}
