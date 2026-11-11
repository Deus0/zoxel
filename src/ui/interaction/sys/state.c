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

// Clicks
void trigger_click_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerClick);
        zox_add(e, Click);
    }
} zoxd_system(trigger_click_system);

void trigger_press_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerPress);
        zox_add(e, Press);
    }
} zoxd_system(trigger_press_system);

void press_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Press);
    }
} zoxd_system(press_system);

void click_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Click);
    }
} zoxd_system(click_system);
