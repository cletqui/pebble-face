#pragma once
#include <pebble.h>

extern int step_lit;         // 0-9, progress within the current goal-multiple
extern int step_milestones;  // 0-10, completed goal-multiples
extern int heart_rate;       // BPM, 0 = no data
extern bool heart_rate_error;
extern bool activity_active;

void health_update_steps(void);
void health_update_hr_activity(void);

// Subscribes to HealthService events; on HR/movement updates, refreshes
// state and marks the watchface dirty via face_window_mark_dirty().
void health_subscribe(void);
