const int2 crosshair_pixel_size = { 16, 16 };
const int2 crosshair_texture_size = { 8, 8 };
const byte crosshair_thickness = 3;
const byte crosshair_corner = 1;
#define crosshair_alpha 232
const color crosshair_air = { 11, 11, 222, crosshair_alpha };
const color crosshair_terrain = { 11, 222, 11, crosshair_alpha };
const color crosshair_target = { 222, 11, 11, crosshair_alpha };
