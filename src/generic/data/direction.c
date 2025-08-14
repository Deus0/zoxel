#define direction_left 0
#define direction_right 1
#define direction_down 2
#define direction_up 3
#define direction_back 4
#define direction_front 5

static const char* direction_names[6] = {
    "left",   // 0
    "right",  // 1
    "down",   // 2
    "up",     // 3
    "back",   // 4
    "front"   // 5
};

byte reverse_direction(byte d) {
    switch (d) {
        case direction_left:  return direction_right;
        case direction_right: return direction_left;
        case direction_up:    return direction_down;
        case direction_down:  return direction_up;
        case direction_front: return direction_back;
        case direction_back:  return direction_front;
        default: return d; // or error
    }
}