#pragma once
#include <pebble.h>

// Geometry, recomputed on bounds change only.
extern int outer_r;
extern int hour_len;
extern int min_len;
extern GPoint center;

// Dot position cache. Angles depend on which groups are enabled (disabled
// groups compact the ring), so this is rebuilt on bounds change AND on any
// settings change, but never from the draw pass — canvas_update_proc reads
// these directly instead of calling trig functions every redraw.
extern GPoint pos_bt;
extern GPoint pos_battery[5];
extern GPoint pos_alarm, pos_notif, pos_event, pos_hr, pos_activity;
extern GPoint pos_step[10];
extern GPoint pos_month[4];
extern GPoint pos_date[5];
extern GPoint pos_weekday;
extern GPoint pos_tick_a[12], pos_tick_b[12]; // hour tick mark endpoints

GPoint point_on_circle(GPoint c, int32_t angle, int radius);
int32_t deg_to_trig(int degrees);

// Recomputes geometry + center from new bounds, then rebuilds the full dot
// position cache.
void layout_compute_geometry(GRect bounds);

// Rebuilds just the dot position cache from the current center/outer_r and
// s_settings visibility toggles. Call after a settings change that could
// affect which groups are shown, without waiting for a bounds change.
void layout_rebuild_positions(void);
