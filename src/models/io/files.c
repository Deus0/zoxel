void dispose_vox_file(vox_file *vox) {
    if (vox->chunks) {
        // first free children voxels
        for (int i = 0; i < vox->pack.model_nums; i++) {
            if (vox->chunks[i].xyzi.voxels) {
                free(vox->chunks[i].xyzi.voxels);
            }
        }
        free(vox->chunks);    // children chunks
    }
    if (vox->palette.values) {
        free(vox->palette.values);  // pallete colorRGBs
    }
    if (vox->palette.values_rgb) {
        free(vox->palette.values_rgb);  // pallete colorRGBs
    }
}

// the depth vox files are scaled up to, the finest Block Res (a body part is sampled at its own depth, so a hat
//  made at depth 4 was half the body's size at Block Res 5)
#define vox_import_depth 5

// a vox file's import settings, from a .txt beside it (res/voxes/gear/top_hat.txt): "depth 4" (or "-depth 4") is
//  the depth it was made at, it's scaled up to vox_import_depth; 0 when there's none
byte read_vox_import_depth(const char* vox_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s", vox_path);
    char* dot = strrchr(path, '.');
    if (!dot || strlen(dot) < 4) {
        return 0;
    }
    snprintf(dot, path + sizeof(path) - dot, ".txt");
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }
    byte depth = 0;
    char word[32];
    int value;
    while (fscanf(file, "%31s", word) == 1) {
        const char* key = word[0] == '-' ? word + 1 : word;
        if (!strcmp(key, "depth") && fscanf(file, "%d", &value) == 1 && value > 0 && value <= vox_import_depth) {
            depth = (byte) value;
        }
    }
    fclose(file);
    return depth;
}

// scales a vox file's first model up, each voxel a cube of them
void scale_vox_file(vox_file* vox, byte scale) {
    if (scale <= 1 || !vox->chunks) {
        return;
    }
    const int3 size = vox->chunks[0].size.xyz;
    const int3 big = { size.x * scale, size.y * scale, size.z * scale };
    if (big.x > 255 || big.y > 255 || big.z > 255) {
        zox_logw("vox file too big to scale [%ix%ix%i] by [%i]", size.x, size.y, size.z, scale);
        return;
    }
    const byte* voxels = vox->chunks[0].xyzi.voxels;
    byte* scaled = malloc(big.x * big.y * big.z);
    for (int x = 0; x < big.x; x++) {
        for (int y = 0; y < big.y; y++) {
            for (int z = 0; z < big.z; z++) {
                const byte3 from = { x / scale, y / scale, z / scale };
                const byte3 to = { x, y, z };
                scaled[byte3_array_index(to, int3_to_byte3(big))] = voxels[byte3_array_index(from, int3_to_byte3(size))];
            }
        }
    }
    free(vox->chunks[0].xyzi.voxels);
    vox->chunks[0].xyzi.voxels = scaled;
    vox->chunks[0].size.xyz = big;
}

void load_files_voxes(ecs *world) {
    byte dbg_log = 0 || is_verbose;
    char* load_directory = concat_file_path(resources_path, directory_voxes);
    if (dbg_log) {
        zox_log("Loading Vox Files  [%s]", load_directory);
    }
    FileList files = get_files(load_directory, 0);
    free(load_directory);
    files_voxes_count = files.count;
    files_voxes = malloc(sizeof(entity) * files_voxes_count);
    files_hashmap_voxes = create_string_hashmap(files_voxes_count);
    if (dbg_log) {
        zox_log("   + Count [%i]", files.count);
    }
    for (int i = 0; i < files.count; i++) {
        char* filepath = files.files[i];
        char* filename = files.filenames[i];
        files_voxes[i] = 0;
        // only .vox files (a .txt beside one is its import settings)
        const size_t path_length = strlen(filepath);
        if (path_length < 4 || strcmp(filepath + path_length - 4, ".vox")) {
            continue;
        }
        if (dbg_log) {
            zox_log("   - [%i] [vox] [%s]", i, filepath);
        }
        vox_file data;
        if (read_vox(filepath, &data) == EXIT_FAILURE) {
            zox_logw("[%s] failed to load", filepath);
            continue;
        }
        if (!data.chunks) {
            zox_logw("[%s] failed to load properly", filepath);
            continue;
        }
        const byte made_depth = read_vox_import_depth(filepath);
        if (made_depth && made_depth < vox_import_depth) {
            scale_vox_file(&data, powers_of_two[vox_import_depth - made_depth]);
        }
        entity e = spawn_vox_file(world, &data, filename);
        dispose_vox_file(&data);
        if (e) {
            string_hashmap_add(files_hashmap_voxes, new_string_data_clone(filename), e);
            files_voxes[i] = e;
        }
    }
    free_files(&files);
}


void dispose_files_voxes(ecs* world) {
    zox_logv("Disposing [%i] [voxes]", files_hashmap_voxes->size);
    string_hashmap_dispose(files_hashmap_voxes);
    files_hashmap_voxes = NULL;
    free(files_voxes);
    files_voxes = NULL;
}
