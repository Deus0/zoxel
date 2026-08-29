// NOTE: Our Zixel Mesh - Character2Ds
guint2 global_mesh2;
guint global_mesh_uvs;

const int square_indicies[] = {
    2, 1, 0,
    0, 3, 2
};

const float square_mesh_verts2[] = {
    -global_mesh_size,      -global_mesh_size,
     -global_mesh_size,     global_mesh_size,
     global_mesh_size,      global_mesh_size,
    global_mesh_size,       -global_mesh_size
};

const float square_mesh_uvs[] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1,
};


void dispose_square_mesh() {
    zox_gpu_dispose_buffer(global_mesh2.x);
    zox_gpu_dispose_buffer(global_mesh2.y);
    zox_gpu_dispose_buffer(global_mesh_uvs);
}

void initialize_square_mesh_textured() {
    global_mesh2 = (guint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
    global_mesh_uvs = zox_gpu_create_buffer();
    // set indicies
    zox_gpu_bind_buffer_element(global_mesh2.x);
    zox_gpu_set_buffer_element(square_indicies, sizeof(square_indicies));
    zox_gpu_bind_buffer_element(0);
    // set verts
    zox_gpu_bind_buffer_array(global_mesh2.y);
    zox_gpu_set_buffer_array(square_mesh_verts2, sizeof(square_mesh_verts2));
    zox_gpu_bind_buffer_array(0);
    // set uvs
    zox_gpu_bind_buffer_array(global_mesh_uvs);
    zox_gpu_set_buffer_array(square_mesh_uvs, sizeof(square_mesh_uvs));
    zox_gpu_bind_buffer_array(0);
}
