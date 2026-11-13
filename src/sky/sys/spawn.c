// Clouds: a field of blocky boxes above the terrain, children of the realm (they go with it)
//  - cells of 4 voxels, so they look built from blocks, 2 to 4 boxes a cloud, smaller ones stacked on top a little offset
//  - from the realm's seed, the same realm has the same clouds
const int clouds_count = 56;
const float clouds_field = 320;         // half the field's width, they wrap past it (the fog hides the edge)
const float clouds_height = 160;        // voxels up, above the hills
const byte cloud_cell = 4;              // voxels a cell, the blockiness

entity spawn_cloud(ecs* world, entity realm, float3 position, float3 size) {
    const entity e = spawn_cube_with_shader(world, prefab_cloud, position, 1, shader_cloud);
    if (!e) {
        return 0;
    }
    zox_set_parent(world, e, realm);
    // the cube's vertices are a unit box (default_cube_size is half), stretched to the cloud's size
    float3 vertices[cube2_vertices_length];
    for (int i = 0; i < cube2_vertices_length; i++) {
        vertices[i] = float3_mul(cube2_vertices[i], size);
    }
    prefab_set_mesh_vertices_float3(world, e, vertices, cube2_vertices_length);
    return e;
}

void spawn_clouds(ecs* world, entity realm, float3 centre, float block_scale, uint seed) {
    const float cell = cloud_cell * block_scale;
    for (int i = 0; i < clouds_count; i++) {
        const uint s = seed_rand(seed + i * 7919);
        float3 position = {
            centre.x + seed_rangef(s, -clouds_field, clouds_field),
            (clouds_height + seed_range(s + 1, 0, 3) * cloud_cell) * block_scale,
            centre.z + seed_rangef(s + 2, -clouds_field, clouds_field)
        };
        position.x = floorf(position.x / cell) * cell;
        position.z = floorf(position.z / cell) * cell;
        float3 size = {
            seed_range(s + 3, 6, 20) * cell,
            seed_range(s + 4, 2, 4) * cell,
            seed_range(s + 5, 5, 16) * cell
        };
        spawn_cloud(world, realm, position, size);
        // smaller boxes on top, offset, a lumpier cloud
        const int boxes = seed_range(s + 6, 1, 4);
        for (int j = 0; j < boxes; j++) {
            const uint s2 = seed_rand(s + 11 * (j + 1));
            size.x = floorf(size.x * seed_rangef(s2, 0.4f, 0.8f) / cell) * cell + cell;
            size.z = floorf(size.z * seed_rangef(s2 + 1, 0.4f, 0.8f) / cell) * cell + cell;
            position.x += seed_range(s2 + 2, -2, 3) * cell;
            position.z += seed_range(s2 + 3, -2, 3) * cell;
            position.y += cell;
            spawn_cloud(world, realm, position, size);
        }
    }
}

// main thread (spawns): the listening camera's realm gets its clouds once, then it moves the realm's
//  CloudCentre with the camera each frame (the drift wraps round it)
void clouds_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        const entity terrain = zox_get_link(world, e, StreamLink);
        const entity realm = zox_valid(terrain) ? zox_get_link(world, terrain, RealmLink) : 0;
        if (!zox_valid(realm) || zox_getv(realm, GenerateRealm) != zox_generate_realm_none) {
            continue;
        }
        CloudCentre* centre = zox_get_mut(realm, CloudCentre);
        if (centre) {
            centre->value = position->value;
            continue;
        }
        if (!zox_has(terrain, BlockScale)) {
            continue;
        }
        spawn_clouds(world, realm, position->value, zox_getv(terrain, BlockScale), (uint) zox_getv(realm, Seed));
        zox_setv(realm, CloudCentre, position->value);
    }
    zox_sys_on_end();
} zoxd_system(clouds_spawn_system);
