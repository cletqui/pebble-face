#include "face_window.h"
#include "../modules/health.h"
#include "../modules/layout.h"
#include "../modules/settings.h"
#include "../modules/theme.h"

static Window *s_window;
static Layer *s_canvas;

// Draw-time-only state: not persisted, not owned by any other module.
static int32_t s_hour_angle;
static int32_t s_min_angle;
static int s_day, s_month, s_weekday;
static int s_battery_dots;
static bool s_is_charging;
static bool s_connected;
static int s_notif_count;
static bool s_alarm_pending;
static bool s_event_pending;

// Draw pass — zero system calls, reads only pre-computed state and the
// position cache built by layout_compute_geometry()/layout_rebuild_positions().
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, theme_bg());
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  // Hour tick marks just inside the dot ring
  if (s_settings.show_hour_ticks) {
    graphics_context_set_stroke_color(ctx, theme_dim());
    graphics_context_set_stroke_width(ctx, 1);
    for (int h = 0; h < 12; h++) {
      graphics_draw_line(ctx, pos_tick_a[h], pos_tick_b[h]);
    }
  }

  // BT dot at 12 o'clock
  graphics_context_set_fill_color(
      ctx, s_connected ? get_color(s_settings.bt_color_idx) : GColorRed);
  graphics_fill_circle(ctx, pos_bt, 4);

  if (s_settings.show_battery_dots) {
    // Color progression: charging=Cyan, 4-5 dots=fg, 3=Yellow, 2=Orange,
    // 1=Red
    GColor lit_color = s_is_charging         ? GColorCyan
                       : s_battery_dots >= 4 ? theme_fg()
                       : s_battery_dots == 3 ? GColorYellow
                       : s_battery_dots == 2 ? GColorOrange
                                             : GColorRed;
    for (int i = 0; i < 5; i++) {
      graphics_context_set_fill_color(
          ctx, i < 5 - s_battery_dots ? theme_dim() : lit_color);
      graphics_fill_circle(ctx, pos_battery[i], 2);
    }
  }

  if (s_settings.show_alarm_dot) {
    graphics_context_set_fill_color(
        ctx, s_alarm_pending ? get_color(s_settings.alarm_color_idx)
                             : theme_dim());
    graphics_fill_circle(ctx, pos_alarm, 2);
  }

  if (s_settings.show_notif_dot) {
    GColor c = s_notif_count == 0 ? theme_dim()
               : s_notif_count >= s_settings.notif_threshold
                   ? get_color(s_settings.notif_alert_color_idx)
                   : get_color(s_settings.notif_normal_color_idx);
    graphics_context_set_fill_color(ctx, c);
    graphics_fill_circle(ctx, pos_notif, 2);
  }

  if (s_settings.show_event_dot) {
    graphics_context_set_fill_color(
        ctx, s_event_pending ? get_color(s_settings.event_color_idx)
                             : theme_dim());
    graphics_fill_circle(ctx, pos_event, 2);
  }

  if (s_settings.show_hr_dot) {
    // Highest matching tier wins: normal < tier1 < tier2 < tier3 < tier4
    GColor c;
    if (heart_rate_error) {
      c = get_color(s_settings.hr_error_color_idx);
    } else if (heart_rate == 0) {
      c = theme_dim();
    } else if (heart_rate >= s_settings.hr_threshold4_bpm) {
      c = get_color(s_settings.hr_threshold4_color_idx);
    } else if (heart_rate >= s_settings.hr_threshold3_bpm) {
      c = get_color(s_settings.hr_threshold3_color_idx);
    } else if (heart_rate >= s_settings.hr_threshold2_bpm) {
      c = get_color(s_settings.hr_threshold2_color_idx);
    } else if (heart_rate >= s_settings.hr_alert_bpm) {
      c = get_color(s_settings.hr_alert_color_idx);
    } else {
      c = get_color(s_settings.hr_color_idx);
    }
    graphics_context_set_fill_color(ctx, c);
    graphics_fill_circle(ctx, pos_hr, 2);
  }

  if (s_settings.show_activity_dot) {
    graphics_context_set_fill_color(
        ctx, activity_active ? get_color(s_settings.activity_color_idx)
                             : theme_dim());
    graphics_fill_circle(ctx, pos_activity, 2);
  }

  // Steps: step_lit dots are lit (1,000 steps each). The milestone color
  // overlays the *front* of that same lit run — up to step_milestones of
  // them — rather than claiming its own dots next to it, so a completed
  // goal never costs the progress display any room.
  if (s_settings.show_step_dots) {
    int milestone_dots =
        step_milestones < step_lit ? step_milestones : step_lit;
    for (int i = 0; i < 10; i++) {
      bool lit = i < step_lit;
      bool milestone = i < milestone_dots;
      GColor c;
      uint8_t radius;
      if (milestone) {
        c = get_color(s_settings.milestone_color_idx);
        radius = 3;
      } else if (lit) {
        c = get_color(s_settings.step_completed_color_idx);
        radius = 2;
      } else {
        c = get_color(s_settings.step_default_color_idx);
        radius = 2;
      }
      graphics_context_set_fill_color(ctx, c);
      graphics_fill_circle(ctx, pos_step[i], radius);
    }
  }

  if (s_settings.show_month_dots) {
    for (int i = 0; i < 4; i++) {
      int bit = s_settings.little_endian_dots ? i : (3 - i);
      bool lit = (s_month & (1 << bit)) != 0;
      graphics_context_set_fill_color(ctx, lit ? theme_fg() : theme_dim());
      graphics_fill_circle(ctx, pos_month[i], 2);
    }
  }

  if (s_settings.show_date_dots) {
    for (int i = 0; i < 5; i++) {
      int bit = s_settings.little_endian_dots ? i : (4 - i);
      bool lit = (s_day & (1 << bit)) != 0;
      graphics_context_set_fill_color(ctx, lit ? theme_fg() : theme_dim());
      graphics_fill_circle(ctx, pos_date[i], 2);
    }
  }

  if (s_settings.show_weekday_dot) {
    graphics_context_set_fill_color(
        ctx, get_color(s_settings.weekday_color_idx[s_weekday]));
    graphics_fill_circle(ctx, pos_weekday, 2);
  }

  // Clock hands — angles pre-computed in tick handler
  graphics_context_set_stroke_width(ctx, 10);
  graphics_context_set_stroke_color(ctx, get_color(s_settings.hour_color_idx));
  graphics_draw_line(ctx, center, point_on_circle(center, s_hour_angle, hour_len));
  graphics_context_set_stroke_color(ctx,
                                    get_color(s_settings.minute_color_idx));
  graphics_draw_line(ctx, center, point_on_circle(center, s_min_angle, min_len));
}

static void unobstructed_change(AnimationProgress progress, void *context) {
  layout_compute_geometry(
      layer_get_unobstructed_bounds(window_get_root_layer(s_window)));
  layer_mark_dirty(s_canvas);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  layout_compute_geometry(layer_get_unobstructed_bounds(root));
  s_canvas = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(root, s_canvas);
  unobstructed_area_service_subscribe(
      (UnobstructedAreaHandlers){.change = unobstructed_change}, NULL);
}

static void window_unload(Window *window) {
  unobstructed_area_service_unsubscribe();
  layer_destroy(s_canvas);
}

void face_window_push(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = window_load,
                                           .unload = window_unload,
                                       });
  window_stack_push(s_window, true);
}

void face_window_teardown(void) { window_destroy(s_window); }

void face_window_mark_dirty(void) {
  if (s_canvas)
    layer_mark_dirty(s_canvas);
}

void face_window_set_hands(int32_t hour_angle, int32_t min_angle) {
  s_hour_angle = hour_angle;
  s_min_angle = min_angle;
}

void face_window_set_date(int day, int month, int weekday) {
  s_day = day;
  s_month = month;
  s_weekday = weekday;
}

void face_window_set_connected(bool connected) {
  s_connected = connected;
  face_window_mark_dirty();
}

void face_window_set_notif(int count) {
  s_notif_count = count;
  face_window_mark_dirty();
}

void face_window_set_alarm_pending(bool pending) {
  s_alarm_pending = pending;
  face_window_mark_dirty();
}

void face_window_set_event_pending(bool pending) {
  s_event_pending = pending;
  face_window_mark_dirty();
}

void face_window_set_battery(int dots, bool is_charging) {
  if (dots == s_battery_dots && is_charging == s_is_charging)
    return;
  s_battery_dots = dots;
  s_is_charging = is_charging;
  face_window_mark_dirty();
}
