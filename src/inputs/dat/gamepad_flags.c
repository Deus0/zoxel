#define zox_btn_a 0
#define zox_btn_b 1
#define zox_btn_x 2
#define zox_btn_y 3
#define zox_btn_select 4
#define zox_btn_guide 5
#define zox_btn_start 6
#define zox_btn_left_stick_push 7
#define zox_btn_right_stick_push 8
#define zox_btn_lb 9
#define zox_btn_rb 10
#define zox_btn_dpad_up 11
#define zox_btn_dpad_down 12
#define zox_btn_dpad_left 13
#define zox_btn_dpad_right 14
#define zox_btn_lt 15
#define zox_btn_rt 16
#define zox_bmp_lt 0
#define zox_bmp_rt 1
#define zox_stk_left 0
#define zox_stk_right 1

byte is_dpad_button(const byte index) {
    return index >= zox_btn_dpad_down && index <= zox_btn_dpad_right;
}
