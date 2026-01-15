static const int3 int3_zero     = { 0, 0, 0 };
static const int3 int3_chaos    = { -666, -666, -666 };
static const int3 int3_left     = { -1, 0, 0 };
static const int3 int3_right    = {  1, 0, 0 };
static const int3 int3_down     = {  0, -1, 0 };
static const int3 int3_up       = {  0, 1, 0 };
static const int3 int3_backward = {  0, 0, -1 };
static const int3 int3_forward  = {  0, 0, 1 };

static inline int3 get_direction_int3(byte i) {
    if (i >= 6) return int3_zero;
    const int3 directions[] = {
        int3_left,
        int3_right,
        int3_down,
        int3_up,
        int3_backward,
        int3_forward
    };
    return directions[i];
}


static const int2 int2_left_     = { -1, 0 };
static const int2 int2_right_    = {  1, 0 };
static const int2 int2_backward_ = {  0, -1 };
static const int2 int2_forward_  = {  0, 1 };

static inline int2 get_direction_int2(byte i) {
    if (i >= 4) return int2_zero;
    const int2 directions[] = {
        int2_left_,
        int2_right_,
        int2_backward_,
        int2_forward_
    };
    return directions[i];
}