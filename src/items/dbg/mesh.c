entity dbg_item_mesh = 0;
extern entity get_linked_character(ecs*, entity);

entity zox_dbg_spawn_item_mesh(
    ecs *world,
    entity player)
{
    float rotate_speed = 16;
    float distance = frand_range(1.0f, 1.6f);
    if (zox_valid(dbg_item_mesh)) {
        zox_log("Deleting [dbg_item_mesh]");
        zox_delete(dbg_item_mesh);
        dbg_item_mesh = 0;
        return 0;
    }
    if (!zox_valid(player)) {
        zox_loge("[dbg_item_mesh] Invalid [player]");
        return 0;
    }
    entity canvas = get_linked_canvas(world, player);
    entity character = get_linked_character(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(character)) {
        zox_loge("[dbg_item_mesh] Invalid [character]");
        return 0;
    }
    entity terrain = zox_get_link(world, character, TerrainLink);
    if (!zox_valid(terrain)) {
        zox_loge("[dbg_item_mesh] Invalid [terrain]");
        return 0;
    }
    entity realm = zox_get_link(world, terrain, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[dbg_item_mesh] Invalid [realm]");
        return 0;
    }
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    // = blocks->value[];
    uint count = 0;
    entity models[128];
    for (int i = 0; i < blocks->length; i++) {
        entity block2 = blocks->value[i];
        if (!zox_has(block2, BlockVox)) {
            continue;
        }
        entity model = zox_has(block2, ModelLink) ?
            zox_getv(block2, ModelLink) :
            0;
        if (model) {
            models[count++] = model;
        }
    }
    if (!count) {
        zox_loge("No models found in blocks.");
        return 0;
    }
    entity model = models[rand_range(0, count)];
    if (!model) {
        zox_loge("[zox_dbg_spawn_item_mesh] Invalid Item Model");
        return 0;
    }
    entity mesh = get_max_model_mesh(world, model);
    if (!mesh) {
        zox_loge("[dbg_item_mesh] Invalid Model [mesh]:[%s]",
            zox_getn(model));
        return 0;
    }
    if (!zox_has(mesh, Mesh)) {
        zox_loge("[dbg_item_mesh] Invalid [mesh] [%s]: No Mesh",
            zox_getn(mesh));
        return 0;
    }
    // float3 camera_position = zox_getv(camera, Position3D);
    entity mesh_clone = spawn_mesh3_clone(world, mesh);
    zox_add(mesh_clone, VoxMesh);
    zox_set_unique_name(mesh_clone, "block_item_mesh");
    zox_setv(mesh_clone, Position3D, spawn_position);
    zox_setv(mesh_clone, Rotation3D, quaternion_identity);
    add_eternal_euler(
        world,
        mesh_clone,
        (float3) { 0, rotate_speed, 0 });
    dbg_item_mesh = mesh_clone;
    inspect_entity(world, mesh_clone);
    zox_log("[dbg_item_mesh] Spawned Block Mesh");
    return mesh_clone;
}

void zox_dbg_spawn_item_mesh2(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    zox_dbg_spawn_item_mesh(
        world,
        player);
}

