#include "engine/api/code.h"
#include "engine/api/maths.h"

s32 mul_div_s32(s32 value, s32 numerator, s32 denominator) {
	s32 a = value / denominator; s32 b = value % denominator;
	return a * numerator + b * numerator / denominator;
}

u64 mul_div_u64(u64 value, u64 numerator, u64 denominator) {
	u64 a = value / denominator; u64 b = value % denominator;
	return a * numerator + b * numerator / denominator;
}
