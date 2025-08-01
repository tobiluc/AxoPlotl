#pragma once

namespace AxoPlotl::Rendering
{

extern unsigned int redraw_frames_;

inline void triggerRedraw(unsigned int n_frames = 2) {
    if (n_frames > redraw_frames_) {
        redraw_frames_ = n_frames;
    }
}

}
