#include "Utils.h"
#include "math.h"
#include "raylib.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

bool approxZero(float val) {
	return fabs(val) < EPSILON;
}

float min(float a, float b) {
	return a < b ? a : b;
}

Quaternion QuaternionConjuguate(Quaternion q){
	return {-q.x,-q.y,-q.z,q.w};
}
