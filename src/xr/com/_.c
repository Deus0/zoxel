// XR controllers (fun/controller.c): a device a hand
#define xr_hand_left 0
#define xr_hand_right 1
#define xr_trigger_cutoff 0.75f
zox_tag(XrController);
zoxc_byte(XrHand);
// the xr camera's culling angles (FieldOfViewAngles) before its eyes are known, wide (fun/cull.c)
#define xr_cull_angles_default (float4) { -1.0f, 1.0f, -1.0f, 1.0f }
byte xr_recentred = 0;      // the tracking space was recentred (a headset's event, openxr/frames.c): the body takes it up

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingXRHeight);
