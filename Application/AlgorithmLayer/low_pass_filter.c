

#include "low_pass_filter.h"

float low_pass_filter(float input, float prevOutput, float alpha) 
	{
    return alpha * input + (1.0f - alpha) * prevOutput;
	}
