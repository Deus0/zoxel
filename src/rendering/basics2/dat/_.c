// NOTE: Our Zixel Mesh - Character2Ds
uint2 global_mesh2;
uint global_mesh_uvs;

// TODO: Refactor global mesh by linking the entity to one instead


/*const int square_indicies[] = {
    2, 1, 0,    0, 3, 2
};

const float square_mesh_verts[] = {
    -global_mesh_size, -global_mesh_size,       0, 0,   // top right
    -global_mesh_size, global_mesh_size,        1, 0,   // bottom right
    global_mesh_size, global_mesh_size,         1, 1,   // bottom left
    global_mesh_size,  -global_mesh_size,       0, 1    // top left
};*/

/*const float square_mesh_verts[] = {
    // x, y                      u, v
    -global_mesh_size,  global_mesh_size,   0, 1,  // top-left
     global_mesh_size,  global_mesh_size,   1, 1,  // top-right
     global_mesh_size, -global_mesh_size,   1, 0,  // bottom-right
    -global_mesh_size, -global_mesh_size,   0, 0   // bottom-left
};*/

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
    global_mesh2 = (uint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
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



    // global_mesh_uvs = zox_gpu_create_buffer();

    // glGenBuffers(1, &global_mesh2.x);
    // glGenBuffers(1, &global_mesh2.y);  // generate a new VBO and get the associated ID
    // glGenBuffers(1, &global_mesh_uvs);  // generate a new VBO and get the associated ID
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indicies), square_indicies, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(square_mesh_verts), square_mesh_verts, GL_STATIC_DRAW);
}