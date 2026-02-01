#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <random>

struct StackItem {
    std::string str1;
    std::string str2;
    int value;

    StackItem(const std::string& f, const std::string& s, int v)
        : str1(f), str2(s), value(v) {}
};

class Stack {
private:
    std::vector<StackItem> items;

public:
    // Add an item to the stack
    void push(const std::string& str1,
              const std::string& str2,
              int value) {
        items.emplace_back(str1, str2, value);
    }

    // Remove and return the top item
    StackItem pop() {
        if (items.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        StackItem top = items.back();
        items.pop_back();
        return top;
    }
};

void populate_stack(int num, Stack& stack, std::mt19937& mt) {
    static const std::vector<std::string> choices = {
        "dt536", "mb252", "ry297"
    };

    std::uniform_int_distribution<int> lenDist(4, 12);
    std::uniform_int_distribution<int> charDist(0, 25);
    std::uniform_int_distribution<int> valDist(0, 255);
    std::uniform_int_distribution<std::size_t> choiceDist(0, 2);

    for (int i = 0; i < num; ++i) {
        int strLength = lenDist(mt);

        std::string str1;
        str1.reserve(strLength);
        for (int j = 0; j < strLength; ++j) {
            str1.push_back(static_cast<char>('a' + charDist(mt)));
        }
        std::string str2 = choices[choiceDist(mt)];
        int val = valDist(mt);

        stack.push(str1, str2, val);
    }
}


int main() {
    unsigned seed = 12345;          
    std::mt19937 mt(seed);
    Stack stack;
    stack.push("hello", "world", 42);
    populate_stack(5, stack, mt);
    for (int i = 0; i < 5; ++i) {
        StackItem item = stack.pop();
        std::cout << "Popped item: " << item.str1 << ", " << item.str2 << ", " << item.value << std::endl;
    }
    return 0;
}
