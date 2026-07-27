#pragma once
#include <pebble.h>

void face_window_push(void);
void face_window_teardown(void);

// No-op if the canvas doesn't exist yet (safe to call during init, before
// face_window_push()).
void face_window_mark_dirty(void);

void face_window_set_hands(int32_t hour_angle, int32_t min_angle);
void face_window_set_date(int day, int month, int weekday);
void face_window_set_connected(bool connected);
void face_window_set_notif(int count);
void face_window_set_alarm_pending(bool pending);
void face_window_set_event_pending(bool pending);

// Marks the watchface dirty only if the battery state actually changed.
void face_window_set_battery(int dots, bool is_charging);
