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
