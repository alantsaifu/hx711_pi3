#include "hx711.h"

/**

	HX711 interface for Raspberry Pi 3
	Based on code by gurov (https://github.com/ggurov/hx711) adapted to work on Pi3

*/


void setHighPri (void)
{
	struct sched_param sched ;

	memset (&sched, 0, sizeof(sched)) ;

	sched.sched_priority = 10 ;
	if (sched_setscheduler (0, SCHED_FIFO, &sched))
	printf ("Warning: Unable to set high priority\n") ;
}


void setup_gpio()
{
	INP_GPIO(DATA_PIN);
	INP_GPIO(CLOCK_PIN);  OUT_GPIO(CLOCK_PIN);
	SCK_OFF;
}

void unpull_pins()
{
   GPIO_PULL = 0;
//   short_wait();
   GPIO_PULLCLK0 = 1 << DATA_PIN;
//   short_wait();
   GPIO_PULL = 0;
   GPIO_PULLCLK0 = 0;
} // unpull_pins

//hack for testing what works on the faster Pi3, see below - should probably be replaced with proper usleep
void delay(int i) {
	static clock_t timer;
	timer = clock();
	while (clock() - timer < i);  
}

void initHX711() {
	setHighPri();
	setup_io();
	setup_gpio();
	reset_converter();
}

/**
 getCleanSample(int numSamples, int spread)

 get a clean sample, by sampling numSamples times, and averaging while filtering out any samples outside the specified filter spread

 will try a maximum of 10 times in a row to get a clean sample, fails with value of 0
*/
long getCleanSample(int numSamples, int spread) {

	int i, j=0, k=0;
	long tmp=0;
	long tmp_avg=0;
	long tmp_avg2;
	long offset=0;
	float filter_low, filter_high;
	float spread_percent = spread / 100.0 /2.0;
	int b;
	long samples[numSamples];


	while (j==0 && k<10) {
		k++;
	
		tmp_avg =0;
	
		// get the dirty samples and average them
		for(i=0;i<numSamples;i++) {
			samples[i] = read_value();
			tmp_avg += samples[i];
		}

		//calculate dirty average
		tmp_avg = tmp_avg / numSamples;

		tmp_avg2 = 0;
	
		j=0;

		filter_low =  (float) tmp_avg * (1.0 - spread_percent);
		filter_high = (float) tmp_avg * (1.0 + spread_percent);

		for(i=0;i<numSamples;i++) {
			if ((samples[i] <= filter_high && samples[i] >= filter_low) || 
			    (samples[i] >= filter_high && samples[i] <= filter_low) ) {
				tmp_avg2 += samples[i];
			    j++;
			}
		}

	}

	return tmp_avg2/j;
}

/**
 speedTest runs 100 samples, and returns 
*/
float speedTest() {
	int i;
	clock_t t1, t2;

	reset_converter();

	t1 = clock();
	for(i=0;i<100;i++) {
		read_value();
	}

	t2 = clock();

	float diff = ((float)(t2 - t1) / CLOCKS_PER_SEC ) *  1000;
	float sps = 1000/(diff/100);

	//printf("%d samples took %f ms, %f sps\n",N_SAMPLES, diff, sps);

	return sps;
}


void uninit() {
  unpull_pins();
  restore_io();
}


void reset_converter(void) {
	SCK_ON;
	usleep(60);
	SCK_OFF;
	usleep(60);
}

// r = 0 - Ch.A, Gain 128
// r = 1 - Ch.B, Gain 64
// r = 2 - Ch.A, Gain 32 <<<**** Different data sheets say different things about this configuration. Do not rely on this just working - test in your setup.
static int gain = 0; //default Ch.a, Gain 128
void set_gain(int r) {
	int i;
	gain = r;

	//wait for data ready
	while( DT_R ); 

	//pull out a reading and configure appropriately for next reading
	for (i=0;i<24+r+1;i++) {
		SCK_ON;
		SCK_OFF;
	}
}


unsigned long read_value() {
	long count; //store the shifted-in data
	int i; 		//iterator

	count = 0;

	//can be replaced with a proper usleep
	const int DELAYLENGTH = 4;

	//white for Data Ready
	while( DT_R ); 
	
	delay(DELAYLENGTH);

	//read in the data
	for(i=0;i<24; i++) {
		SCK_ON;
		delay(DELAYLENGTH);
	    if (DT_R > 0 ) { count++; }
	    SCK_OFF;
		delay(DELAYLENGTH);
	    count = count << 1;
	}

	//set gain for next reading
	for (i=0; i<gain+1; i++) {
		SCK_ON;
		delay(DELAYLENGTH);

		SCK_OFF;
		delay(DELAYLENGTH);		
	}


	//TODO: check correctness for negative values!
	//  count = ~0x1800000 & count;
	//  count = ~0x800000 & count;
	if (count & 0x800000) {
		count |= (long) ~0xffffff;
	}

	return count;
}