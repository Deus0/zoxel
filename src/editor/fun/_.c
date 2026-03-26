#include "shared.c"
#include "old.c"
#include "inspector.c"
#include "hierarchy.c"

// grabs all entity list data into entity + name labels
void editor_fetch_children(ecs *world, entity_array_d* entities, text_group_dynamic_array_d* labels, entity target) {

    // add game entities
    if (!zox_valid(target)) {
        return;
    }

    add_entity_to_labels(world, target, labels, entities, 0);

    fetch_entity_list_by_id(world, target, zox_id(Children), labels, entities, 0);
    fetch_entity_list_by_id(world, target, zox_id(CameraLinks), labels, entities, 0);
    fetch_entity_list_by_id(world, target, zox_id(PlayerLinks), labels, entities, 0);
    fetch_entity_list_by_id(world, target, zox_id(BlockLinks), labels, entities, 0);
    fetch_entity_list_by_id(world, target, zox_id(StatLinks), labels, entities, 0);
    fetch_entity_list_by_id(world, target, zox_id(ItemLinks), labels, entities, 0);
    fetch_entity_list_by_id(world, target, zox_id(QuestLinks), labels, entities, 0);
}
