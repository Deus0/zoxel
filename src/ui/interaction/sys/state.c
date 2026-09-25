// Selected
void trigger_select_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerSelect);
        zox_add(e, Select);
    }
} zoxd_system(trigger_select_system);

void select_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Select);
    }
} zoxd_system(select_system);

// Selected
void trigger_deselect_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerDeselect);
        zox_add(e, Deselect);
    }
} zoxd_system(trigger_deselect_system);

void deselect_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Deselect);
    }
} zoxd_system(deselect_system);
