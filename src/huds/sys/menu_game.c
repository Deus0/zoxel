// TODO: Taskbar buttons should have a ID hooked up we can use here?? we can go through them and see if it matches window id...
// For now use taskbar hooks id

// NOTE: For a Game Menu it checks and activates Taskbar Toggle
zox_sys2(MenuGameBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EntityInitialize);
    zox_sys_in(CanvasLink);
    zox_sys_out(TaskbarToggleLink);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(EntityInitialize, state);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(TaskbarToggleLink, link);

        if (state->value != zox_dirty_active) {
            continue;
        }

        entity taskbar = find_child_with_tag_recursive(world, canvas->value, zox_id(Taskbar));

        if (!taskbar) {
            zox_log("Taskbar not on canvas [%s]", zox_get_name(canvas->value));
            continue;
        }

        zox_geter(taskbar, Children, toggles);

        if (!toggles->length) {
            zox_log("Invalid Taskbar [%s]", zox_get_name(taskbar));
            continue;
        }

        // TODO: Get Proper Toggle per spawned window
        entity wtoggle = toggles->value[0]; // for now, we just add to player  menu
        //  zox_geter_value(taskbar, TaskbarToggleLink, entity, wbutton);

        if (!zox_valid(wtoggle)) {
            continue;
        }

        link->value = wtoggle;
        // zox_set(e, TaskbarToggleLink, { wtoggle });
        zox_set(wtoggle, ActiveState, { 1 });
        zox_set(wtoggle, ActiveStateDirty, { zox_dirty_trigger });

    }
} zox_sys_end(MenuGameBeginSystem);
