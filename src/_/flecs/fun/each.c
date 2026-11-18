// adds a tag to every entity with T (not prefabs), after walking them: an add moves it to another table
void zox_each_add_id(ecs* world, entity with, entity tag) {
    entity_array_d* entities = create_entity_array_d(initial_dynamic_array_size);
    iter it = ecs_each_id(world, with);
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            if (!zox_is_prefab(it.entities[i])) {
                entity_array_d_add(entities, it.entities[i]);
            }
        }
    }
    for (size_t i = 0; i < entities->size; i++) {
        ecs_add_id(world, entities->data[i], tag);
    }
    dispose_entity_array_d(entities);
}
