#pragma once

namespace AxoPlotl::Rendering
{

extern unsigned int redraw_frames_;

inline void triggerRedraw(unsigned int n_frames = 10) {
    redraw_frames_ = n_frames;
}

}
