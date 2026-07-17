const color empty_color = { 0, 0, 0, 0 };

void generate_texture_fill(color* data, int2 size, color fill_color) {
    int2 position;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            data[int2_array_index(position, size)] = fill_color;
        }
    }
}
