entity local_crosshair;
const int2 crosshair_pixel_size = { 32, 32 };
const int2 crosshair_texture_size = { 8, 8 };
const byte crosshair_thickness = 4;
const byte crosshair_corner = 1;
#define crosshair_alpha 144
const color crosshair_air = { 44, 44, 44, crosshair_alpha };
const color crosshair_terrain = { 44, 122, 122, crosshair_alpha };
const color crosshair_target = { 144, 11, 11, crosshair_alpha };