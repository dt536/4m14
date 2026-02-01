#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

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

int main() {
    Stack stack;
    stack.push("hello", "world", 42);
    stack.push("cambridge", "engineering", 100);
    StackItem item1 = stack.pop();
    std::cout << "Popped item1: " << item1.str1 << ", " << item1.str2 << ", " << item1.value << std::endl;
    StackItem item2 = stack.pop();
    std::cout << "Popped item2: " << item2.str1 << ", " << item2.str2 << ", " << item2.value << std::endl;
    return 0;
}
