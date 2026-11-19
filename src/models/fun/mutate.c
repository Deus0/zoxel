// a colour mutated (a mutate node, eve/mutate.c), range: hue turn (degrees), saturation added (0 to 100), value change
//  (a share either way)
//  - turn: the model's hue turn, the same for all its colours (one look); tint: the hue its greys take
//  - each colour its own saturation and value from seed: whites tint (a cream or a blue chook), blacks stay dark
color_rgb mutate_color(color_rgb c, uint seed, float turn, float tint, float3 range) {
    float3 hsv = color_rgb_to_hsv(c);
    hsv.x = hsv.y < 10 ? tint : zox_fmod(hsv.x + turn + 360, 360);
    hsv.y = fminf(100, hsv.y + seed_rangef(seed, 0, range.y));
    hsv.z = fminf(100, hsv.z * (1 + seed_rangef(seed + 1, -range.z, range.z)));
    return hsv_to_color_rgb(hsv);
}
