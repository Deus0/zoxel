// tst: model_views, spawns one of the realm's models up in the sky and looks at it from 4 viewports
//  - runs on the main menu: spawns and generates a realm of its own, no player or terrain
//  - for looking at a model: bash bsh/tst.sh model_views, then look at tst/model_views.ppm, or
//      tst/models/<model name>.ppm (kept per model, to compare them)
//      (top left +z, top right +x, bottom left -z, bottom right above at an angle)
//  - zox_tst_model=<name> picks the model whose realm entity's name holds it, i.e. zox_tst_model=sword
//      (the realm's models are logged), else the first one
//  - the 4 cameras draw straight to the screen on gray, each filtered (CameraFilter) to its own copy
//      of the model (CameraRenderer), so no sky; the menu's view is hidden (nohud and its render texture)
//  - checks: a screenshot before the model spawns and one after differ in the middle of every viewport
//      (fails if a viewport's camera didn't draw it, or one camera's clear wiped the others)
//  - render code it leans on: spawn_viewport_camera, camera_look_at, viewport_grid_cell, zox_gpu_clear_rect
extern char zox_screenshot_request[256];
extern byte nohud;
#define zox_tst_model_views_count 4
const char* zox_tst_model_views_empty_file = "model_views_empty.ppm";
const char* zox_tst_model_views_file = "model_views.ppm";
char zox_tst_model_views_model_file[192] = "";    // models/<name>.ppm, a copy kept per model
const char* zox_tst_model_views_names[zox_tst_model_views_count] = { "+z", "+x", "-z", "above" };
byte zox_tst_model_views_stage = 0;
int zox_tst_model_views_frames = 0;
entity zox_tst_model_views_cameras[zox_tst_model_views_count];
entity zox_tst_model_views_source = 0;     // the realm entity holding the model
entity zox_tst_model_views_mesh = 0;       // its model's mesh
entity zox_tst_model_views_clones[zox_tst_model_views_count];  // a copy of it for each view
entity zox_tst_model_views_realm = 0;
int zox_tst_model_views_models = 0;        // model meshes found last frame
int zox_tst_model_views_settled = 0;       // frames that count stayed the same
const float3 zox_tst_model_views_spot = { 0, 0, 0 };  // where it goes
const color zox_tst_model_views_background = { 96, 96, 96, 255 };

// a realm child's model mesh (a model group, a model, or something linking one), if it has one ready to copy
entity zox_tst_model_views_mesh_of(ecs* world, entity e) {
    const entity model = zox_has(e, ModelLink) ? zox_get_link(world, e, ModelLink) : e;
    const entity first = get_model_first(world, model);
    if (!zox_valid(first) || !zox_has(first, MaxRenderDepth) || !zox_has(first, ModelLods)) {
        return 0;
    }
    const entity mesh = get_max_model_mesh(world, model);
    if (!zox_valid(mesh) || !zox_has(mesh, Mesh) || !zox_has(mesh, MeshIndicies)) {
        return 0;
    }
    if (!zox_get(mesh, MeshIndicies)->length || !zox_get(mesh, MeshVertices)->length) {
        return 0;
    }
    return mesh;
}

// a model's file: models/<its entity name without the _<id> on the end>.ppm
void zox_tst_model_views_file_name(const char* name, char* out, size_t out_size) {
    char base[128];
    snprintf(base, sizeof(base), "%s", name);
    char* underscore = strrchr(base, '_');
    // unnamed_<n> keeps its number, it's how they're told apart
    if (underscore && underscore[1] && strncmp(base, "unnamed_", 8) != 0) {
        byte digits = 1;
        for (const char* c = underscore + 1; *c; c++) {
            digits &= *c >= '0' && *c <= '9';
        }
        if (digits) {
            *underscore = 0;
        }
    }
    snprintf(out, out_size, "models/%s.ppm", base);
}


// whether the realm has a model mesh named with it yet
byte zox_tst_model_views_has(ecs* world, entity realm, const char* wanted) {
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity child = it.entities[j];
            const char* name = zox_getn(child);
            if (name && strstr(name, wanted) && zox_tst_model_views_mesh_of(world, child)) {
                return 1;
            }
        }
    }
    return 0;
}

// picks the realm's model to look at, logs the choices
entity zox_tst_model_views_pick(ecs* world, entity realm, int* models_count, byte log) {
    const char* wanted = getenv("zox_tst_model");
    entity first = 0;
    entity picked = 0;
    int count = 0;
    char names[1024] = "";
    char first_name[128] = "";
    char picked_name[128] = "";
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity child = it.entities[j];
            if (!zox_tst_model_views_mesh_of(world, child)) {
                continue;
            }
            // unnamed ones (vox files) by their place, so zox_tst_model=unnamed_17 picks one
            char unnamed[32];
            const char* name = zox_getn(child);
            if (!name || !name[0]) {
                snprintf(unnamed, sizeof(unnamed), "unnamed_%i", count);
                name = unnamed;
            }
            count++;
            if (strlen(names) + strlen(name) + 3 < sizeof(names)) {
                strcat(names, " ");
                strcat(names, name);
            }
            if (!first) {
                first = child;
                snprintf(first_name, sizeof(first_name), "%s", name);
            }
            if (!picked && wanted && wanted[0] && strstr(name, wanted)) {
                picked = child;
                snprintf(picked_name, sizeof(picked_name), "%s", name);
            }
        }
    }
    *models_count = count;
    if (!count || !log) {
        return 0;
    }
    zox_log("[tst] realm models [%i]:%s", count, names);
    if (wanted && wanted[0] && !picked) {
        zox_logw("[tst] no realm model named [%s], looking at the first", wanted);
    }
    zox_tst_model_views_file_name(picked ? picked_name : first_name,
        zox_tst_model_views_model_file, sizeof(zox_tst_model_views_model_file));
    return picked ? picked : first;
}

// the middle and size of a mesh, from its vertices
void zox_tst_model_views_bounds(ecs* world, entity mesh, float3* centre, float* radius) {
    const MeshVertices* vertices = zox_get(mesh, MeshVertices);
    float3 lower = vertices->value[0];
    float3 upper = vertices->value[0];
    for (int i = 1; i < vertices->length; i++) {
        const float3 v = vertices->value[i];
        lower = (float3) { fminf(lower.x, v.x), fminf(lower.y, v.y), fminf(lower.z, v.z) };
        upper = (float3) { fmaxf(upper.x, v.x), fmaxf(upper.y, v.y), fmaxf(upper.z, v.z) };
    }
    *centre = float3_scale(float3_add(lower, upper), 0.5f);
    *radius = float3_length(float3_subtract(upper, lower)) * 0.5f;
    if (*radius < 0.01f) {
        *radius = 0.01f;
    }
}


void zox_tst_model_views_screenshot(const char* path) {
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", path);
}

byte zox_tst_model_views_ready(ecs* world) {
    zox_tst_model_views_frames++;
    zox_tst_progress(zox_tst_model_views_stage * 10000 + zox_tst_model_views_frames);
    // a realm of its own, generated on the main menu (no game, no terrain)
    if (zox_tst_model_views_stage == 0) {
        zox_tst_waiting = "the game and app to spawn a realm into";
        if (!zox_valid(local_game) || !zox_valid(main_app) || zox_tst_model_views_frames < 5) {
            return 0;
        }
        zox_tst_model_views_realm = spawn_realm(world, local_game, 666);
        zox_setv(zox_tst_model_views_realm, GenerateRealm, zox_generate_realm_start);
        zox_tst_model_views_stage = 1;
        zox_tst_model_views_frames = 0;
        return 0;
    }
    // pick a model and place the cameras around it
    if (zox_tst_model_views_stage == 1) {
        zox_tst_waiting = "the realm to generate, and its models' meshes";
        // generated (GenerateRealm counts back to 0), then no new model meshes for a while
        if (zox_getv(zox_tst_model_views_realm, GenerateRealm)) {
            return 0;
        }
        int count = 0;
        zox_tst_model_views_pick(world, zox_tst_model_views_realm, &count, 0);
        if (count != zox_tst_model_views_models) {
            zox_tst_model_views_models = count;
            zox_tst_model_views_settled = 0;
            return 0;
        }
        if (!count || ++zox_tst_model_views_settled < 30) {
            return 0;
        }
        // a model asked for by name may finish later than the rest (clothes), wait for it a while
        const char* wanted = getenv("zox_tst_model");
        if (wanted && wanted[0] && !zox_tst_model_views_has(world, zox_tst_model_views_realm, wanted) &&
            zox_tst_model_views_settled < 600) {
            zox_tst_waiting = "the model asked for by zox_tst_model";
            return 0;
        }
        zox_tst_model_views_source = zox_tst_model_views_pick(world, zox_tst_model_views_realm, &count, 1);
        zox_tst_model_views_mesh = zox_tst_model_views_mesh_of(world, zox_tst_model_views_source);
        const float3 spot = zox_tst_model_views_spot;
        float3 centre;
        float radius;
        zox_tst_model_views_bounds(world, zox_tst_model_views_mesh, &centre, &radius);
        const float3 target = float3_add(spot, centre);
        const float fov = 50;
        // far enough that its bounding sphere fits the view, with a margin
        const float distance = radius / sinf(fov * 0.5f * degreesToRadians) * 1.15f;
        const float3 directions[zox_tst_model_views_count] = {
            { 0, 0.25f, 1 },
            { 1, 0.25f, 0 },
            { 0, 0.25f, -1 },
            { 0.6f, 1, 0.6f } };
        for (int i = 0; i < zox_tst_model_views_count; i++) {
            const float3 position = float3_add(target, float3_scale(float3_normalize(directions[i]), distance));
            const entity camera = spawn_viewport_camera(
                world,
                main_app,
                position,
                camera_look_at(position, target),
                fov,
                viewport_grid_cell(i, 2, 2));
            // draws only what's linked to it (its copy of the model) on gray: no sky, menu or terrain
            zox_add(camera, CameraFilter);
            zox_setv(camera, Color, zox_tst_model_views_background);
            zox_tst_model_views_cameras[i] = camera;
        }
        // only the 4 views on the screen: no hud, no menu's view
        nohud = 1;
        const entity game_camera = main_cameras[0];
        const entity game_view = zox_valid(game_camera) ? zox_get_link(world, game_camera, RenderTextureLink) : 0;
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_log("[tst] looking at [%s] mesh [%s] radius [%.2f] from [%.2f] away",
            zox_getn(zox_tst_model_views_source), zox_getn(zox_tst_model_views_mesh), radius, distance);
        zox_tst_model_views_stage = 2;
        zox_tst_model_views_frames = 0;
        return 0;
    }
    // the empty views, to compare with
    if (zox_tst_model_views_stage == 2) {
        zox_tst_waiting = "the empty views' screenshot";
        if (zox_tst_model_views_frames == 10) {
            zox_tst_model_views_screenshot(zox_tst_path(zox_tst_model_views_empty_file));
        }
        if (zox_tst_model_views_frames <= 10 || zox_screenshot_request[0]) {
            return 0;
        }
        // a copy of the model for each view, filtered to its camera
        for (int i = 0; i < zox_tst_model_views_count; i++) {
            const entity clone = spawn_mesh3_clone(world, zox_tst_model_views_mesh);
            if (!clone) {
                zox_tst_check(0, "couldn't copy the mesh of [%s]", zox_getn(zox_tst_model_views_source));
                return 1;
            }
            zox_add(clone, VoxMesh);
            zox_set_unique_name(clone, "model_views_model");
            zox_setv(clone, Position3D, zox_tst_model_views_spot);
            zox_setv(clone, Rotation3D, quaternion_identity);
            zox_setv(clone, Scale1, 1);
            zox_link(world, clone, CameraRenderer, zox_tst_model_views_cameras[i]);
            zox_tst_model_views_clones[i] = clone;
        }
        zox_tst_model_views_stage = 3;
        zox_tst_model_views_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the model views' screenshot";
    if (zox_tst_model_views_stage == 3) {
        if (zox_tst_model_views_frames < 10) {
            return 0;
        }
        zox_tst_model_views_screenshot(zox_tst_path(zox_tst_model_views_file));
        zox_tst_model_views_stage = 4;
        return 0;
    }
    // saved at the end of the frame it was requested in
    return !zox_screenshot_request[0];
}

void zox_tst_model_views_check(ecs* world) {
    for (int i = 0; i < zox_tst_model_views_count; i++) {
        zox_tst_check(zox_valid(zox_tst_model_views_clones[i]), "view [%s]'s model wasn't spawned", zox_tst_model_views_names[i]);
        const entity camera = zox_tst_model_views_cameras[i];
        zox_tst_check(zox_valid(camera) && zox_is_linked(world, main_app, ViewportCamera, camera),
            "view [%s]'s camera isn't a viewport of the app", zox_tst_model_views_names[i]);
    }
    int2 size_empty;
    int2 size;
    byte* empty = zox_tst_load_ppm(zox_tst_path(zox_tst_model_views_empty_file), &size_empty);
    byte* pixels = zox_tst_load_ppm(zox_tst_path(zox_tst_model_views_file), &size);
    zox_tst_check(empty, "no empty views screenshot [%s]", zox_tst_path(zox_tst_model_views_empty_file));
    zox_tst_check(pixels, "no model views screenshot [%s]", zox_tst_path(zox_tst_model_views_file));
    // kept by the model's name too
    if (pixels) {
        const byte copied = zox_tst_copy_file(
            zox_tst_path(zox_tst_model_views_file),
            zox_tst_path(zox_tst_model_views_model_file));
        zox_tst_check(copied, "couldn't save [%s]", zox_tst_path(zox_tst_model_views_model_file));
        if (copied) {
            zox_log("[tst] saved [tst/%s]", zox_tst_model_views_model_file);
        }
    }
    if (empty && pixels && int2_equals(size, size_empty)) {
        // the middle half of each view: the model shows there, so pixels changed when it spawned
        //  (the screenshot's rows start at the top, as does viewport_grid_cell's index 0)
        const int2 half = { size.x / 2, size.y / 2 };
        for (int i = 0; i < zox_tst_model_views_count; i++) {
            const int2 corner = { (i % 2) * half.x, (i / 2) * half.y };
            int changed = 0;
            int total = 0;
            for (int y = corner.y + half.y / 4; y < corner.y + half.y * 3 / 4; y++) {
                for (int x = corner.x + half.x / 4; x < corner.x + half.x * 3 / 4; x++) {
                    const size_t p = ((size_t) y * size.x + x) * 3;
                    const int difference = abs(pixels[p] - empty[p]) + abs(pixels[p + 1] - empty[p + 1]) + abs(pixels[p + 2] - empty[p + 2]);
                    changed += difference > 24;
                    total++;
                }
            }
            const float share = total ? changed / (float) total : 0;
            zox_log("[tst] view [%s]: [%.1f%%] of its middle changed with the model", zox_tst_model_views_names[i], share * 100);
            zox_tst_check(share > 0.02f, "view [%s] didn't draw the model, [%.2f%%] of its middle changed",
                zox_tst_model_views_names[i], share * 100);
        }
    } else if (empty && pixels) {
        zox_tst_check(0, "screenshots differ in size [%ix%i] and [%ix%i]", size_empty.x, size_empty.y, size.x, size.y);
    }
    free(empty);
    free(pixels);
}
