void generate_texture_graybox(color* data, int2 big_size, int2 position, int2 size) {
    for (int j = position.x; j < position.x + size.x; j++) {
        for (int k = position.y; k < position.y + size.y; k++) {
            int index = j + k * big_size.x;
            if (j == position.x || k == position.y || j == position.x + size.x - 1 || k == position.y + size.y - 1) {
                data[index].r = 0;
                data[index].g = 0;
                data[index].b = 0;
            } else {
                data[index].r = 125;
                data[index].g = 125;
                data[index].b = 125;
            }
        }
    }
}
