#pragma once
#include <pebble.h>

GColor get_color(uint8_t idx);
GColor theme_bg(void);
GColor theme_dim(void);
GColor theme_fg(void);

// Recomputes whether the light theme is active for the given local hour:
// always-on/off in Dark/Light mode, or within [light_start_hour,
// light_end_hour) in Auto mode. Cheap int comparison — call once per minute
// tick or settings change, never from the draw pass.
void theme_update(int hour);
