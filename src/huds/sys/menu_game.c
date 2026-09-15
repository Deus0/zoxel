// TODO: Taskbar buttons should have a ID hooked up we can use here?? we can go through them and see if it matches window id...
// For now use taskbar hooks id

// NOTE: For a Game Menu it checks and activates Taskbar Toggle
zox_sys2(TaskbarBeginSystem) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            // zox_logw("Canvas is missing from taskbar");
            continue;
        }
        entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
        if (!taskbar) {
            continue;
        }
        // Get Taskbar Body
        entity body = zox_get_child_by_id(world, e, zox_id(WindowBody));
        if (!body) {
            zox_loge("Invalid [taskbar_body]");
            continue;
        }
        // for every TaskbarWindowID, check if entity has id, if it does, we found our toggle
        entity toggle = 0;
        iter it2 = zox_children(world, body);
        while (zox_children_next(it2) && !toggle) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_has(e2, TaskbarWindowID)) {
                    continue;
                }
                entity id = zox_getv(e2, TaskbarWindowID);
                if (zox_has_id(e, id)) {
                    toggle = e2;
                    break;
                }
            }
        }
        // TODO: Get Proper Toggle per spawned window
        // entity wtoggle = toggles[0]; // for now, we just add to player  menu
        if (!zox_valid(toggle)) {
            zox_log("Unfound Toggle [%s]: [%s]",
                zox_sys_e_name,
                zox_getn(taskbar));
            continue;
        }
        zox_link(world, e, Toggle, toggle);
        zox_setv(toggle, ActiveState, 1);
        zox_setv(toggle, ActiveStateDirty, zox_dirty_trigger);
    }
} zox_sys_end(TaskbarBeginSystem);
