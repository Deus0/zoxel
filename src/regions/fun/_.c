int2 tunk_position_to_region_position(int2 position) {
    position.x = floor_div(position.x, region_dividor);
    position.y = floor_div(position.y, region_dividor);
    return position;
}

int2 region_position_to_tunk_position(int2 position) {
    position.x *= region_dividor;
    position.y *= region_dividor;
    return position;
}

int2 region_position_to_block_position(int2 position, byte terrain_depth) {
    byte chunk_length = powers_of_two[terrain_depth];
    position.x *= chunk_length * region_dividor;
    position.y *= chunk_length * region_dividor;
    return position;
}

int2 block_position_to_region_position(int2 position, byte terrain_depth) {
    byte chunk_length = powers_of_two[terrain_depth];
    position.x = floor_div(position.x, chunk_length * region_dividor);
    position.y = floor_div(position.y, chunk_length * region_dividor);
    return position;
}
