#include "IdGenerator.h"

std::atomic<int> IdGenerator::value;

int IdGenerator::next() {
    int v = value++;
    return v;
}
