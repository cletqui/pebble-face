#include "health.h"
#include "settings.h"
#include "../windows/face_window.h"

#define STEP_GOAL_DEFAULT 10000

int step_lit;
int step_milestones;
int heart_rate;
bool heart_rate_error;
bool activity_active;

void health_update_steps(void) {
  if (!s_settings.show_step_dots)
    return;
  int32_t steps = 0;
#if defined(PBL_HEALTH)
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(
      HealthMetricStepCount, time_start_of_today(), time(NULL));
  if (mask & HealthServiceAccessibilityMaskAvailable) {
    steps = (int32_t)health_service_sum_today(HealthMetricStepCount);
  }
#endif
  int goal =
      s_settings.step_goal > 0 ? s_settings.step_goal : STEP_GOAL_DEFAULT;
  step_milestones = (int)(steps / goal);
  step_lit = (int32_t)(steps % goal) * 10 / goal;
  // Once a goal has been hit, keep at least one dot lit so the milestone
  // color has somewhere to show — otherwise right at a goal boundary
  // step_lit rounds down to 0 and the milestone would be invisible until
  // ~1,000 more steps.
  if (step_milestones > 0 && step_lit == 0)
    step_lit = 1;
}

void health_update_hr_activity(void) {
#if defined(PBL_HEALTH)
  if (s_settings.show_hr_dot) {
    time_t now = time(NULL);
    HealthServiceAccessibilityMask mask = health_service_metric_accessible(
        HealthMetricHeartRateBPM, now - 300, now);
    if (mask & HealthServiceAccessibilityMaskAvailable) {
      heart_rate =
          (int)health_service_peek_current_value(HealthMetricHeartRateBPM);
      heart_rate_error = false;
    } else if (mask & HealthServiceAccessibilityMaskNotSupported) {
      heart_rate = 0;
      heart_rate_error = false;
    } else {
      heart_rate = 0;
      heart_rate_error = true;
    }
  }
  if (s_settings.show_activity_dot) {
    HealthActivityMask activities = health_service_peek_current_activities();
    activity_active =
        (activities & HealthActivityWalk) || (activities & HealthActivityRun);
  }
#endif
}

#if defined(PBL_HEALTH)
static void health_event_handler(HealthEventType event, void *context) {
  if (event == HealthEventHeartRateUpdate ||
      event == HealthEventMovementUpdate) {
    health_update_hr_activity();
    face_window_mark_dirty();
  }
}
#endif

void health_subscribe(void) {
#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_event_handler, NULL);
#endif
}
