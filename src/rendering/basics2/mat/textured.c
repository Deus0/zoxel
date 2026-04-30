entity shader_textured2D = 0;
entity material_textured2D = 0;

typedef struct {
    gint vertex_position;
    gint vertex_uv;
    uint position;
    uint angle;
    uint scale;
    uint camera_matrix;
    uint texture;
    uint brightness;
    uint alpha;
} MaterialTextured2D;
zoxc_custom(MaterialTextured2D);

MaterialTextured2D create_MaterialTextured2D(uint material) {
    return (MaterialTextured2D) {
        glGetAttribLocation(material, "vertex_position"),
        glGetAttribLocation(material, "vertex_uv"),
        glGetUniformLocation(material, "position"),
        glGetUniformLocation(material, "angle"),
        glGetUniformLocation(material, "scale"),
        glGetUniformLocation(material, "camera_matrix"),
        glGetUniformLocation(material, "texture"),
        glGetUniformLocation(material, "brightness"),
        glGetUniformLocation(material, "alpha") };
}

entity spawn_shader_textured2D(ecs *world) {
    byte shader_index = get_new_shader_source_index();

    char* vert = get_shader_source(world, "textured2D.vert");
    char* frag = get_shader_source(world, "textured2D.frag");

    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;

    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("=> [spawn_shader_textured2D] Failed:\n%s", vert);
        return 0;
    }

    zox_name("shader_textured2D");

    return e;
}

entity spawn_material_textured2D(ecs *world) {

    entity shader = spawn_shader_textured2D(world);
    if (!shader) {
        return 0;
    }

    uint material;
    entity e = spawn_material(world, shader, &material);

    if (!e) {
        return 0;
    }

    zox_set(e, ShaderLink, { shader });

    MaterialTextured2D attributes = create_MaterialTextured2D(material);
    zox_set_data(e, MaterialTextured2D, attributes)
    material_textured2D = e;

    shader_textured2D = shader;
    return e;
}

// todo: refactor this into entity, and just link other ones to it for mesh, MeshLink?
uint2 squareTexturedMesh;
uint squareTexturedModelUVs;

void dispose_square_mesh() {
    zox_gpu_dispose_buffer(squareTexturedMesh.x);
    zox_gpu_dispose_buffer(squareTexturedMesh.y);
    zox_gpu_dispose_buffer(squareTexturedModelUVs);
}

void initialize_square_mesh_textured() {
    glGenBuffers(1, &squareTexturedMesh.x);
    glGenBuffers(1, &squareTexturedMesh.y);  // generate a new VBO and get the associated ID
    glGenBuffers(1, &squareTexturedModelUVs);  // generate a new VBO and get the associated ID

    zox_gpu_bind_buffer_element(squareTexturedMesh.x);
    zox_gpu_bind_buffer_array(squareTexturedMesh.y);

    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indicies), square_indicies, GL_STATIC_DRAW);
    zox_gpu_set_buffer_element(square_indicies, sizeof(square_indicies));
    // glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexturedVerts), squareTexturedVerts, GL_STATIC_DRAW);
    zox_gpu_set_buffer_array(squareTexturedVerts, sizeof(squareTexturedVerts));

    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
}
