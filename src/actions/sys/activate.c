// right click = place
void ActionActivateSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionB);
    zox_sys_in(ActionIndex);
    zox_sys_in(ActionLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerActionB, trigger);
        zox_sys_i(ActionIndex, index);
        zox_sys_i(ActionLinks, actions);

        if (trigger->value != zox_dirty_active) {
            continue;
        }

        if (index->value >= actions->length) {
            zox_logw("Action selected is out of bounds [%i of %i]", index->value, actions->length);
            continue;
        }

        const entity action = actions->value[index->value];
        // no action assigned
        if (zox_valid(action) && !zox_gett_value(action, ActivateBegin)) {
            zox_set(action, ActivateBegin, { zox_dirty_trigger });
            // zox_log("action begin [%f]", zox_gett_value(action, WarmupTime));
        }

    }
} zoxd_system2(ActionActivateSystem);