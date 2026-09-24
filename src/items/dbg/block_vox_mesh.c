entity dbg_block_vox_mesh = 0;
extern entity get_linked_character(ecs*, entity);

entity zox_dbg_spawn_block_vox_mesh(
    ecs *world,
    entity player)
{
    float rotate_speed = 16;
    float distance = frand_range(1.0f, 1.6f);
    if (zox_valid(dbg_block_vox_mesh)) {
        zox_log("Deleting [dbg_block_vox_mesh]");
        zox_delete(dbg_block_vox_mesh);
        dbg_block_vox_mesh = 0;
        return 0;
    }
    if (!zox_valid(player)) {
        zox_loge("[dbg_block_vox_mesh] Invalid [player]");
        return 0;
    }
    entity game = zox_get_parent(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(game)) {
        zox_loge("[dbg_item_mesh] Invalid [game]");
        return 0;
    }
    entity realm = zox_get_link(world, game, RealmLink);
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
    const BlockLinks* blocks2 = zox_get(realm, BlockLinks);
    uint count = 0;
    entity blocks[256];
    for (int i = 0; i < blocks2->length; i++) {
        entity block = blocks2->value[i];
        if (!zox_has(block, BlockVox)) {
            continue;
        }
        entity model = zox_has(block, ModelLink) ?
            zox_getv(block, ModelLink) :
            0;
        if (model) {
            blocks[count++] = block;
        }
    }
    if (!count) {
        zox_loge("No models found in blocks.");
        return 0;
    }
    entity block = blocks[rand_range(0, count)];
    if (!block) {
        zox_loge("[zox_dbg_spawn_item_mesh] Invalid Item Model");
        return 0;
    }
    dbg_block_vox_mesh = spawn_entity_mesh_clone(
        world,
        block,
        spawn_position,
        rotate_speed);
    inspect_entity(world, dbg_block_vox_mesh);
    zox_log("[dbg_block_vox_mesh] Spawned Block Mesh");
    return dbg_block_vox_mesh;
}

void zox_dbg_spawn_block_vox_mesh2(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    zox_dbg_spawn_block_vox_mesh(
        world,
        player);
}

entity dbg_block_vox_meshes[256];

void zox_dbg_spawn_block_vox_meshes_all(
    ecs *world,
    entity player)
{
    float rotate_speed = 16;
    float distance = 1.3f;
    float spacing = 1.5f;

    for (int i = 0; i < 256; i++) {
        if (zox_valid(dbg_block_vox_meshes[i])) {
            zox_log("[dbg_block_vox_meshes_all] Deleting Block Meshes");
            for (int j = 0; j < 256; j++) {
                if (zox_valid(dbg_block_vox_meshes[j])) {
                    zox_delete(dbg_block_vox_meshes[j]);
                    dbg_block_vox_meshes[j] = 0;
                }
            }
            return;
        }
    }

    if (!zox_valid(player)) {
        zox_loge("[dbg_block_vox_meshes_all] Invalid [player]");
        return;
    }

    entity game = zox_get_parent(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(game)) {
        zox_loge("[dbg_block_vox_meshes_all] Invalid [game]");
        return;
    }

    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[dbg_block_vox_meshes_all] Invalid [realm]");
        return;
    }

    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);

    const BlockLinks* blocks2 = zox_get(realm, BlockLinks);
    uint count = 0;
    entity blocks[256];
    for (int i = 0; i < blocks2->length; i++) {
        entity block = blocks2->value[i];
        if (!zox_has(block, BlockVox)) {
            continue;
        }
        entity model = zox_has(block, ModelLink) ?
            zox_getv(block, ModelLink) :
            0;
        if (!model) {
            continue;
        }
        entity mesh = get_max_model_mesh(world, model);
        if (!mesh || !zox_has(mesh, Mesh)) {
            continue;
        }
        if (count < 256) {
            blocks[count++] = block;
        }
    }
    if (!count) {
        zox_loge("[dbg_block_vox_meshes_all] No models found in blocks.");
        return;
    }
    int columns = (int) sqrtf((float) count);
    if (columns * columns < (int) count) {
        columns++;
    }

    int rows = ((int) count + columns - 1) / columns;

    for (int i = 0; i < (int) count; i++) {
        int column = i % columns;
        int row = i / columns;

        float x = ((float) column - (float) (columns - 1) * 0.5f) * spacing;
        float z = ((float) row - (float) (rows - 1) * 0.5f) * spacing;

        float3 block_position = spawn_position;
        block_position.x += x;
        block_position.z += z;

        dbg_block_vox_meshes[i] = spawn_entity_mesh_clone(
            world,
            blocks[i],
            block_position,
            rotate_speed);

        if (i == 0) {
            inspect_entity(world, dbg_block_vox_meshes[i]);
        }
    }
    zox_log("[dbg_block_vox_meshes_all] Spawned [%u] Block Meshes", count);
}


void zox_dbg_spawn_block_vox_meshes_all2(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    zox_dbg_spawn_block_vox_meshes_all(
        world,
        player);
}
