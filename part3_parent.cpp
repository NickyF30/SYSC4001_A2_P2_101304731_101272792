//
// Created by nicky on 2025-11-06.
//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed" << endl;
        return 1;
    }

    if (pid == 0) {
        //child
        cout << "Child about to exec, PID: " << getpid() << endl;
        execl("./part3_child", "part3_child", nullptr);
    }
    else {
        // Parent
        int counter = 0;
        int cycle = 0;
        cout << "Parent process PID: " << getpid() << ", Child PID: " << pid << endl;
        cout << "Parent waiting for child to complete..." << endl;

        // wait for child process to finish
        int status;
        pid_t child_pid = wait(&status);

        if (child_pid > 0) {
            cout << "\nChild process (PID: " << child_pid << ") has finished." << endl;
        }

        cout << "\nParent now terminating as well." << endl;
    }
    return 0;
}