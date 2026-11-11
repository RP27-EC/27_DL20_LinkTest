#ifndef _LED_H
#define _LED_H

#include "stm32f4xx_hal.h"
#include "gpio.h"


/* Private macro -------------------------------------------------------------*/
#define  LED_PORT   	 GPIOH
#define  LED_BLUE_PIN    GPIO_PIN_10
#define  LED_GREEN_PIN   GPIO_PIN_11
#define  LED_RED_PIN     GPIO_PIN_12

#define  LED_BLUE_ON        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET)
#define  LED_GREEN_ON      HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET)
#define  LED_RED_ON        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET)

#define  LED_BLUE_OFF     HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET)
#define  LED_GREEN_OFF     HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET)
#define  LED_RED_OFF       HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET)

#define  LED_BLUE_Toggle 	HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10)
#define  LED_GREEN_Toggle 	HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_11)
#define  LED_RED_Toggle 	HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_12)


/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    LED_OFF = 0,
    LED_ON = 1,
	LED_BLINK =2
} led_state_e;

typedef enum
{
    LED_colour_blue = 0,
    LED_colour_green = 1,
	LED_colour_red =2
} led_colour_e;

typedef struct led
{
  led_state_e state;
  led_colour_e colour;
  float 	   blink_fre; //每秒亮多少次
	
} led_t;


/* Exported function --------------------------------------------------------*/
void led_work(led_t *led);
/* Exported variables --------------------------------------------------------*/
extern led_t led;


#endif

