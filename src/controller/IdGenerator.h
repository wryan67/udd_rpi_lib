#pragma once
#include <atomic>

class IdGenerator {
private:
    static std::atomic<int> value;

public:
    static int next();
};

