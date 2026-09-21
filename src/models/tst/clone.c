entity dbg_vox_mesh_clone = 0;
extern entity get_linked_character(ecs*, entity);

entity zox_dbg_spawn_vox_mesh_clone(
    ecs *world,
    entity player)
{
    float rotate_speed = 16;
    if (zox_valid(dbg_vox_mesh_clone)) {
        zox_log("Deleting [dbg_vox_mesh_clone]");
        zox_delete(dbg_vox_mesh_clone);
        dbg_vox_mesh_clone = 0;
        return 0;
    }
    if (!zox_valid(player)) {
        return 0;
    }
    entity canvas = get_linked_canvas(world, player);
    entity character = get_linked_character(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(character)) {
        zox_log("No character to render");
        return 0;
    }
    float3 camera_position = zox_getv(camera, Position3D);
    entity mesh_clone =
        spawn_mesh3_clone(
            world,
            character);
    zox_add(mesh_clone, VoxMesh);
    zox_set_unique_name(mesh_clone, "character_clone");
    zox_setv(mesh_clone, Position3D, camera_position);
    zox_setv(mesh_clone, Rotation3D, quaternion_identity);
    add_eternal_euler(
        world,
        mesh_clone,
        (float3) { 0, rotate_speed, 0 });
    dbg_vox_mesh_clone = mesh_clone;
    return mesh_clone;
}

void zox_dbg_spawn_vox_mesh_clone2(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    zox_dbg_spawn_vox_mesh_clone(
        world,
        player);
}
