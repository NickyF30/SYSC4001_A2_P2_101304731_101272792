//
// Created by nicky on 2025-11-06.
//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

int main() {
    int counter = 0;
    int cycle = 0;

    cout << "Child process PID: " << getpid() << " started" << endl;
    cout << "Child will run until counter reaches -500" << endl;

    //tested with this because -500 is too long
    //while (counter > -100){

    while (counter > -500) {
        if (counter % 3 == 0) {
            cout << "Cycle number: " << cycle << " - " << counter << " is a multiple of 3" << endl;
        } else {
            cout << "Cycle number: " << cycle << endl;
        }
        counter--;
        cycle++;
        sleep(1);
    }

    cout << "\nChild reached -500. Exiting" << endl;
    return 0;
}