// frames
const int default_frame_size = 64;
const color default_outline_color_frame = { 0, 0, 0, 200 };
const color default_fill_color_frame = { 6, 55, 55, 122 };

// nothing uses these? wait taskbar?
const int default_icon_size = 48; // 64; // 48;
const color default_outline_color_icon = { 0, 122, 66, 166 };
const color default_fill_color_icon = { 22, 44, 66, 122 };
const float default_icon_radius = 0.569f; // 16 outt  of 48 originally
const int default_icon_texture_size = 32; // 32

// blue + red = purple
#define frame_alpha 188
#define window_alpha 82
const color default_fill_color_frame_item = { 6, 6, 77, frame_alpha };
const color default_fill_color_frame_skill = { 77, 6, 6, frame_alpha };
const color default_fill_color_frame_stat = { 33, 33, 33, frame_alpha };