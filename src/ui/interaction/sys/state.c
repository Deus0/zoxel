// Selected
void trigger_select_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerSelect);
        zox_add(e, Select);
    }
    zox_sys_on_end();
} zoxd_system(trigger_select_system);

void select_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Select);
    }
    zox_sys_on_end();
} zoxd_system(select_system);

// the ui events (non fragmenting tags): every observer of one sees it, this one removes it (observers' commands are
//  deferred, so its removal lands after all of them)
void deselect_cleanup_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], Deselect);
    }
}

void click_cleanup_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], Click);
    }
}

void right_click_cleanup_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], RightClick);
    }
}

void press_cleanup_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], Press);
    }
}

