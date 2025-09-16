void delete_nodes(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, NodeLinks)) {
        zox_geter(e, NodeLinks, nodes);
        for (int i = 0; i < nodes->length; i++) {
            entity e2 = nodes->value[i];
            delete_nodes(world, e2);
        }
    }
    zox_delete(e);
}