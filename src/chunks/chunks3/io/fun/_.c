bool file_exists(const char *path) {
    struct stat buffer;
    return stat(path, &buffer) == 0;
}

void get_chunk_filename(char* out, const int3 position) {
    sprintf(out, "chunk_%i_%i_%i.dat", position.x, position.y, position.z);
}