//
// Created by nicky on 2025-11-06.
//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

// shared data
// multiple to be checked
// counter variable to be shared
struct SharedData {
    int multiple;
    int counter;
};

int main() {
    // share memory segment
    key_t key = ftok(".", 'x');
    int shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);

    if (shmid < 0) {
        cerr << "Failed to create shared memory" << endl;
        return 1;
    }

    // attach  memory
    SharedData* shared = (SharedData*)shmat(shmid, NULL, 0);
    if (shared == (void*)-1) {
        cerr << "Failed to attach shared memory" << endl;
        return 1;
    }

    shared->multiple = 3;
    shared->counter = 0;

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed" << endl;
        return 1;
    }

    if (pid == 0) {
        // Child
        cout << "Child about to exec, PID: " << getpid() << endl;
        execl("./part4_child", "part4_child", nullptr);

        cerr << "Child execl failed" << endl;
        return 1;
    }
    else {
        // Parent (process 1)
        int cycle = 0;
        cout << "Parent process PID: " << getpid() << ", Child PID: " << pid << endl;
        cout << "Parent: Multiple is set to " << shared->multiple << endl;

        // Parent increment counter
        while (shared->counter < 500) {
            shared->counter++;  // Increment first

            if (shared->counter % shared->multiple == 0) {
                cout << "Parent (P1) - Cycle: " << cycle << " - Shared Counter: " << shared->counter
                     << " is a multiple of " << shared->multiple << endl;
            } else {
                cout << "Parent (P1) - Cycle: " << cycle << endl;
            }

            cycle++;
            sleep(1);
        }

        cout << "\nParent: Counter reached 500, finishing..." << endl;

        // Wait for child to finish
        wait(NULL);

        // Detach and remove memory
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);

        cout << "Parent: Cleaned up shared memory and exiting." << endl;
    }
    return 0;
}