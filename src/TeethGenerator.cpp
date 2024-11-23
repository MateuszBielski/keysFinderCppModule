#include "TeethGenerator.h"

using namespace std;

TeethGenerator::TeethGenerator()
{
}

TeethGenerator::~TeethGenerator()
{
}
std::vector<KeyTeeth> TeethGenerator::Generate()
{
	return vector<KeyTeeth>(sizeOfGenerated);
}
void TeethGenerator::SizeOfGenerated(size_t n)
{
	sizeOfGenerated = n;
}
