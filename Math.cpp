#include "pch.h"
#include "Math.h"

/// <summary>
/// Get Float Average
/// </summary>
/// <param name="arr"> : Float Array</param>
/// <param name="count"> : Count of Float</param>
/// <returns></returns>
float favgf(const float* arr, const unsigned int count) {
	float sum = 0.f;
	for (unsigned long i = 0; i < count; ++i) {
		sum += arr[i];
	}
	if (sum == 0.f or count == 0) {
		return 0.f;
	}
	else {
		return sum / count;
	}
}

float favgf(std::vector<float>& arr) {
	return 0.f;
}