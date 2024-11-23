#pragma once
#include <vector>
#include "KeyTeeth.h"

class TeethGenerator
{
private:
    size_t sizeOfGenerated = 1;
public:
    TeethGenerator();
    void SizeOfGenerated(size_t );
    std::vector<KeyTeeth> Generate();
    virtual ~TeethGenerator();

};

