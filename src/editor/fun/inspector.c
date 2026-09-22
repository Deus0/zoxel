void inspect_entity(ecs* world, entity e) {
    if (local_inspector) {
        zox_setv(local_inspector, EntityTarget, e);
        zox_setv(local_inspector, InspectorDirty, zox_dirty_trigger);
    }
}
