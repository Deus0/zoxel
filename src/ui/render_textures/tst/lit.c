// tst: lit_mesh, a lit mesh (colored3D_lit) is shaded by the sun, a face at a time (normals from the screen)
//  - here (ui) to wait for the screen fader, as eyelids and hdr do: views taken while it faded in were dimmed
//  - on the menus, no realm: the menu's camera in a void (no skybox, a black clear), two grey quads 2m in front of
//    it: the left one faces the camera, the right one turned 45 degrees towards it (filling the right half, hiding
//    the menu's cube), its normal (-0.71, 0, 0.71) in the camera's space
//  - the sun from behind the camera, a little to the right (0.5, 0, 1), ambient 0.2, sun 0.8: the left face lit
//    0.2 + 0.8 * 0.89 = 0.92, the right 0.2 + 0.8 * 0.32 = 0.45
//  - then the sun from in front (behind the quads): both faces ambient only (0.2), much darker
//  - checks: both drawn, the left brighter than the right in the sun, both darker with the sun behind them
//  - run: bash bsh/tst.sh lit_mesh (and --vulkan: the same checks on the other backend)
const char* zox_tst_lit_files[2] = { "lit_mesh_sun.ppm", "lit_mesh_shade.ppm" };
byte zox_tst_lit_stage = 0;
int zox_tst_lit_frames = 0;
entity zox_tst_lit_mesh = 0;

// a quad's two triangles from its corners (counterclockwise from its front), each vertex its own: flat shaded
static inline void zox_tst_lit_quad(MeshVertices* vertices, MeshIndicies* indices, MeshColors* colors, int at, float3 a, float3 b,
    float3 c, float3 d) {
    const float3 corners[6] = { a, b, c, a, c, d };
    for (int i = 0; i < 6; i++) {
        vertices->value[at + i] = corners[i];
        indices->value[at + i] = at + i;
        colors->value[at + i] = (color) { 160, 160, 160, 0 };
    }
}

// the quads in the camera's space (its rotation), 2m in front of it; the sun from behind it
static inline void zox_tst_lit_spawn(ecs* world, entity camera) {
    MeshVertices vertices = { 0 };
    MeshIndicies indices = { 0 };
    MeshColors colors = { 0 };
    initialize_MeshVertices(&vertices, 12);
    initialize_MeshIndicies(&indices, 12);
    initialize_MeshColors(&colors, 12);
    zox_tst_lit_quad(&vertices, &indices, &colors, 0,
        (float3) { -1.6f, -1.2f, 0 }, (float3) { -0.05f, -1.2f, 0 }, (float3) { -0.05f, 1.2f, 0 }, (float3) { -1.6f, 1.2f, 0 });
    // hinged at x 0.05, turned 45 degrees towards the camera: its normal (-0.71, 0, 0.71)
    const float3 far = { 0.05f + 1.6f * 0.707f, 0, 1.6f * 0.707f };
    zox_tst_lit_quad(&vertices, &indices, &colors, 6,
        (float3) { 0.05f, -1.2f, 0 }, (float3) { far.x, -1.2f, far.z }, (float3) { far.x, 1.2f, far.z }, (float3) { 0.05f, 1.2f, 0 });
    const float4 rotation = zox_getv(camera, Rotation3D);
    const float3 position = float3_add(zox_getv(camera, Position3D), float4_rotate_float3(rotation, (float3) { 0, 0, -2 }));
    const entity e = spawn_lit_mesh(world, 0, position, vertices, indices, colors);
    zox_setv(e, Rotation3D, rotation);
    zox_tst_lit_mesh = e;
}

byte zox_tst_lit_mesh_ready(ecs* world) {
    const entity camera = main_cameras[0];
    if (!zox_valid(camera) || !zox_valid(material_colored3D_lit)) {
        zox_tst_waiting = "the menu's camera and the lit material";
        return 0;
    }
    const float4 rotation = zox_getv(camera, Rotation3D);
    if (!zox_tst_lit_mesh) {
        zox_tst_lids_make_void(world, camera);
        zox_setv(camera, Color, ((color) { 0, 0, 0, 255 }));
        zox_tst_lit_spawn(world, camera);
        render_ambient_light = (float3) { 0.2f, 0.2f, 0.2f };
        render_sun_light = (float3) { 0.8f, 0.8f, 0.8f };
    }
    zox_tst_waiting = "the screen fader to fade out";
    byte fading = 0;
    iter it = zox_each(world, CanvasOverlay);
    while (zox_each_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity overlay = it.entities[j];
            fading |= !zox_is_prefab(overlay) && zox_has(overlay, Alpha) && zox_getv(overlay, Alpha) > 0.01f;
        }
    }
    if (fading) {
        return 0;
    }
    zox_tst_waiting = "the lit mesh to upload";
    if (!zox_getv(zox_tst_lit_mesh, MeshRenderCount)) {
        return 0;
    }
    zox_tst_waiting = "the screenshots";
    zox_tst_lit_frames++;
    zox_tst_progress(zox_tst_lit_stage * 1000 + zox_tst_lit_frames);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_lit_stage >= 2) {
        return 1;
    }
    // the sun from behind the camera, then from in front of it; a few frames drawn with it, then saved
    render_sun_direction = float4_rotate_float3(rotation, zox_tst_lit_stage == 0 ? (float3) { 0.5f, 0, 1 } : (float3) { 0, 0, -1 });
    if (zox_tst_lit_frames < 10) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(zox_tst_lit_files[zox_tst_lit_stage]));
    zox_tst_lit_stage++;
    zox_tst_lit_frames = 0;
    return 0;
}

// a spot's brightness, r + g + b averaged over 5 x 5 pixels, x and y 0 to 1 from the top left
static inline int zox_tst_lit_at(const byte* pixels, int2 size, float x, float y) {
    const int cx = (int) (x * (size.x - 1)), cy = (int) (y * (size.y - 1));
    int sum = 0;
    for (int i = -2; i <= 2; i++) for (int j = -2; j <= 2; j++) {
        const byte* p = zox_tst_ppm_pixel(pixels, size, int_max(0, int_min(size.x - 1, cx + i)), int_max(0, int_min(size.y - 1, cy + j)));
        sum += p[0] + p[1] + p[2];
    }
    return sum / 25;
}

void zox_tst_lit_mesh_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_lit_stage == 2, "it stopped at stage [%i]", zox_tst_lit_stage);
    int2 sizes[2];
    byte* images[2];
    for (byte i = 0; i < 2; i++) {
        images[i] = zox_tst_load_ppm(zox_tst_path(zox_tst_lit_files[i]), &sizes[i]);
        zox_tst_check(images[i], "[%s] wasn't saved", zox_tst_lit_files[i]);
    }
    if (images[0] && images[1]) {
        // spots clear of the menu's title band (0.44 to 0.56 from the top)
        const int left = zox_tst_lit_at(images[0], sizes[0], 0.25f, 0.28f);
        const int right = zox_tst_lit_at(images[0], sizes[0], 0.7f, 0.28f);
        const int left_shade = zox_tst_lit_at(images[1], sizes[1], 0.25f, 0.28f);
        const int right_shade = zox_tst_lit_at(images[1], sizes[1], 0.7f, 0.28f);
        zox_log("[tst] sun: left [%i] right [%i], sun behind: left [%i] right [%i]", left, right, left_shade, right_shade);
        zox_tst_check(left_shade > 15 && right_shade > 15, "a quad isn't drawn [%i %i]", left_shade, right_shade);
        zox_tst_check(left > right + 30, "the face to the sun [%i] isn't brighter than the one turned away [%i]", left, right);
        zox_tst_check(right > right_shade + 30, "the turned face lit [%i] isn't brighter than ambient [%i]", right, right_shade);
        zox_tst_check(left > left_shade * 2, "the face to the sun [%i] isn't much brighter than ambient [%i]", left, left_shade);
    }
    for (byte i = 0; i < 2; i++) {
        free(images[i]);
    }
}

void zox_tsts_lit_mesh(ecs* world) {
    (void) world;
    zox_tst_add("lit_mesh", zox_tst_lit_mesh_ready, zox_tst_lit_mesh_check, 30);
    zox_tst_set_setup("lit_mesh", NULL, 1);
}
