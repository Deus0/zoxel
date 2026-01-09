// Use this to set ui_scale dynamically
byte calculate_ui_scale(int2 size) {
    int area = size.x * size.y;
    if (area <= 400 * 400)   return 1;
    if (area <= 800 * 800)   return 2;
    if (area <= 1200 * 1200) return 3;
    return 4;
}