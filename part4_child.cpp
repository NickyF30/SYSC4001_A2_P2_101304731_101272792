//
// Created by nicky on 2025-11-06.
//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
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
    // Access the shared memory created by parent
    key_t key = ftok(".", 'x');
    int shmid = shmget(key, sizeof(SharedData), 0666);

    if (shmid < 0) {
        cerr << "Child: Failed to access shared memory" << endl;
        return 1;
    }

    //  share memory
    SharedData* shared = (SharedData*)shmat(shmid, NULL, 0);
    if (shared == (void*)-1) {
        cerr << "Child: Failed to attach shared memory" << endl;
        return 1;
    }

    cout << "Child process PID: " << getpid() << " started" << endl;

    // wait until counter is larger than 100 then child executes too
    cout << "Child: Waiting for counter to reach 100..." << endl;
    while (shared->counter <= 100) {
        sleep(1);
    }

    cout << "Child: Counter reached 100+, starting execution..." << endl;
    cout << "Child: Multiple is " << shared->multiple << endl;

    int cycle = 0;

    // child uses the shared counter and displays multiples
    while (shared->counter < 500) {
        int current = shared->counter;

        if (current % shared->multiple == 0) {
            cout << "Child (P2) - Cycle: " << cycle << " - Shared Counter: " << current
                 << " is a multiple of " << shared->multiple << endl;
        } else {
            cout << "Child (P2) - Cycle: " << cycle << endl;
        }

        cycle++;
        sleep(1);
    }

    cout << "\nChild: Counter reached 500, exiting..." << endl;

    // Detach shared memory
    shmdt(shared);

    return 0;
}