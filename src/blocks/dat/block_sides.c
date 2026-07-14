/*#define direction_left 0
#define direction_right 1
#define direction_down 2
#define direction_up 3
#define direction_back 4
#define direction_front 5
#define direction_none 6
#define direction_end 6*/

byte byte3_on_edge_axis(const byte3 pos, const byte3 size, const byte axis) {
    return (axis == direction_left && pos.x == 0) ||
        (axis == direction_down && pos.y == 0) ||
        (axis == direction_back && pos.z == 0) ||
        (axis == direction_right && pos.x == size.x - 1) ||
        (axis == direction_up && pos.y == size.y - 1) ||
        (axis == direction_front && pos.z == size.z - 1);
}