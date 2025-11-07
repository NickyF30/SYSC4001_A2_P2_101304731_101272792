//
// Created by nicky on 2025-11-06.
//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

using namespace std;

// shared data
// multiple to be checked
// counter variable to be shared
struct SharedData {
    int multiple;
    int counter;
};

// shared union
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

//semaphore
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
    // Create shared memory
    key_t key = ftok(".", 'x');
    int shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);

    // semaphore
    key_t sem_key = ftok(".", 's');
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);

    // Initialize semaphore to 1
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    // Attach memory
    SharedData* shared = (SharedData*)shmat(shmid, NULL, 0);

    // Initialize variables (semaphore)
    sem_wait(semid);
    shared->multiple = 3;
    shared->counter = 0;
    sem_signal(semid);

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed" << endl;
        return 1;
    }

    if (pid == 0) {
        // Child process
        cout << "Child about to exec, PID: " << getpid() << endl;
        execl("./part5_child", "part5_child", nullptr);

        cerr << "Child execl failed" << endl;
        return 1;
    }
    else {
        // Parent (Process 1)
        int cycle = 0;
        cout << "Parent process PID: " << getpid() << ", Child PID: " << pid << endl;

        sem_wait(semid);
        cout << "Parent: Multiple is set to " << shared->multiple << endl;
        sem_signal(semid);

        // Parent increments
        while (true) {
            sem_wait(semid);  // Enter critical section

            if (shared->counter >= 500) {
                sem_signal(semid);  // Exit
                break;
            }

            shared->counter++;
            int current = shared->counter;
            int mult = shared->multiple;

            sem_signal(semid);  // Exit

            // outside critical section
            if (current % mult == 0) {
                cout << "Parent (P1) - Cycle: " << cycle << " - Shared Counter: " << current
                     << " is a multiple of " << mult << endl;
            } else {
                cout << "Parent (P1) - Cycle: " << cycle << endl;
            }

            cycle++;
            sleep(1);
        }

        cout << "\nParent: Counter reached 500, finishing..." << endl;

        // Wait for child
        wait(NULL);

        // Cleanup
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);

        cout << "Parent: Cleaned up shared memory and semaphore, exiting." << endl;
    }
    return 0;
}