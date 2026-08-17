// TODO: Taskbar buttons should have a ID hooked up we can use here?? we can go through them and see if it matches window id...
// For now use taskbar hooks id

// NOTE: For a Game Menu it checks and activates Taskbar Toggle
zox_sys2(MenuGameBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_out(TaskbarToggleLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, state);
        zox_sys_o(TaskbarToggleLink, link);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            // zox_logw("Canvas is missing from taskbar");
            continue;
        }
        entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
        if (!taskbar) {
            zox_log("Taskbar not on canvas [%s]", zox_get_name(canvas));
            continue;
        }
        entity toggles[layouts2_children_capacity];
        uint toggles_length = zox_get_children(world, taskbar, toggles, layouts2_children_capacity);
        if (!toggles_length) {
            zox_log("Invalid Taskbar [%s]", zox_get_name(taskbar));
            continue;
        }
        // TODO: Get Proper Toggle per spawned window
        entity wtoggle = toggles[0]; // for now, we just add to player  menu
        //  zox_geter_value(taskbar, TaskbarToggleLink, entity, wbutton);
        if (!zox_valid(wtoggle)) {
            zox_log("Invalid Toggle [%s]", zox_get_name(taskbar));
            continue;
        }
        link->value = wtoggle;
        // zox_set(e, TaskbarToggleLink, { wtoggle });
        zox_set(wtoggle, ActiveState, { 1 });
        zox_set(wtoggle, ActiveStateDirty, { zox_dirty_trigger });

    }
} zox_sys_end(MenuGameBeginSystem);
