#include "layout.h"
#include "settings.h"

#define SPACING_DEG 10 // dot-to-dot spacing within a group
#define GAP1_DEG 15    // gap between BT dot and first group
#define GAP2_DEG 13    // gap between groups

int outer_r;
int hour_len;
int min_len;
GPoint center;

GPoint pos_bt;
GPoint pos_battery[5];
GPoint pos_alarm, pos_notif, pos_event, pos_hr, pos_activity;
GPoint pos_step[10];
GPoint pos_month[4];
GPoint pos_date[5];
GPoint pos_weekday;
GPoint pos_tick_a[12], pos_tick_b[12];

GPoint point_on_circle(GPoint c, int32_t angle, int radius) {
  return (GPoint){
      .x = c.x + (int)(sin_lookup(angle) * radius / TRIG_MAX_RATIO),
      .y = c.y - (int)(cos_lookup(angle) * radius / TRIG_MAX_RATIO),
  };
}

int32_t deg_to_trig(int degrees) { return (TRIG_MAX_ANGLE * degrees) / 360; }

void layout_rebuild_positions(void) {
  pos_bt = point_on_circle(center, 0, outer_r);

  // Right side: battery → alarm → notif → event → HR → activity (clockwise)
  int r_ang = GAP1_DEG;

  if (s_settings.show_battery_dots) {
    for (int i = 0; i < 5; i++) {
      pos_battery[i] =
          point_on_circle(center, deg_to_trig(r_ang + i * SPACING_DEG), outer_r);
    }
    r_ang += 4 * SPACING_DEG + GAP2_DEG;
  }

  if (s_settings.show_alarm_dot) {
    pos_alarm = point_on_circle(center, deg_to_trig(r_ang), outer_r);
    r_ang += GAP2_DEG;
  }

  if (s_settings.show_notif_dot) {
    pos_notif = point_on_circle(center, deg_to_trig(r_ang), outer_r);
    r_ang += GAP2_DEG;
  }

  if (s_settings.show_event_dot) {
    pos_event = point_on_circle(center, deg_to_trig(r_ang), outer_r);
    r_ang += GAP2_DEG;
  }

  if (s_settings.show_hr_dot) {
    pos_hr = point_on_circle(center, deg_to_trig(r_ang), outer_r);
    r_ang += GAP2_DEG;
  }

  if (s_settings.show_activity_dot) {
    pos_activity = point_on_circle(center, deg_to_trig(r_ang), outer_r);
  }

  // Steps: bottom arc centered at 6 o'clock
  if (s_settings.show_step_dots) {
    for (int i = 0; i < 10; i++) {
      pos_step[i] =
          point_on_circle(center, deg_to_trig(225 - i * SPACING_DEG), outer_r);
    }
  }

  // Left side: month → date → weekday (counter-clockwise)
  int l_ang = 360 - GAP1_DEG;

  if (s_settings.show_month_dots) {
    for (int i = 0; i < 4; i++) {
      pos_month[i] =
          point_on_circle(center, deg_to_trig(l_ang - i * SPACING_DEG), outer_r);
    }
    l_ang -= 3 * SPACING_DEG + GAP2_DEG;
  }

  if (s_settings.show_date_dots) {
    for (int i = 0; i < 5; i++) {
      pos_date[i] =
          point_on_circle(center, deg_to_trig(l_ang - i * SPACING_DEG), outer_r);
    }
    l_ang -= 4 * SPACING_DEG + GAP2_DEG;
  }

  if (s_settings.show_weekday_dot) {
    pos_weekday = point_on_circle(center, deg_to_trig(l_ang), outer_r);
  }
}

void layout_compute_geometry(GRect bounds) {
  int min_dim = bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h;
  outer_r = min_dim / 2 - PBL_IF_ROUND_ELSE(10, 6);
  hour_len = outer_r * 55 / 94;
  min_len = outer_r * 74 / 94;
  center = grect_center_point(&bounds);

  for (int h = 0; h < 12; h++) {
    int32_t a = deg_to_trig(h * 30);
    pos_tick_a[h] = point_on_circle(center, a, outer_r - 12);
    pos_tick_b[h] = point_on_circle(center, a, outer_r - 5);
  }

  layout_rebuild_positions();
}
