// Lab 4, Task 1: Playlist manager using a doubly linked list
// Each node is one song. We keep head, tail and a "current" pointer
// that acts like the song the music player is on right now.

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <cstdlib>
using namespace std;

struct Song {
    int id;
    string name;
    int minutes;
    int seconds;
    Song* next;
    Song* prev;

    Song(int i, const string& n, int m, int s) {
        id = i;
        name = n;
        minutes = m;
        seconds = s;
        next = nullptr;
        prev = nullptr;
    }
};

class Playlist {
private:
    Song* head;
    Song* tail;
    Song* current;   // song that is playing now, nullptr if none yet

    // print one song on a single line
    void printSong(const Song* s) const {
        cout << "ID: " << s->id
             << " | Name: " << s->name
             << " | Duration: " << s->minutes << ":"
             << setfill('0') << setw(2) << s->seconds << setfill(' ')
             << endl;
    }

public:
    Playlist() {
        head = nullptr;
        tail = nullptr;
        current = nullptr;
    }

    // free every node when the playlist goes away
    ~Playlist() {
        Song* temp = head;
        while (temp != nullptr) {
            Song* nextSong = temp->next;
            delete temp;
            temp = nextSong;
        }
    }

    // returns the node with this id, or nullptr if not found
    Song* findById(int id) const {
        Song* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    // Add Song: insert at the end
    bool addSong(int id, const string& name, int minutes, int seconds) {
        if (findById(id) != nullptr) {
            return false;   // id already used
        }
        Song* node = new Song(id, name, minutes, seconds);
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        return true;
    }

    // Delete Song: remove by id
    bool deleteSong(int id) {
        Song* node = findById(id);
        if (node == nullptr) return false;

        // if we are deleting the playing song, move current to a neighbour
        if (current == node) {
            if (node->next != nullptr) current = node->next;
            else current = node->prev;
        }

        // connect the neighbours to each other
        if (node->prev != nullptr) node->prev->next = node->next;
        else head = node->next;   // node was the first one

        if (node->next != nullptr) node->next->prev = node->prev;
        else tail = node->prev;   // node was the last one

        delete node;
        return true;
    }

    // Display Playlist Forward
    void displayForward() const {
        if (head == nullptr) {
            cout << "Playlist is empty." << endl;
            return;
        }
        cout << "Playlist (first to last):" << endl;
        for (Song* temp = head; temp != nullptr; temp = temp->next) {
            printSong(temp);
        }
    }

    // Display Playlist Backward
    void displayBackward() const {
        if (tail == nullptr) {
            cout << "Playlist is empty." << endl;
            return;
        }
        cout << "Playlist (last to first):" << endl;
        for (Song* temp = tail; temp != nullptr; temp = temp->prev) {
            printSong(temp);
        }
    }

    // Search Song by id and show its details
    void searchSong(int id) const {
        Song* node = findById(id);
        if (node == nullptr) {
            cout << "No song found with ID " << id << "." << endl;
            return;
        }
        cout << "Song found:" << endl;
        printSong(node);
    }

    // Play Next Song
    void playNext() {
        if (head == nullptr) {
            cout << "Playlist is empty." << endl;
            return;
        }
        if (current == nullptr) {
            current = head;   // nothing was playing, start from the top
        } else if (current->next == nullptr) {
            cout << "This is already the last song." << endl;
            return;
        } else {
            current = current->next;
        }
        cout << "Now playing: ";
        printSong(current);
    }

    // Play Previous Song
    void playPrevious() {
        if (head == nullptr) {
            cout << "Playlist is empty." << endl;
            return;
        }
        if (current == nullptr) {
            current = tail;   // nothing was playing, start from the end
        } else if (current->prev == nullptr) {
            cout << "This is already the first song." << endl;
            return;
        } else {
            current = current->prev;
        }
        cout << "Now playing: ";
        printSong(current);
    }

    // Reverse Playlist in place by swapping next and prev of every node
    void reverse() {
        if (head == nullptr || head == tail) {
            cout << "Nothing to reverse." << endl;
            return;
        }
        Song* temp = head;
        while (temp != nullptr) {
            Song* oldNext = temp->next;
            temp->next = temp->prev;
            temp->prev = oldNext;
            temp = oldNext;   // move on using the old next pointer
        }
        // head and tail trade places
        Song* swapHelper = head;
        head = tail;
        tail = swapHelper;
        cout << "Playlist reversed." << endl;
    }
};

// ---------- small input helpers ----------

int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) exit(0);   // input ended
        try {
            size_t used = 0;
            int value = stoi(line, &used);
            if (used == line.size()) return value;
        } catch (...) {
        }
        cout << "Please enter a whole number." << endl;
    }
}

string readName() {
    while (true) {
        cout << "Song name: ";
        string line;
        if (!getline(cin, line)) exit(0);
        if (!line.empty()) return line;
        cout << "Name cannot be empty." << endl;
    }
}

void readDuration(int& minutes, int& seconds) {
    while (true) {
        cout << "Duration (minutes:seconds, for example 3:45): ";
        string line;
        if (!getline(cin, line)) exit(0);
        int m, s;
        if (sscanf(line.c_str(), "%d:%d", &m, &s) == 2 &&
            m >= 0 && s >= 0 && s < 60) {
            minutes = m;
            seconds = s;
            return;
        }
        cout << "Invalid duration. Seconds must be from 0 to 59." << endl;
    }
}

int main() {
    Playlist playlist;
    int choice;

    do {
        cout << "\n===== Playlist Manager =====" << endl;
        cout << "1) Add Song" << endl;
        cout << "2) Delete Song" << endl;
        cout << "3) Display Playlist Forward" << endl;
        cout << "4) Display Playlist Backward" << endl;
        cout << "5) Search Song" << endl;
        cout << "6) Play Next Song" << endl;
        cout << "7) Play Previous Song" << endl;
        cout << "8) Reverse Playlist" << endl;
        cout << "0) Exit" << endl;
        choice = readInt("Enter your choice: ");

        if (choice == 1) {
            int id = readInt("Song ID: ");
            string name = readName();
            int minutes, seconds;
            readDuration(minutes, seconds);
            if (playlist.addSong(id, name, minutes, seconds))
                cout << "Song added." << endl;
            else
                cout << "A song with this ID already exists." << endl;
        } else if (choice == 2) {
            int id = readInt("Enter ID of song to delete: ");
            if (playlist.deleteSong(id))
                cout << "Song deleted." << endl;
            else
                cout << "No song found with that ID." << endl;
        } else if (choice == 3) {
            playlist.displayForward();
        } else if (choice == 4) {
            playlist.displayBackward();
        } else if (choice == 5) {
            int id = readInt("Enter ID to search: ");
            playlist.searchSong(id);
        } else if (choice == 6) {
            playlist.playNext();
        } else if (choice == 7) {
            playlist.playPrevious();
        } else if (choice == 8) {
            playlist.reverse();
        } else if (choice != 0) {
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);

    cout << "Goodbye!" << endl;
    return 0;
}
