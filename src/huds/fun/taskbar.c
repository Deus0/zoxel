// nested function (GCC extension)
void on_closed_taskbar_window(
    ecs *world,
    ClickEventData data)
{
    entity window = zox_get_parent_by_id(
        world,
        data.clicked,
        zox_id(Window));
    if (!zox_valid(window)) {
        zox_loge("Close button couldn't find window [%s]",
            zox_getn(data.clicked));
        return;
    }
    entity toggle = zox_get_link(world, window, Toggle);
    if (zox_valid(toggle)) {
        /*zox_log("[on_closed_taskbar_window] Window [%s] has [Toggle] %s", zox_getn(window), zox_getn(toggle));*/
        zox_setv(toggle, ActiveState, 0);
        zox_setv(toggle, ActiveStateDirty, zox_dirty_trigger);
    } else {
        zox_loge("Window [%s] has no [Toggle]",
            zox_getn(window));
    }
    zox_delete(window);
}

// NOTE: window has just spawned, we can only use set with it
// TODO: Move to a system for TaskbarWindow or regular window..!
void link_window_to_taskbar(
    ecs* world,
    entity window,
    entity canvas,
    entity window_id)
{
    byte dbg_log = 0;
    if (!zox_valid(window)) {
        zox_loge("[on_spawned_taskbar_window] Invalid Window");
        return;
    }
    if (!zox_valid(canvas)) {
        zox_loge("[on_spawned_taskbar_window] Invalid Canvas");
        return;
    }
    entity taskbar = zox_get_child_by_id(
        world,
        canvas,
        zox_id(Taskbar));
    if (!zox_valid(taskbar)) {
        zox_loge("[on_spawned_taskbar_window] Invalid Taskbar");
        return;
    }
    entity taskbar_body = zox_get_child_by_id(
        world,
        taskbar,
        zox_id(WindowBody));
    if (!zox_valid(taskbar_body)) {
        zox_loge("[on_spawned_taskbar_window] Invalid taskbar_body");
        return;
    }
    // get children taskbar icons and see if window has any of them
    entity toggle = 0;
    entity frames[zox_children_capacity];
    uint length = zox_get_children_by_id(
        world,
        taskbar_body,
        frames,
        zox_children_capacity,
        zox_id(TaskbarWindowID));
    for (uint j = 0; j < length; j++) {
        entity e2 = frames[j];
        if (!zox_valid(e2)) {
            continue;
        }
        entity id = zox_getv(e2, TaskbarWindowID);
        if (dbg_log) {
            zox_log("Frame [%s] had ID [%s] =? [%s]",
                zox_get_name(e2),
                zox_get_name(id),
                zox_get_name(window_id));
        }
        if (window_id == id) { // zox_has_id(window, id)) {
            toggle = e2;
            break;
        }
    }
    if (zox_valid(toggle)) { //  && zox_has(toggle, Toggle)) {
        zox_link(world, window, Toggle, toggle);
        zox_setv(toggle, ActiveState, 1);
        zox_setv(toggle, ActiveStateDirty, zox_dirty_trigger);
    } else {
        zox_loge("Window could not find Toggle [%s]",
            zox_getn(toggle));
    }
}
