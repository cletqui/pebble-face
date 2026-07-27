# TicDot

Minimal analog watchface for Pebble color watches. All information is encoded as colored dots on a single ring, arranged clockwise from 12 o'clock.

## Layout

All dots share the same outer ring. Going **clockwise from 12**:

| Position | Group | Dots |
|----------|-------|------|
| 12 o'clock | Bluetooth | 1 (larger) |
| 1–2 o'clock | Battery | 5 |
| 2–3 o'clock | Alarm, Notification, Event, Heart rate, Activity | 1 each |
| 4:30–7:30 | Steps | 10 |
| ~9:30 | Weekday | 1 |
| 9:30–10:30 | Date | 5 (binary) |
| 10:30–11 | Month | 4 (binary) |

Groups that are disabled are skipped and the remaining ones compact toward 12.

## Dot reference

### Bluetooth (12 o'clock)

Single larger dot. Configured color (default White) when connected, Red when disconnected.

### Battery (1–2 o'clock) — 5 dots

Each dot represents 20% charge, filling toward 12. Color reflects charge level:

| Dots lit | Charge | Color  |
| -------- | ------ | ------ |
| 5        | 100%   | White  |
| 4        | 80%    | White  |
| 3        | 60%    | Yellow |
| 2        | 40%    | Orange |
| 1        | 20%    | Red    |

All lit dots turn **Cyan** while charging.

### Status dots (2–3 o'clock)

Single dots arranged clockwise after the battery group. Each can be toggled independently.

| Dot          | Dim (off)  | Lit (on)                                          |
| ------------ | ---------- | ------------------------------------------------- |
| Alarm        | No alarm   | Upcoming alarm pending                            |
| Notification | No notifs  | Unread notifications; alert color above threshold |
| Event        | No events  | Upcoming calendar event                           |
| Heart rate   | No reading | Normal BPM; 4 stacked alert tiers above configurable BPM thresholds; configurable error color when sensor fails |
| Activity     | Idle       | Walk or run detected                              |

### Steps (4:30–7:30) — 10 dots

Each dot = 1/10th of the daily goal (default 10 000 steps). Dots light up in the **Dot Completed Color** as steps accumulate, starting from the **Dot Default Color** (unlit).

Once the goal is reached, the dot at that position becomes a **milestone dot**: bigger, and colored with the single **Milestone Color**. The remaining dots keep tracking live progress toward the *next* milestone in the normal default/completed colors. This lets the same 10-dot ring encode up to 10× the daily goal — e.g. with the default 10 000-step goal, 3 big dots means 30 000 steps done, and a full ring of big dots means 100 000.

### Month (10–11 o'clock) — 4 dots, binary

4-bit encoding. Default is LSB nearest 12 (little-endian): January = `1000`, December = `0011`. Can be switched to MSB-first (big-endian) in settings.

### Date (10–11 o'clock) — 5 dots, binary

5-bit encoding. Default is LSB nearest 12 (little-endian): Day 1 = `10000`, day 31 = `11111`. Can be switched to MSB-first (big-endian) in settings.

### Weekday (10 o'clock) — 1 dot

Color encodes the day, defaulting to the French/ancient planetary associations:

| Day       | Planet  | Color      |
| --------- | ------- | ---------- |
| Sunday    | Sol     | Yellow     |
| Monday    | Luna    | Light Gray |
| Tuesday   | Mars    | Red        |
| Wednesday | Mercury | Green      |
| Thursday  | Jupiter | Orange     |
| Friday    | Venus   | Pink       |
| Saturday  | Saturn  | Magenta    |

All seven colors are individually configurable.

## Settings

Configured from the Pebble / Rebble phone app. All dot groups can be toggled on or off individually; the layout adapts automatically.

| Section        | Setting                                  | Default        |
| -------------- | ---------------------------------------- | -------------- |
| Display        | Theme (Dark / Light / Auto by time)      | Dark           |
| Display        | Auto: light theme start / end hour       | 7 / 20         |
| Display        | Binary dots LSB first                    | On             |
| Display        | Hour ticks                               | Off            |
| Display        | Battery / Steps / Date / Month / Weekday | All on         |
| Display        | Alarm / Event dots                       | On             |
| Display        | Notification dot                         | Off            |
| Display        | Heart rate / Activity dots               | On             |
| Steps          | Daily step goal                          | 10 000         |
| Steps          | Dot default / completed color            | Light Gray / Cyan |
| Steps          | Milestone color                          | Green          |
| Clock Hands    | Hour hand color                          | Red            |
| Clock Hands    | Minute hand color                        | White          |
| Bluetooth      | Vibrate on disconnect                    | On             |
| Bluetooth      | Connected dot color                      | White          |
| Alarm & Events | Alarm / Event dot color                  | White          |
| Notifications  | Normal / Alert color                     | White / Red    |
| Notifications  | Alert threshold (notif count)            | 5              |
| Health         | Heart rate normal / error color          | White / Magenta |
| Health         | HR threshold 1-4 (BPM) / color           | 100 Yellow, 130 Orange, 150 Red, 170 Purple |
| Health         | Activity dot color                       | Green          |
| Weekday Colors | Per-day color (Sun–Sat)                  | Planetary      |

Available colors: Orange, Red, Green, Blue, Cyan, Yellow, Magenta, White, Light Gray, Pink, Purple.

HR thresholds 2–4 are automatically clamped to be no lower than the tier before them, so an inverted setting (e.g. Threshold 2 lower than Threshold 1) can't create a tier that never displays.

### Theme

**Dark** and **Light** force the background; **Auto** switches based on the current hour, using the configured start/end hour range (wraps past midnight if start > end). In Light theme, White flips to Black and Light Gray flips to Dark Gray wherever they'd otherwise vanish against the white background, so every other color setting keeps the same visual weight without needing to be reconfigured.

## Power

- Ticks on `MINUTE_UNIT` only — no per-second wakeups
- Battery and Bluetooth handlers are fully event-driven
- Heart rate and activity update on health events only, not every tick
- Canvas redraws only when display state changes
- No system calls in the draw pass
- Dot positions (all trig/`sin_lookup`/`cos_lookup` calls) are precomputed into a cache whenever geometry or dot-visibility settings change, never inside the draw pass itself

## Architecture

```
src/c/
  main.c              init/deinit, event handlers, AppMessage inbox — the coordinator
  modules/
    settings.{h,c}     Settings struct, persistence, HR threshold clamping
    theme.{h,c}        color table, dark/light/auto theme state
    health.{h,c}       step/HR/activity state from HealthService
    layout.{h,c}       geometry + dot position cache
  windows/
    face_window.{h,c}  the watchface window: canvas, draw pass, draw-time-only state
```

Each module exposes only what other modules need through its header; internal state stays `static` inside the `.c` file (e.g. battery/notification/connection state lives only in `face_window.c`). `main.c` doesn't contain any drawing or health logic itself — it just wires services to module functions.

## Platforms

Color platforms only: **basalt** (Pebble Time), **chalk** (Pebble Time Round), **emery** (Pebble Time 2).

## Building

```sh
# First time: install Clay
npm install pebble-clay --save --ignore-scripts

pebble build
pebble install --emulator emery      # Pebble Time 2 emulator
pebble install --phone <ip>          # sideload via Developer Mode
pebble install --cloudpebble         # sideload via Rebble proxy
```

> Run `pebble clean` before rebuilding after any `messageKeys` change in `package.json`.

## References

- Rebble SDK docs: <https://developer.rebble.io>
- Rebble app store: <https://apps.rebble.io>
- TicDot on Rebble: <https://apps.repebble.com/01d9cb793d7b4f109ad3a9cc>
