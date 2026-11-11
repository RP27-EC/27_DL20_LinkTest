/**
  ******************************************************************************
  * @file           : ave_filter.c\h
  * @brief          : 
  * @note           : 2022-1-21 15:10:59
  ******************************************************************************
  */
	
#ifndef __LOW_PASS_FILTER_H
#define __LOW_PASS_FILTER_H

#include "stm32f4xx_hal.h"

float low_pass_filter(float input, float prevOutput, float alpha) ;
#endif
