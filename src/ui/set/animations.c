// byte game_overlay_layer = max_layers2D - 12; // -1
#define canvas_edge_size 8
color canvas_overlay_color = color_black;

static inline byte get_game_overlay_layer() {
    return max_layers2D - 16;
}