#define direction_left 0
#define direction_right 1
#define direction_down 2
#define direction_up 3
#define direction_back 4
#define direction_front 5

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

static inline byte normal_to_direction(float3 normal) {
    if (float3_equals(normal, float3_left)) {
        return direction_left;
    } else if (float3_equals(normal, float3_right)) {
        return direction_right;
    } else if (float3_equals(normal, float3_down)) {
        return direction_down;
    } else if (float3_equals(normal, float3_up)) {
        return direction_up;
    } else if (float3_equals(normal, float3_backward)) {
        return direction_back;
    } else if (float3_equals(normal, float3_forward)) {
        return direction_front;
    } else {
        return 0;
    }
}


static inline int3 reverse_position(int3 position, byte d, byte length) {
    if (d == direction_left) {
        position.x = length - 1;
    } else if (d == direction_right) {
        position.x = 0;
    } else if (d == direction_down) {
        position.y = length - 1;
    } else if (d == direction_up) {
        position.y = 0;
    } else if (d == direction_back) {
        position.z = length - 1;
    } else if (d == direction_front) {
        position.z = 0;
    }
    return position;
}

static inline int3 move_position(int3 position, byte dir) {
    if (dir == direction_left) {
        position.x--;
    } else if (dir == direction_right) {
        position.x++;
    } else if (dir == direction_down) {
        position.y--;
    } else if (dir == direction_up) {
        position.y++;
    } else if (dir == direction_back) {
        position.z--;
    } else if (dir == direction_front) {
        position.z++;
    }
    return position;
}
