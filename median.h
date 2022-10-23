// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#ifndef INC_281P2A_MEDIAN_H
#define INC_281P2A_MEDIAN_H
#include <queue>
#include <vector>
using std::vector;
using std::priority_queue;

// TODO: ALWAYS keep the smaller.size() >= larger.size()
struct MedianContainer{
    priority_queue<uint32_t,vector<uint32_t>> smaller;
    priority_queue<uint32_t,vector<uint32_t>,std::greater<>> larger;
    // functions
    size_t size() const;
    int inspect() const;
    void push(const uint32_t& N);
};



size_t MedianContainer::size() const {
    return smaller.size() + larger.size();
}


int MedianContainer::inspect() const {

    if (size() % 2 == 0){ // two medians, get average
        return (static_cast<int>(smaller.top() + larger.top()) / 2);
    }
    else {
        return static_cast<int>(smaller.top());
    }
}

// always put into smaller first
/// Smaller is of higher priority
void MedianContainer::push(const uint32_t &N) {
    if (size() == 0){
        smaller.push(N);
    }
    else if (smaller.size() == larger.size()){
        if (N > larger.top()){
            auto e = larger.top();
            larger.pop();
            smaller.push(e);
            larger.push(N);
        }
        else{
            smaller.push(N);
        }
    }
    else if (smaller.size() == larger.size() + 1){
        if (N < smaller.top()){
            auto e = smaller.top();
            smaller.pop();
            larger.push(e);
            smaller.push(N);
        }
        else{
            larger.push(N);
        }
    }
    else{
        printf("Wrong algorithm!\n");
        exit(1);
    }
}
#endif //INC_281P2A_MEDIAN_H
