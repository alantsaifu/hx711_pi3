#include <stdio.h>
#include "gb_common.h"
#include <sched.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//Scale (HX711) connections
#define CLOCK_PIN	20
#define DATA_PIN	21

//GPIO parameters
#define SCK_ON  (GPIO_SET0 = (1 << CLOCK_PIN))
#define SCK_OFF (GPIO_CLR0 = (1 << CLOCK_PIN))
#define DT_R    (GPIO_IN0  & (1 << DATA_PIN))

void 		   initHX711();
float		   speedTest();
long 		   getCleanSample(int numSamples, int spread);
void           reset_converter(void);
unsigned long  read_value();
void           set_gain(int r);
void           setHighPri (void);