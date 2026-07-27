#include "theme.h"
#include "settings.h"

// 0=Orange 1=Red 2=Green 3=Blue 4=Cyan 5=Yellow 6=Magenta 7=White 8=LightGray
// 9=Pink 10=Purple
static const GColor s_color_table[] = {
    GColorOrange,       GColorRed,          GColorIslamicGreen,
    GColorBlue,         GColorCyan,         GColorYellow,
    GColorMagenta,      GColorWhite,        GColorLightGray,
    GColorShockingPink, GColorPurple};

static bool s_light_active;

void theme_update(int hour) {
  switch (s_settings.theme_mode) {
  case 1: // Light
    s_light_active = true;
    break;
  case 2: { // Auto: on during [start, end), handling wrap past midnight
    int start = s_settings.light_start_hour;
    int end = s_settings.light_end_hour;
    s_light_active = start <= end ? (hour >= start && hour < end)
                                   : (hour >= start || hour < end);
    break;
  }
  default: // Dark
    s_light_active = false;
    break;
  }
}

// White and Light Gray are near-invisible on a light background (and vice
// versa on dark), so in light theme they're swapped for their
// contrast-equivalent counterpart. Every other color keeps its chosen value
// in both themes.
GColor get_color(uint8_t idx) {
  GColor c = s_color_table[idx % ARRAY_LENGTH(s_color_table)];
  if (s_light_active) {
    if (gcolor_equal(c, GColorWhite))
      return GColorBlack;
    if (gcolor_equal(c, GColorLightGray))
      return GColorDarkGray;
  }
  return c;
}

GColor theme_bg(void) { return s_light_active ? GColorWhite : GColorBlack; }

GColor theme_dim(void) {
  return s_light_active ? GColorLightGray : GColorDarkGray;
}

GColor theme_fg(void) { return s_light_active ? GColorBlack : GColorWhite; }
