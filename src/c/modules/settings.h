#pragma once
#include <pebble.h>

// Fields appended at end only, never reordered or removed, so persisted
// settings on existing installs stay valid after an update.
typedef struct {
  int step_goal;
  bool vibrate_disconnect;
  bool show_step_dots;
  bool show_date_dots;
  bool show_battery_dots;
  uint8_t hour_color_idx;
  uint8_t minute_color_idx;
  uint8_t milestone_color_idx;      // big dot once a goal-multiple is hit
  uint8_t step_completed_color_idx; // lit sub-goal dots, not yet a milestone
  bool show_month_dots;
  bool show_weekday_dot;
  bool show_alarm_dot;
  bool show_notif_dot;
  bool show_event_dot;
  uint8_t bt_color_idx;
  uint8_t alarm_color_idx;
  uint8_t notif_normal_color_idx;
  uint8_t notif_alert_color_idx;
  uint8_t notif_threshold;
  uint8_t event_color_idx;
  uint8_t weekday_color_idx[7];
  bool show_hr_dot;
  bool show_activity_dot;
  uint8_t hr_color_idx;
  uint8_t hr_alert_color_idx; // HR threshold 1 color
  uint8_t hr_alert_bpm;       // HR threshold 1 BPM
  uint8_t activity_color_idx;
  bool little_endian_dots;
  bool show_hour_ticks;
  uint8_t hr_error_color_idx;
  bool light_theme_unused; // deprecated: replaced by theme_mode below
  uint8_t hr_threshold2_bpm;
  uint8_t hr_threshold2_color_idx;
  uint8_t hr_threshold3_bpm;
  uint8_t hr_threshold3_color_idx;
  uint8_t hr_threshold4_bpm;
  uint8_t hr_threshold4_color_idx;
  uint8_t theme_mode; // 0=Dark 1=Light 2=Auto (by hour)
  uint8_t light_start_hour;
  uint8_t light_end_hour;
  uint8_t step_default_color_idx; // unlit sub-goal dot color
} Settings;

extern Settings s_settings;

void settings_load(void);
void settings_save(void);

// Clamps HR thresholds 2-4 to be non-decreasing relative to the one before,
// so a threshold set lower than a previous tier (e.g. a fat-fingered phone
// edit) can't create a tier that never displays.
void settings_normalize(void);
