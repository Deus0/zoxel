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
    uint count = 0;
    entity items[256];
    // for children of ItemVox
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity child = it2.entities[j];
            if (!zox_has(child, ItemVox)) {
                continue;
            }
            entity model = zox_has(child, ModelLink) ?
                zox_getv(child, ModelLink) :
                0;
            if (model) {
                items[count++] = child;
            }
        }
    }
    if (!count) {
        zox_loge("No models found in blocks.");
        return 0;
    }
    entity item = items[rand_range(0, count)];
    if (!item) {
        zox_loge("[zox_dbg_spawn_item_mesh] Invalid Item Model");
        return 0;
    }
    entity model = zox_has(item, ModelLink) ?
        zox_getv(item, ModelLink) :
        0;
    entity mesh = get_max_model_mesh(world, model);
    if (!mesh) {
        zox_loge("[dbg_item_mesh] Invalid Model [mesh]:[%s]",
            zox_getn(model));
        return 0;
    }
    if (!zox_has(mesh, Mesh)) {
        zox_loge("[dbg_item_mesh] Invalid [mesh] [%s]: No Mesh on Entity [%s]",
            zox_getn(model),
            zox_getn(mesh));
        return 0;
    }
    // float3 camera_position = zox_getv(camera, Position3D);
    entity mesh_clone = spawn_mesh3_clone(world, mesh);
    if (!mesh_clone) {
        zox_loge("Invalid [mesh_clone]");
        return 0;
    }
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

entity dbg_item_meshes[256];

void zox_dbg_spawn_item_meshes_all(
    ecs *world,
    entity player)
{
    float rotate_speed = 16;
    float distance = 1.3f;
    float spacing = 1.5f;

    for (int i = 0; i < 256; i++) {
        if (zox_valid(dbg_item_meshes[i])) {
            zox_log("[dbg_item_meshes_all] Deleting Item Meshes");
            for (int j = 0; j < 256; j++) {
                if (zox_valid(dbg_item_meshes[j])) {
                    zox_delete(dbg_item_meshes[j]);
                    dbg_item_meshes[j] = 0;
                }
            }
            return;
        }
    }

    if (!zox_valid(player)) {
        zox_loge("[dbg_item_meshes_all] Invalid [player]");
        return;
    }

    entity game = zox_get_parent(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(game)) {
        zox_loge("[dbg_item_meshes_all] Invalid [game]");
        return;
    }

    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[dbg_item_meshes_all] Invalid [realm]");
        return;
    }

    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);

    uint count = 0;
    entity items[256];

    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity item = it2.entities[j];
            if (!zox_has(item, ItemVox)) {
                continue;
            }

            entity model = zox_has(item, ModelLink) ?
            zox_getv(item, ModelLink) :
            0;
            if (!model) {
                continue;
            }

            entity mesh = get_max_model_mesh(world, model);
            if (!mesh || !zox_has(mesh, Mesh)) {
                continue;
            }

            if (count >= 256) {
                break;
            }

            items[count++] = item;
        }

        if (count >= 256) {
            break;
        }
    }

    if (!count) {
        zox_loge("[dbg_item_meshes_all] No models found in items.");
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

        float3 item_position = spawn_position;
        item_position.x += x;
        item_position.z += z;

        dbg_item_meshes[i] = spawn_entity_mesh_clone(
            world,
            items[i],
            item_position,
            rotate_speed);

        if (i == 0) {
            inspect_entity(world, dbg_item_meshes[i]);
        }
    }

    zox_log("[dbg_item_meshes_all] Spawned [%u] Item Meshes", count);
}

void zox_dbg_spawn_item_meshes_all2(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    zox_dbg_spawn_item_meshes_all(
        world,
        player);
}



