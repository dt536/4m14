//Conflict between cout lock and stack lock fixed by removing cout lock in thread_reverse function.
//cout could be unlocked but stack lock still held, causing misprints.

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <random>
#include <thread>
#include <mutex>
#include <algorithm>

std::mutex cout_mutex;

struct StackItem {
    std::string str1;
    std::string str2;
    int value;

    StackItem(const std::string& f, const std::string& s, int v)
        : str1(f), str2(s), value(v) {}
};

struct stack_stats {
    int sum;
    int min;
    int max;
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
            std::size_t idx = min_indices[dist(rng)];
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
            std::size_t idx = max_indices[dist(rng)];
            items.erase(items.begin() + idx);
        }
        if (!items.empty()){
            items.pop_back();
        }
        return 1;
    }

    stack_stats reverse() {
        std::lock_guard<std::mutex> lock(m_);
        if (items.empty()) {
            return {0, 256, -1};
        }
        int sum = 0;
        int min = 256;
        int max = -1;
        for (const auto& it: items){
            if (it.value < min){
                min = it.value;
            }
            if (it.value > max){
                max = it.value;
            }
            sum += it.value;
        }
        std::reverse(items.begin(), items.end());
        return {sum, min, max}; 
    }   

    std::vector<StackItem> print_items() const {
        std::lock_guard<std::mutex> lock(m_);
        if (items.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return items;

    }

    std::string largest_val(std::mt19937& rng) const {
        std::lock_guard<std::mutex> lock(m_);
        if (items.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        int dt536_sum = 0;
        int mb252_sum = 0;    
        int ry297_sum = 0;
        for (const auto& it : items) {
            if (it.str2 == "dt536") {
                dt536_sum += it.value;
            } else if (it.str2 == "mb252") {
                mb252_sum += it.value;
            } else if (it.str2 == "ry297") {
                ry297_sum += it.value;
            }
        }
        int maxSum = std::max({dt536_sum, mb252_sum, ry297_sum});

        // Collect all CRSIDs with that sum
        std::vector<std::string> candidates;
        if (dt536_sum == maxSum) candidates.push_back("dt536");
        if (mb252_sum == maxSum) candidates.push_back("mb252");
        if (ry297_sum == maxSum) candidates.push_back("ry297");

        // Randomly select one of the tied CRSIDs
        std::uniform_int_distribution<std::size_t> pick(0, candidates.size() - 1);
        return candidates[pick(rng)];
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
    int removed = stack.remove_min_max_top(mt); // removes min, max, then top
    if (removed == 0) break;  
    {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "Stack size: " << stack.size() << std::endl;
    }   
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 0.05s        
    }   
}

void thread_reverse(Stack& stack){
    while(true){
        stack_stats stats = stack.reverse(); 
        if (stats.sum == 0 && stats.min == 256 && stats.max == -1) break;
        {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Reversed stack. Sum: " << stats.sum 
                  << ", Min: " << stats.min 
                  << ", Max: " << stats.max << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 0.05s 
    }
}

void thread_print(Stack& stack){
    while(true){
        try {
            std::vector<StackItem> current_stack = stack.print_items();
            for (const auto& item : current_stack) {
                {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout <<  "Item: " << item.str1 << " " << item.str2 << " " << item.value << std::endl;
                }
            }
        } catch (const std::out_of_range& e) {
            break;
    }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); //longer delay
    }
}

void thread_crs_id(Stack& stack){
    unsigned seed = 123;
    std::mt19937 mt(seed);
    while(true){
        {
        std::lock_guard<std::mutex> lock(cout_mutex);
        try{
            std::string crs_id = stack.largest_val(mt);
            std::cout << "Largest CRS ID: " << crs_id << std::endl;
        } catch (const std::out_of_range& e){
            break;
        }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 0.5s delay
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
    //thread 1
    std::thread t1(thread_reverse, std::ref(stack));
    //thread 2
    std::thread t2(thread_print, std::ref(stack));
    //thread 3
    std::thread t3(thread_crs_id, std::ref(stack));
    t3.join();
    t4.join();
    t1.join();
    t2.join();
    return 0;
}
