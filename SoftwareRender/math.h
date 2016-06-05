#pragma once
#include <algorithm>

using namespace std;

static float Clamp(float n, float lower, float upper) {
	if (n > upper)
		return upper;
	else if (n < lower)
		return lower;
	return n;
}

static float Interpolate(float min, float max, float gradient)
{
	return min + (max - min) * Clamp(gradient, 0 , 1);
}