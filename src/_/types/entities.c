#define zox_component_string_entities(c) " ee [%i]", c->value.length

zox_base_type(entities)

#define zoxd_entities(T) \
    zoxd_arrayd(T) \
    add_to_entity_array_d(component_ids_entities, ecs_id(T)); \
    zox_observe(on_destroyed_##T, EcsOnRemove, [in] T)

// Weak is without the Destruction Hooks
#define zoxc_entitiesw(T)\
    /*add_to_entity_array_d(component_ids_entities, ecs_id(T));*/ \
    zoxc_arrayd(T, entity)

#define zoxd_entitiesw(T) \
    zoxd_arrayd(T) \
    add_to_entity_array_d(component_ids_entities, ecs_id(T));
