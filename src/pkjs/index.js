var Clay = require("pebble-clay");

var COLOR_OPTIONS = [
  { label: "Orange", value: 0 },
  { label: "Red", value: 1 },
  { label: "Green", value: 2 },
  { label: "Blue", value: 3 },
  { label: "Cyan", value: 4 },
  { label: "Yellow", value: 5 },
  { label: "Magenta", value: 6 },
  { label: "White", value: 7 },
  { label: "Light Gray", value: 8 },
  { label: "Pink", value: 9 },
  { label: "Purple", value: 10 },
];

function colorSelect(messageKey, label, defaultValue) {
  return {
    type: "select",
    messageKey: messageKey,
    label: label,
    defaultValue: defaultValue,
    options: COLOR_OPTIONS,
  };
}

var clayConfig = [
  { type: "heading", defaultValue: "TicDot" },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Display" },
      {
        type: "select",
        messageKey: "ThemeMode",
        label: "Theme",
        defaultValue: 0,
        options: [
          { label: "Dark", value: 0 },
          { label: "Light", value: 1 },
          { label: "Auto (by time of day)", value: 2 },
        ],
      },
      {
        type: "slider",
        messageKey: "LightStartHour",
        label: "Auto: Light Theme Start Hour",
        defaultValue: 7,
        min: 0,
        max: 23,
        step: 1,
      },
      {
        type: "slider",
        messageKey: "LightEndHour",
        label: "Auto: Light Theme End Hour",
        defaultValue: 20,
        min: 0,
        max: 23,
        step: 1,
      },
      {
        type: "toggle",
        messageKey: "LittleEndianDots",
        label: "Binary Dots: LSB First",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowBatteryDots",
        label: "Battery Dots",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowStepDots",
        label: "Step Dots",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowDateDots",
        label: "Date Dots",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowMonthDots",
        label: "Month Dots",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowWeekdayDot",
        label: "Weekday Dot",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowAlarmDot",
        label: "Alarm Dot",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowNotifDot",
        label: "Notification Dot",
        defaultValue: false,
      },
      {
        type: "toggle",
        messageKey: "ShowEventDot",
        label: "Event Dot",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowHourTicks",
        label: "Hour Ticks",
        defaultValue: false,
      },
      {
        type: "toggle",
        messageKey: "ShowHrDot",
        label: "Heart Rate Dot",
        defaultValue: true,
      },
      {
        type: "toggle",
        messageKey: "ShowActivityDot",
        label: "Activity Dot",
        defaultValue: true,
      },
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Steps" },
      {
        type: "slider",
        messageKey: "StepGoal",
        label: "Daily Goal",
        defaultValue: 10000,
        min: 1000,
        max: 30000,
        step: 1000,
      },
      {
        type: "text",
        defaultValue:
          "Each dot is 1/10th of the daily goal, and lights up as you walk. Once a full goal is completed, that dot gets bigger and switches to the Milestone Color — up to 10 milestones (10x goal) on the same ring.",
      },
      colorSelect("StepDefaultColor", "Dot Default Color", 8),
      colorSelect("StepCompletedColor", "Dot Completed Color", 4),
      colorSelect("MilestoneColor", "Milestone Color", 2),
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Clock Hands" },
      colorSelect("HourColor", "Hour Hand", 1),
      colorSelect("MinuteColor", "Minute Hand", 7),
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Bluetooth" },
      {
        type: "toggle",
        messageKey: "VibrateOnDisconnect",
        label: "Vibrate on Disconnect",
        defaultValue: true,
      },
      colorSelect("BtColor", "Connected Color", 7),
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Alarm & Events" },
      colorSelect("AlarmColor", "Alarm Dot", 7),
      colorSelect("EventColor", "Event Dot Color", 7),
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Notifications" },
      colorSelect("NotifNormalColor", "Normal Color", 7),
      colorSelect("NotifAlertColor", "Alert Color", 1),
      {
        type: "slider",
        messageKey: "NotifThreshold",
        label: "Alert Threshold",
        defaultValue: 5,
        min: 1,
        max: 20,
        step: 1,
      },
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Health" },
      colorSelect("HrColor", "Heart Rate Normal Color", 7),
      colorSelect("HrErrorColor", "Heart Rate Error Color", 6),
      {
        type: "text",
        defaultValue:
          "Heart Rate Thresholds (low to high). If a threshold is set below the one before it, the watch clamps it back up automatically.",
      },
      {
        type: "slider",
        messageKey: "HrAlertBpm",
        label: "Threshold 1 (BPM)",
        defaultValue: 100,
        min: 60,
        max: 220,
        step: 5,
      },
      colorSelect("HrAlertColor", "Threshold 1 Color", 5),
      {
        type: "slider",
        messageKey: "HrThreshold2Bpm",
        label: "Threshold 2 (BPM)",
        defaultValue: 130,
        min: 60,
        max: 220,
        step: 5,
      },
      colorSelect("HrThreshold2Color", "Threshold 2 Color", 0),
      {
        type: "slider",
        messageKey: "HrThreshold3Bpm",
        label: "Threshold 3 (BPM)",
        defaultValue: 150,
        min: 60,
        max: 220,
        step: 5,
      },
      colorSelect("HrThreshold3Color", "Threshold 3 Color", 1),
      {
        type: "slider",
        messageKey: "HrThreshold4Bpm",
        label: "Threshold 4 (BPM)",
        defaultValue: 170,
        min: 60,
        max: 220,
        step: 5,
      },
      colorSelect("HrThreshold4Color", "Threshold 4 Color", 10),
      colorSelect("ActivityColor", "Activity Dot Color", 2),
    ],
  },

  {
    type: "section",
    items: [
      { type: "heading", defaultValue: "Weekday Colors" },
      colorSelect("WeekdayColor0", "Sunday", 5),
      colorSelect("WeekdayColor1", "Monday", 8),
      colorSelect("WeekdayColor2", "Tuesday", 1),
      colorSelect("WeekdayColor3", "Wednesday", 2),
      colorSelect("WeekdayColor4", "Thursday", 0),
      colorSelect("WeekdayColor5", "Friday", 9),
      colorSelect("WeekdayColor6", "Saturday", 6),
    ],
  },

  { type: "submit", defaultValue: "Save" },
];

var clay = new Clay(clayConfig);
