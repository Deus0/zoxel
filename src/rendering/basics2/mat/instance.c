uint2 shader2D_basic;
uint square2DMaterial;
Material2D material2D;
uint2 squareMesh;

void dispose_shader2D_instance_material() {
    zox_gpu_dispose_shader(shader2D_basic.x);
    zox_gpu_dispose_shader(shader2D_basic.y);
    zox_gpu_dispose_buffer(squareMesh.x);
    zox_gpu_dispose_buffer(squareMesh.y);
    zox_dispose_material(square2DMaterial);
}

void initialize_mesh() {
    squareMesh = (uint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
    //glGenBuffers(1, &squareMesh.x);
    //glGenBuffers(1, &squareMesh.y);

    zox_gpu_bind_buffer_element(squareMesh.x);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indicies), square_indicies, GL_STATIC_DRAW);
    zox_gpu_set_buffer_element(square_indicies, sizeof(square_indicies));

    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(squareMesh.y);
    zox_gpu_set_buffer_array(square_vertices, sizeof(squareTexturedVerts));
    // glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    zox_gpu_enable_attribute_float2(material2D.vertex_position);
    //glEnableVertexAttribArray(material2D.vertex_position);
    //glVertexAttribPointer(material2D.vertex_position, 2, GL_FLOAT, GL_FALSE, 8, 0);

    zox_gpu_bind_buffer_array(0);
}

int load_instance2D_material(ecs *world) {

    char* vert = get_shader_source(world, "basic2.vert");
    char* frag = get_shader_source(world, "basic2.frag");
    shader2D_basic = zox_gpu_compile_shader(vert, frag);

    if (uint2_equals(shader2D_basic, uint2_zero)) {
        zox_log_error("shader2D_basic has failed")
        return EXIT_FAILURE;
    }

    square2DMaterial = spawn_gpu_material_program((const uint2) { shader2D_basic.x, shader2D_basic.y });
    if (!square2DMaterial) {
        zox_log_error("=> [load_instance2D_material] Failed:\nVert Shader:\n%s\nFrag Shader:\n%s", vert, frag);
        return EXIT_FAILURE;
    }

    initialize_material2D_properties(&material2D, square2DMaterial);

    initialize_mesh();

    return EXIT_SUCCESS;
}

void shader2D_instance_begin(const float4x4 viewMatrix) {
    if (square2DMaterial == 0) {
        return;
    }
    //! This sets the materials actually, would be best to group entities per material here?
    zox_gpu_material(square2DMaterial);
    zox_gpu_bind_buffer_element(squareMesh.x);    // for indices
    zox_gpu_bind_buffer_array(squareMesh.y);            // for vertex coordinates

    zox_gpu_enable_attribute_float2(material2D.vertex_position);
    //glEnableVertexAttribArray(material2D.vertex_position);
    //glVertexAttribPointer(material2D.vertex_position, 2, GL_FLOAT, GL_FALSE, 8, 0);  // 2 * 4

    zox_gpu_float4x4(material2D.camera_matrix, viewMatrix);
}

// does this per material!
void render_instance2D(float2 position, float angle, float scale, float brightness) {
    // set variables, can this be done using a filtered / system ?
    glUniform3f(material2D.position, position.x, position.y, 0);
    glUniform1f(material2D.scale, scale);
    glUniform1f(material2D.angle, angle);
    glUniform1f(material2D.brightness, brightness);
    zox_gpu_render(6);
}

void shader2D_instance_end() {
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
    zox_disable_material();
}
