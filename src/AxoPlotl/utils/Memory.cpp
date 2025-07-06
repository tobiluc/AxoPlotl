#include "Memory.h"
#include <mach/task_info.h>
#include <mach/mach.h>

double AxoPlotl::getMemoryUsageMB()
{
    mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count);
    return info.resident_size / (1024.0 * 1024.0);
}
