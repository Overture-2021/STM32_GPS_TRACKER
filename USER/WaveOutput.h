#ifndef __WaveOutput_H
#define __WaveOutput_H

#include "sys.h"
#include "math.h"

#define PI  3.1415926
#define Vref 3.2          
#define Um  (Vref / 2)    
#define N   256           

#define Sine_WaveOutput_Enable 

void SineWave_Init(u16 Wave_Fre);


#endif

