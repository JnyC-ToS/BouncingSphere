#include "Utils.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

bool approxZero(float val) {
	return fabs(val) < EPSILON;
}

float min(float a, float b) {
	return a < b ? a : b;
}

float modulof(float f, float mod) {
	if (f < mod) {
		if (f >= 0)
			return f;
		return modulof(f + mod, mod);
	}
	return modulof(f - mod, mod);
}
