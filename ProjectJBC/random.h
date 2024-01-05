#pragma once
#include <stdlib.h>
#include <time.h>

void InitRandom(unsigned int seed_value = time(NULL)) {
	srand(seed_value);
}
