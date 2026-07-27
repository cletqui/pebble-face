#include "message_keys.auto.h"
#include <pebble.h>

#include "modules/health.h"
#include "modules/layout.h"
#include "modules/settings.h"
#include "modules/theme.h"
#include "windows/face_window.h"

static int32_t hour_angle_for(struct tm *t) {
  return deg_to_trig(((t->tm_hour % 12) * 60 + t->tm_min) * 360 / (12 * 60));
}

static int32_t min_angle_for(struct tm *t) {
  return deg_to_trig(t->tm_min * 6);
}

static void tick_handler(struct tm *t, TimeUnits units_changed) {
  face_window_set_hands(hour_angle_for(t), min_angle_for(t));

  if (t->tm_hour == 0 && t->tm_min == 0) {
    face_window_set_date(t->tm_mday, t->tm_mon + 1, t->tm_wday);
  }

  theme_update(t->tm_hour);
  health_update_steps();
  face_window_mark_dirty();
}

static void battery_handler(BatteryChargeState state) {
  int dots = (state.charge_percent + 19) / 20;
  face_window_set_battery(dots > 5 ? 5 : dots, state.is_charging);
}

static void connection_handler(bool connected) {
  face_window_set_connected(connected);
  if (!connected && s_settings.vibrate_disconnect) {
    vibes_short_pulse();
  }
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t;

// Clay sends select/slider values as CSTRING (HTML form data is always
// strings). Read with atoi() when the tuple type is CSTRING, fall back to
// numeric read otherwise.
#define TUPLE_INT_VAL(t)                                                     \
  ((t)->type == TUPLE_CSTRING ? atoi((t)->value->cstring) : (t)->value->int32)
#define APPLY_BOOL(key, field)                                               \
  t = dict_find(iter, key);                                                  \
  if (t)                                                                     \
    s_settings.field = TUPLE_INT_VAL(t) != 0;
#define APPLY_U8(key, field)                                                 \
  t = dict_find(iter, key);                                                  \
  if (t)                                                                     \
    s_settings.field = (uint8_t)TUPLE_INT_VAL(t);
#define APPLY_I32(key, field)                                                \
  t = dict_find(iter, key);                                                  \
  if (t)                                                                     \
    s_settings.field = (int)TUPLE_INT_VAL(t);

  APPLY_I32(MESSAGE_KEY_StepGoal, step_goal)
  APPLY_BOOL(MESSAGE_KEY_VibrateOnDisconnect, vibrate_disconnect)
  APPLY_BOOL(MESSAGE_KEY_ShowStepDots, show_step_dots)
  APPLY_BOOL(MESSAGE_KEY_ShowDateDots, show_date_dots)
  APPLY_BOOL(MESSAGE_KEY_ShowBatteryDots, show_battery_dots)
  APPLY_U8(MESSAGE_KEY_HourColor, hour_color_idx)
  APPLY_U8(MESSAGE_KEY_MinuteColor, minute_color_idx)
  APPLY_U8(MESSAGE_KEY_MilestoneColor, milestone_color_idx)
  APPLY_U8(MESSAGE_KEY_StepCompletedColor, step_completed_color_idx)
  APPLY_U8(MESSAGE_KEY_StepDefaultColor, step_default_color_idx)
  APPLY_BOOL(MESSAGE_KEY_ShowMonthDots, show_month_dots)
  APPLY_BOOL(MESSAGE_KEY_ShowWeekdayDot, show_weekday_dot)
  APPLY_BOOL(MESSAGE_KEY_ShowAlarmDot, show_alarm_dot)
  APPLY_BOOL(MESSAGE_KEY_ShowNotifDot, show_notif_dot)
  APPLY_BOOL(MESSAGE_KEY_ShowEventDot, show_event_dot)
  APPLY_U8(MESSAGE_KEY_BtColor, bt_color_idx)
  APPLY_U8(MESSAGE_KEY_AlarmColor, alarm_color_idx)
  APPLY_U8(MESSAGE_KEY_NotifNormalColor, notif_normal_color_idx)
  APPLY_U8(MESSAGE_KEY_NotifAlertColor, notif_alert_color_idx)
  APPLY_U8(MESSAGE_KEY_NotifThreshold, notif_threshold)
  APPLY_U8(MESSAGE_KEY_EventColor, event_color_idx)
  APPLY_BOOL(MESSAGE_KEY_ShowHrDot, show_hr_dot)
  APPLY_BOOL(MESSAGE_KEY_ShowActivityDot, show_activity_dot)
  APPLY_U8(MESSAGE_KEY_HrColor, hr_color_idx)
  APPLY_U8(MESSAGE_KEY_HrAlertColor, hr_alert_color_idx)
  APPLY_U8(MESSAGE_KEY_HrAlertBpm, hr_alert_bpm)
  APPLY_U8(MESSAGE_KEY_ActivityColor, activity_color_idx)
  APPLY_BOOL(MESSAGE_KEY_LittleEndianDots, little_endian_dots)
  APPLY_BOOL(MESSAGE_KEY_ShowHourTicks, show_hour_ticks)
  APPLY_U8(MESSAGE_KEY_HrErrorColor, hr_error_color_idx)
  APPLY_U8(MESSAGE_KEY_HrThreshold2Bpm, hr_threshold2_bpm)
  APPLY_U8(MESSAGE_KEY_HrThreshold2Color, hr_threshold2_color_idx)
  APPLY_U8(MESSAGE_KEY_HrThreshold3Bpm, hr_threshold3_bpm)
  APPLY_U8(MESSAGE_KEY_HrThreshold3Color, hr_threshold3_color_idx)
  APPLY_U8(MESSAGE_KEY_HrThreshold4Bpm, hr_threshold4_bpm)
  APPLY_U8(MESSAGE_KEY_HrThreshold4Color, hr_threshold4_color_idx)
  APPLY_U8(MESSAGE_KEY_ThemeMode, theme_mode)
  APPLY_U8(MESSAGE_KEY_LightStartHour, light_start_hour)
  APPLY_U8(MESSAGE_KEY_LightEndHour, light_end_hour)

#undef APPLY_BOOL
#undef APPLY_U8
#undef APPLY_I32

  for (int day = 0; day < 7; day++) {
    t = dict_find(iter, MESSAGE_KEY_WeekdayColor0 + day);
    if (t)
      s_settings.weekday_color_idx[day] = (uint8_t)TUPLE_INT_VAL(t);
  }

  t = dict_find(iter, MESSAGE_KEY_NotifCount);
  if (t)
    face_window_set_notif(t->value->int32);

  t = dict_find(iter, MESSAGE_KEY_AlarmPending);
  if (t)
    face_window_set_alarm_pending(t->value->uint8 != 0);

  t = dict_find(iter, MESSAGE_KEY_EventPending);
  if (t)
    face_window_set_event_pending(t->value->uint8 != 0);

  settings_normalize();
  settings_save();
  health_update_steps();
  health_update_hr_activity();
  layout_rebuild_positions();
  time_t now = time(NULL);
  theme_update(localtime(&now)->tm_hour);
  face_window_mark_dirty();
}

static void prv_init(void) {
  settings_load();

  battery_handler(battery_state_service_peek());
  face_window_set_connected(connection_service_peek_pebble_app_connection());
  health_update_steps();
  health_update_hr_activity();

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  face_window_set_date(t->tm_mday, t->tm_mon + 1, t->tm_wday);
  face_window_set_hands(hour_angle_for(t), min_angle_for(t));
  theme_update(t->tm_hour);

  face_window_push();

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = connection_handler,
  });
  app_message_register_inbox_received(inbox_received);
  app_message_open(512, 64);
  health_subscribe();
}

static void prv_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  face_window_teardown();
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
