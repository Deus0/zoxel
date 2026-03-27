// update inspector element text every .5 seconds by checking if value changed... with a string check

byte set_entity_label_with_text(ecs *world, entity e, const char* text) {
    zox_geter(e, Children, children);
    if (children->length == 0) {
        return 0;
    }
    return set_entity_text(world, children->value[0], text);
}

zox_sys2(InspectorElementSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EntityTarget);
    zox_sys_in(ComponentTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(EntityTarget, target);
        zox_sys_i(ComponentTarget, component);

        if (!target->value || !zox_alive(target->value) || !component->value) {
            continue;
        }

        uint tlength = inspector_component_size_buffer;
        char buffer[tlength];
        get_component_label(world, target->value, component->value, buffer, tlength);
        set_entity_label_with_text(world, e, buffer);
    }
} zox_sys_end(InspectorElementSystem);
