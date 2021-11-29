#include "Utils.h"
#include "math.h"
#include "stdlib.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

bool approxZero(float val) {
	return fabs(val) < EPSILON;
}

float min(float a, float b) {
	return a < b ? a : b;
}

float random() {
	return rand() % RAND_MAX / (float) RAND_MAX;
}

float randPos() {
	return random() * 2 - 1;
}
