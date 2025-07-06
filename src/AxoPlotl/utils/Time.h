#ifndef TIME_H
#define TIME_H

#include "Typedefs.h"
#include <array>

namespace AxoPlotl
{

namespace Time
{
extern float DELTA_TIME;
extern float FRAMES_PER_SECOND;
extern float SECONDS_SINCE_LAST_FRAME;
extern float TIME_OF_LAST_FRAME;

// Updates Time variables
void update();

}

}

#endif // TIME_H
