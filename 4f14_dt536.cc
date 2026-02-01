#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <random>
#include <thread>
#include <mutex>

std::mutex cout_mutex;

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
    mutable std::mutex m_;

public:
    // Add an item to the stack
    void push(const std::string& str1,
              const std::string& str2,
              int value) {
        std::lock_guard<std::mutex> lock(m_);
        items.emplace_back(str1, str2, value);
    }

    // Remove and return the top item
    StackItem pop() {
        std::lock_guard<std::mutex> lock(m_);
        if (items.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        StackItem top = items.back();
        items.pop_back();
        return top;
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(m_);
        return items.size();
    } 

    int remove_min_max_top(std::mt19937& rng) {
    // Remove an item with the minimum value and maximum value
    std::lock_guard<std::mutex> lock(m_);  
    if (items.empty()) {
        return 0;
    }
    if (!items.empty()){
        int min = items[0].value;
        for (const auto& it: items){
            if (it.value < min){
                min = it.value;
            }
        }
        std::vector<std::size_t> min_indices;
        for (std::size_t i = 0; i < items.size(); ++i) {
            if (items[i].value == min) {
                min_indices.push_back(i);
            }
        }
        std::uniform_int_distribution<std::size_t> dist(0, min_indices.size() - 1);
        std::size_t idx = dist(rng);
        items.erase(items.begin() + idx);
    }

    if (!items.empty()){
        int max = items[0].value;
        for (const auto& it: items){
            if (it.value > max){
                max = it.value;
            }
        }
        std::vector<std::size_t> max_indices;
        for (std::size_t i = 0; i < items.size(); ++i) {
            if (items[i].value == max) {
                max_indices.push_back(i);
            }
        }
        std::uniform_int_distribution<std::size_t> dist(0, max_indices.size() - 1);
        std::size_t idx = dist(rng);
        items.erase(items.begin() + idx);
    }
    if (!items.empty()){
        items.pop_back();
    }
    return 1;
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

void thread_remove(Stack& stack){
    unsigned seed = 123;
    std::mt19937 mt(seed);
    while (true) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "Stack size: " << stack.size() << std::endl;
    int removed = stack.remove_min_max_top(mt); // removes min, max, then top
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 0.05s
    if (removed == 0) break;              
}
}


int main() {
    unsigned seed = 123;          
    std::mt19937 mt(seed);
    Stack stack;
    populate_stack(738, stack, mt);
    std::cout << "Initial stack size: " << stack.size() << std::endl;
    //thread 4
    std::thread t4(thread_remove, std::ref(stack));
    t4.join();

    return 0;
}
