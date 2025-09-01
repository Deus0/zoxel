// returns 1 if bit at index is set, else 0
byte get_bit(byte value, byte index) {
    return (value >> index) & 1;
}

// sets or clears bit at index
void set_bit(byte *value, byte index, byte new_state) {
    if (new_state) {
        *value |= (1 << index);
    } else {
        *value &= ~(1 << index);
    }
}

/*

byte state = 0;
// set bit 0 (value becomes 00000001)
set_bit(&state, 0, 1);

// query bit 0 -> 1
byte pressed = get_bit(state, 0);

*/