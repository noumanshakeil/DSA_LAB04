// Lab 4, Task 2: Josephus Problem using a circular linked list
// N people stand in a circle numbered 1 to N. Starting from person 1,
// every k-th person is eliminated until one person is left.

#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

struct Person {
    int id;
    Person* next;

    Person(int i) {
        id = i;
        next = nullptr;
    }
};

// Create Circle: builds people 1..n and links the last one back to the first.
// It returns the LAST person, so last->next is person 1.
Person* createCircle(int n) {
    Person* first = new Person(1);
    Person* last = first;
    for (int i = 2; i <= n; i++) {
        Person* p = new Person(i);
        last->next = p;
        last = p;
    }
    last->next = first;   // close the circle
    return last;
}

int readPositive(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) exit(0);
        try {
            size_t used = 0;
            int value = stoi(line, &used);
            if (used == line.size() && value >= 1) return value;
        } catch (...) {
        }
        cout << "Please enter a whole number of 1 or more." << endl;
    }
}

int main() {
    int n = readPositive("Number of people (N): ");
    int k = readPositive("Step count (k): ");

    // prev is always the person just before curr, we need it to unlink curr
    Person* prev = createCircle(n);
    Person* curr = prev->next;   // person 1, counting starts here
    int remaining = n;

    cout << "\nElimination order: ";
    bool firstPrinted = true;

    // Elimination Process: stop when only one person is left
    while (curr->next != curr) {
        // walking a full lap changes nothing, so only walk the extra steps
        int steps = (k - 1) % remaining;
        for (int i = 0; i < steps; i++) {
            prev = curr;
            curr = curr->next;
        }

        // Display Eliminated Order
        if (!firstPrinted) cout << ", ";
        cout << curr->id;
        firstPrinted = false;

        // remove curr from the circle and start counting from the next person
        prev->next = curr->next;
        delete curr;
        curr = prev->next;
        remaining--;
    }

    if (firstPrinted) cout << "(nobody was eliminated)";
    cout << endl;

    // Display Survivor
    cout << "Survivor: Person " << curr->id << endl;

    delete curr;   // free the last node
}
