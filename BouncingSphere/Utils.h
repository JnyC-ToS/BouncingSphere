#ifndef __UTILS_H__
#define __UTILS_H__

#define EPSILON 1.e-6f
#define CYLINDER_CAPS_NONE 0
#define CYLINDER_CAPS_FLAT 1
#define CYLINDER_CAPS_ROUNDED 2

template <typename T> int sgn(T val);
bool approxZero(float val);
float min(float a, float b);

#endif
