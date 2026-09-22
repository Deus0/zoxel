void trigger_begin_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerBegin);
        zox_add(e, Begin);
    }
} zoxd_system(trigger_begin_system);

void begin_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Begin);
    }
} zoxd_system(begin_system);

void trigger_end_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerEnd);
        zox_add(e, End);
    }
} zoxd_system(trigger_end_system);

void end_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, End);
    }
} zoxd_system(end_system);


void trigger_exit_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, TriggerExit);
        zox_add(e, Exit);
    }
} zoxd_system(trigger_exit_system);

void exit_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, End);
    }
} zoxd_system(exit_system);

