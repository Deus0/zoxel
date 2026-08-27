// re upload gpu data
void zox_dbg_refresh_uis(ecs *world, int32_t keycode) {
    if (keycode != zox_key_l) {
        return;
    }
    entity canvas = zox_getv(dbg_player, CanvasLink);
    // for all children and sub children, refresh the
    entity test_id = zox_id(Zigel); // zox_id(MeshDirty);
    entity uis[64];
    uint length = zox_get_children_by_id_recursive(world, canvas, uis, 64, test_id, 0);
    for (uint j = 0; j < length; j++) {
        entity e = uis[j];
        zox_log("Refreshing UI [%s]", zox_getn(e));
        // zox_remove(e, Initialize);
        // break;
        // zox_setv(ui, Initialize, 1);
        // zox_setv(ui, GenerateTexture, 1);
        // zox_setv(ui, MeshDirty, 1);
    }
}
