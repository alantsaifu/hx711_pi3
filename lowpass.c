
#include "hx711.h"

long offset=0;

void lowPassTest(const float alpha[],int n) {
  int i;
  static int initSize = 0;  


  const int CNT_TO_STABLE = 2;
  const int STABLE_THRESH = 3000; // +/-

  static long* reading; 
  static long* readingLastStable;
  static int* stableCount;

  long value =  getCleanSample(2, 5);
   
  if (initSize!=n) {
    if (0==reading) {
      reading = (long*) malloc(n * sizeof(long));
      readingLastStable = (long*) malloc(n * sizeof(long));
      stableCount = (int*) malloc(n * sizeof(int));

      for (i = 0; i<n; i++) {
        reading[i]=value;
        readingLastStable[i]=value;
        stableCount[i]=0;
      }

    } else {
      reading = (long*) realloc(reading, n * sizeof(long));
      readingLastStable = (long*) realloc(readingLastStable, n * sizeof(long));
      stableCount = (int*) realloc(stableCount, n * sizeof(int));
    }
    initSize = n;
  }

  for (i = 0; i<n; i++) {
    long newValue = reading[i] * (1-alpha[i]) + value * alpha[i];    
    if (abs(reading[i]-value)<STABLE_THRESH) {
      if (stableCount[i]<255) {
        stableCount[i]++;      
      }
    } else {
      stableCount[i] = 0;
    }

    int isStable = (((int)stableCount[i])>=CNT_TO_STABLE);
    
    reading[i] = newValue;

    if (isStable && abs(readingLastStable[i]-reading[i])>STABLE_THRESH) {
      //TODO
    }

    //printf("%ld",(long)reading[i] - offset);
    //printf(" ");
    //printf(isStable?"*":" ");
      
    if (i<n-1) { 
        //printf("\t"); 
    } else {
        if(isStable){
            printf("%ld",(long)reading[i] - offset);
            printf("\n");
        }
    }
  }
}

int main(int argc, char **argv) {

  initHX711();

  if (argc == 2) {
   offset = atol(argv[1]);
  }
  
  while (1==1) {
     lowPassTest((const float[]){1, 0.50, 0.20 , 0.1},4);
  } 

}