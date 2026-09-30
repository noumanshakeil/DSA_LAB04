// Lab 4, Task 3: Binary arithmetic using a doubly linked list
// Every bit is a node. The head is the most significant bit (left side)
// and the tail is the least significant bit (right side).
// Numbers are always kept in blocks of 8 bits.

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

struct Bit {
    int value;   // 0 or 1
    Bit* next;
    Bit* prev;

    Bit(int v) {
        value = v;
        next = nullptr;
        prev = nullptr;
    }
};

class BinaryNumber {
private:
    Bit* head;   // most significant bit
    Bit* tail;   // least significant bit
    int size;    // how many bits are stored

    void clear() {
        Bit* temp = head;
        while (temp != nullptr) {
            Bit* nextBit = temp->next;
            delete temp;
            temp = nextBit;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

public:
    BinaryNumber() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    // copy constructor, so copies get their own nodes
    BinaryNumber(const BinaryNumber& other) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        for (Bit* p = other.head; p != nullptr; p = p->next) {
            addAtEnd(p->value);
        }
    }

    // assignment, done by taking a copy and swapping with it
    BinaryNumber& operator=(BinaryNumber other) {
        swap(head, other.head);
        swap(tail, other.tail);
        swap(size, other.size);
        return *this;
    }

    ~BinaryNumber() {
        clear();
    }

    bool isEmpty() const {
        return size == 0;
    }

    void addAtEnd(int v) {
        Bit* node = new Bit(v);
        if (tail == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        size++;
    }

    void addAtFront(int v) {
        Bit* node = new Bit(v);
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            node->next = head;
            head->prev = node;
            head = node;
        }
        size++;
    }

    // add zeros on the left until the length is a multiple of 8
    void padToBytes() {
        while (size == 0 || size % 8 != 0) {
            addAtFront(0);
        }
    }

    // Store Binary Number: takes a string of 0s and 1s (spaces are ignored).
    // Returns false if the text is not a valid binary number.
    bool store(const string& text) {
        string bits = "";
        for (size_t i = 0; i < text.size(); i++) {
            char c = text[i];
            if (c == ' ') continue;
            if (c != '0' && c != '1') return false;
            bits += c;
        }
        if (bits.empty()) return false;

        clear();
        for (size_t i = 0; i < bits.size(); i++) {
            addAtEnd(bits[i] - '0');
        }
        padToBytes();   // makes it 8, 16, 24 ... bits long
        return true;
    }

    // print the bits from left to right, 8 at a time
    void show() const {
        int count = 0;
        for (Bit* p = head; p != nullptr; p = p->next) {
            cout << p->value;
            count++;
            if (count % 8 == 0 && p->next != nullptr) cout << " ";
        }
    }

    // 1's Complement: flip every bit
    void onesComplement() {
        for (Bit* p = head; p != nullptr; p = p->next) {
            p->value = 1 - p->value;
        }
    }

    // 2's Complement: flip every bit, then add 1 using binary addition.
    // The width stays the same, so a carry out of the top is dropped.
    void twosComplement() {
        onesComplement();

        BinaryNumber one;   // same length as this number, value 1
        for (int i = 1; i < size; i++) one.addAtEnd(0);
        one.addAtEnd(1);

        *this = add(*this, one, false);
    }

    // shift left by one place (multiply by 2): add a 0 at the right end
    void shiftLeft() {
        addAtEnd(0);
    }

    // Binary Addition: go from the least significant bit to the most
    // significant bit, carrying as we go. If keepOverflow is true and a
    // carry is left at the end, it becomes a new bit on the left.
    static BinaryNumber add(const BinaryNumber& a, const BinaryNumber& b,
                            bool keepOverflow = true) {
        BinaryNumber result;
        Bit* pa = a.tail;
        Bit* pb = b.tail;
        int carry = 0;

        while (pa != nullptr || pb != nullptr) {
            int sum = carry;
            if (pa != nullptr) {
                sum += pa->value;
                pa = pa->prev;
            }
            if (pb != nullptr) {
                sum += pb->value;
                pb = pb->prev;
            }
            result.addAtFront(sum % 2);
            carry = sum / 2;
        }

        if (carry == 1 && keepOverflow) {
            result.addAtFront(1);
        }
        result.padToBytes();
        return result;
    }

    // Binary Multiplication using repeated addition and shifting.
    // For every 1 bit in b (starting from the right), add the current
    // shifted copy of a to the result. Then shift a left by one place.
    static BinaryNumber multiply(const BinaryNumber& a, const BinaryNumber& b) {
        BinaryNumber result;
        result.store("0");   // starts as 00000000

        BinaryNumber shifted = a;
        for (Bit* p = b.tail; p != nullptr; p = p->prev) {
            if (p->value == 1) {
                result = add(result, shifted);
            }
            shifted.shiftLeft();
        }
        return result;
    }

    // Conversion to Decimal: doubling and adding each bit from the left.
    // Digits are kept in a vector so even very long numbers work.
    string toDecimal() const {
        vector<int> digits(1, 0);   // least significant digit first
        for (Bit* p = head; p != nullptr; p = p->next) {
            int carry = p->value;
            for (size_t i = 0; i < digits.size(); i++) {
                int v = digits[i] * 2 + carry;
                digits[i] = v % 10;
                carry = v / 10;
            }
            if (carry > 0) digits.push_back(carry);
        }
        string text = "";
        for (int i = (int)digits.size() - 1; i >= 0; i--) {
            text += (char)('0' + digits[i]);
        }
        return text;
    }
};

// ---------- small input helpers ----------

int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) exit(0);
        try {
            size_t used = 0;
            int value = stoi(line, &used);
            if (used == line.size()) return value;
        } catch (...) {
        }
        cout << "Please enter a whole number." << endl;
    }
}

void readBinary(BinaryNumber& target, const string& label) {
    while (true) {
        cout << "Enter bits for " << label << " (only 0 and 1): ";
        string line;
        if (!getline(cin, line)) exit(0);
        if (target.store(line)) return;
        cout << "That is not a valid binary number." << endl;
    }
}

// asks which of the two stored numbers to use, returns nullptr if it is empty
BinaryNumber* pickNumber(BinaryNumber& A, BinaryNumber& B) {
    int which = readInt("Which number? 1 for A, 2 for B: ");
    BinaryNumber* chosen = nullptr;
    if (which == 1) chosen = &A;
    else if (which == 2) chosen = &B;
    else {
        cout << "Please choose 1 or 2." << endl;
        return nullptr;
    }
    if (chosen->isEmpty()) {
        cout << "That number has not been stored yet." << endl;
        return nullptr;
    }
    return chosen;
}

int main() {
    BinaryNumber A, B;
    int choice;

    do {
        cout << "\n===== Binary Arithmetic with DLL =====" << endl;
        cout << "1) Store number A" << endl;
        cout << "2) Store number B" << endl;
        cout << "3) Show stored numbers" << endl;
        cout << "4) 1's Complement" << endl;
        cout << "5) 2's Complement" << endl;
        cout << "6) Binary Addition (A + B)" << endl;
        cout << "7) Binary Multiplication (A * B)" << endl;
        cout << "8) Conversion to Decimal" << endl;
        cout << "0) Exit" << endl;
        choice = readInt("Enter your choice: ");

        if (choice == 1) {
            readBinary(A, "A");
            cout << "A stored as: ";
            A.show();
            cout << endl;
        } else if (choice == 2) {
            readBinary(B, "B");
            cout << "B stored as: ";
            B.show();
            cout << endl;
        } else if (choice == 3) {
            cout << "A = ";
            if (A.isEmpty()) cout << "(not stored)"; else A.show();
            cout << endl;
            cout << "B = ";
            if (B.isEmpty()) cout << "(not stored)"; else B.show();
            cout << endl;
        } else if (choice == 4) {
            BinaryNumber* chosen = pickNumber(A, B);
            if (chosen != nullptr) {
                BinaryNumber result = *chosen;   // work on a copy
                result.onesComplement();
                cout << "Original     : ";
                chosen->show();
                cout << endl << "1's complement: ";
                result.show();
                cout << endl;
            }
        } else if (choice == 5) {
            BinaryNumber* chosen = pickNumber(A, B);
            if (chosen != nullptr) {
                BinaryNumber result = *chosen;   // work on a copy
                result.twosComplement();
                cout << "Original     : ";
                chosen->show();
                cout << endl << "2's complement: ";
                result.show();
                cout << endl;
            }
        } else if (choice == 6) {
            if (A.isEmpty() || B.isEmpty()) {
                cout << "Store both A and B first." << endl;
            } else {
                BinaryNumber sum = BinaryNumber::add(A, B);
                cout << "A   = "; A.show(); cout << "  (" << A.toDecimal() << ")" << endl;
                cout << "B   = "; B.show(); cout << "  (" << B.toDecimal() << ")" << endl;
                cout << "A+B = "; sum.show(); cout << "  (" << sum.toDecimal() << ")" << endl;
            }
        } else if (choice == 7) {
            if (A.isEmpty() || B.isEmpty()) {
                cout << "Store both A and B first." << endl;
            } else {
                BinaryNumber product = BinaryNumber::multiply(A, B);
                cout << "A   = "; A.show(); cout << "  (" << A.toDecimal() << ")" << endl;
                cout << "B   = "; B.show(); cout << "  (" << B.toDecimal() << ")" << endl;
                cout << "A*B = "; product.show(); cout << "  (" << product.toDecimal() << ")" << endl;
            }
        } else if (choice == 8) {
            BinaryNumber* chosen = pickNumber(A, B);
            if (chosen != nullptr) {
                chosen->show();
                cout << " = " << chosen->toDecimal() << " in decimal" << endl;
            }
        } else if (choice != 0) {
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);

    cout << "Goodbye!" << endl;
    return 0;
}
