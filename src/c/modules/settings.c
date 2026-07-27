#include "settings.h"

#define SETTINGS_PERSIST_KEY 1
#define STEP_GOAL_DEFAULT 10000

Settings s_settings;

void settings_load(void) {
  s_settings = (Settings){
      .step_goal = STEP_GOAL_DEFAULT,
      .vibrate_disconnect = true,
      .show_step_dots = true,
      .show_date_dots = true,
      .show_battery_dots = true,
      .hour_color_idx = 1,   // Red
      .minute_color_idx = 7, // White
      .milestone_color_idx = 2,      // Green
      .step_completed_color_idx = 4, // Cyan
      .show_month_dots = true,
      .show_weekday_dot = true,
      .show_alarm_dot = true,
      .show_notif_dot = false,
      .show_event_dot = true,
      .bt_color_idx = 7,           // White
      .alarm_color_idx = 7,        // White
      .notif_normal_color_idx = 7, // White
      .notif_alert_color_idx = 1,  // Red
      .notif_threshold = 5,
      .event_color_idx = 7, // White
      // French/ancient planetary: Sun=Yellow Mon=LightGray Tue=Red Wed=Green
      //                           Thu=Orange Fri=Pink      Sat=Magenta
      .weekday_color_idx = {5, 8, 1, 2, 0, 9, 6},
      .show_hr_dot = true,
      .show_activity_dot = true,
      .hr_color_idx = 7,       // White
      .hr_alert_color_idx = 5, // Yellow (tier 1)
      .hr_alert_bpm = 100,
      .activity_color_idx = 2, // Green
      .little_endian_dots = true,
      .show_hour_ticks = false,
      .hr_error_color_idx = 6, // Magenta
      .light_theme_unused = false,
      .hr_threshold2_bpm = 130,
      .hr_threshold2_color_idx = 0, // Orange
      .hr_threshold3_bpm = 150,
      .hr_threshold3_color_idx = 1, // Red
      .hr_threshold4_bpm = 170,
      .hr_threshold4_color_idx = 10, // Purple
      .theme_mode = 0,               // Dark
      .light_start_hour = 7,
      .light_end_hour = 20,
      .step_default_color_idx = 8, // Light Gray
  };
  persist_read_data(SETTINGS_PERSIST_KEY, &s_settings, sizeof(s_settings));
  settings_normalize();
}

void settings_save(void) {
  persist_write_data(SETTINGS_PERSIST_KEY, &s_settings, sizeof(s_settings));
}

void settings_normalize(void) {
  if (s_settings.hr_threshold2_bpm < s_settings.hr_alert_bpm)
    s_settings.hr_threshold2_bpm = s_settings.hr_alert_bpm;
  if (s_settings.hr_threshold3_bpm < s_settings.hr_threshold2_bpm)
    s_settings.hr_threshold3_bpm = s_settings.hr_threshold2_bpm;
  if (s_settings.hr_threshold4_bpm < s_settings.hr_threshold3_bpm)
    s_settings.hr_threshold4_bpm = s_settings.hr_threshold3_bpm;
}
