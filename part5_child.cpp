//
// Created by nicky on 2025-11-06.
//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

using namespace std;

// shared data
// multiple to be checked
// counter variable to be shared
struct SharedData
{
    int multiple;
    int counter;
};

// Union
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void sem_wait(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;  // P operation (wait/down)
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

void sem_signal(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;   // V operation (signal/up)
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

int main() {
    // shared memory created by parent
    key_t key = ftok(".", 'x');
    int shmid = shmget(key, sizeof(SharedData), 0666);

    // access the semaphore created
    key_t sem_key = ftok(".", 's');
    int semid = semget(sem_key, 1, 0666);

    // attach  memory
    SharedData* shared = (SharedData*)shmat(shmid, NULL, 0);

    cout << "Child process PID: " << getpid() << " started" << endl;

    // wait until counter is  100
    cout << "Child: Waiting for counter to reach 100..." << endl;
    while (true) {
        sem_wait(semid);  // Enter critical section
        int current = shared -> counter;
        sem_signal(semid);  // Exit

        if (current > 100) {
            break;
        }
        sleep(1);
    }

    cout << "Child: Counter reached 100+, starting execution..." << endl;

    sem_wait(semid);
    cout << "Child: Multiple is " << shared ->multiple << endl;
    sem_signal(semid);

    int cycle = 0;

    // Child reads shared counter displays multiples
    while (true) {
        sem_wait(semid);  // Enter critical section

        if (shared->  counter >= 500) {
            sem_signal(semid);  // Exit
            break;
        }

        int current = shared ->counter;
        int mult = shared -> multiple;

        sem_signal(semid);  // Exit

        // outside critical section
        if (current % mult == 0) {
            cout << "Child (P2) - Cycle: " << cycle << " - Shared Counter: " << current
                 << " is a multiple of " << mult << endl;
        } else {
            cout << "Child (P2) - Cycle: " << cycle << endl;
        }

        cycle++;
        sleep(1);
    }

    cout << "\nChild: Counter reached 500, exiting..." << endl;

    // detach shared memory as parent cleans
    shmdt(shared);

    return 0;
}