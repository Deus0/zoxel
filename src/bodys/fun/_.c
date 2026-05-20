// NOTE: entity_array_d* parts = create_entity_array_d(1);
// NOTE: dispose_entity_array_d(parts);

void fetch_slots_r(ecs *world, entity_array_d* entities, entity e) {
    add_to_entity_array_d(entities, e);
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        fetch_slots_r(world, entities, slot);
    }
}

void fetch_slots_parts_r(ecs *world, entity_array_d* entities, entity e) {
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        entity part = zox_getv(slot, DataLink);
        add_to_entity_array_d(entities, part);
        fetch_slots_parts_r(world, entities, slot);
    }
}

entity get_item_model(ecs* world, entity e) {
    if (!zox_valid(e)) {
        zox_logw("Item Invalid");
        return 0;
    }
    if (!zox_has(e, ModelLink)) {
        zox_logw("Item has no ModelLink [%s]", zox_get_name(e));
        return 0;
    }
    zox_geter_value(e, ModelLink, entity, model);
    if (!zox_valid(model) || !zox_has(model, ModelLods) || !zox_has(model, MaxRenderDepth)) {
        zox_logw("Item has Invalid Model [%s]", zox_get_name(e));
        return 0;
    }
    zox_geter_value(model, MaxRenderDepth, byte, mdepth);
    zox_geter(model, ModelLods, mlods);
    return mlods->value[mdepth];
}
