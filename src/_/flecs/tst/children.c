// tst: children, the claims in doc/children.md
//  - set / get / is / remove parent, zox_set_parent(.., 0) removes it
//  - get children into a buffer (extras skipped at capacity), by id, first by id, by index, counts
//  - recursive: first match and all matches below a parent
//  - walking up: nearest ancestor by id, and the root
//  - children are deleted with their parent (and their children)
//  - ordered children keep insertion order, also after deleting some and adding more (recycled ids)
//  - zox_get_ordered_children is empty for an invalid parent, or one without ordered children
//  - run: bash bsh/tst.sh children
zox_tag(TstChildTag);

static inline byte zox_tst_has_entity(const entity* list, uint count, entity e) {
    for (uint i = 0; i < count; i++) {
        if (list[i] == e) {
            return 1;
        }
    }
    return 0;
}

byte zox_tst_children_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_children_check(ecs* world) {
    // parenting
    const entity parent = zox_new();
    const entity child = zox_new();
    zox_tst_check(!zox_get_parent(world, child), "new entity has a parent");
    zox_tst_check(zox_set_parent(world, child, parent), "set_parent didn't return 1");
    zox_tst_check(zox_get_parent(world, child) == parent && zox_is_parent(world, child, parent), "parent not set");
    zox_remove_parent(world, child);
    zox_tst_check(!zox_get_parent(world, child), "remove_parent kept the parent");
    zox_set_parent(world, child, parent);
    zox_set_parent(world, child, 0);
    zox_tst_check(!zox_get_parent(world, child), "set_parent 0 kept the parent");
    // children queries
    #define zox_tst_children_count 12
    entity kids[zox_tst_children_count];
    for (int i = 0; i < zox_tst_children_count; i++) {
        kids[i] = zox_new();
        if (i % 3 == 0) {
            zox_add(kids[i], TstChildTag);
        }
        zox_set_parent(world, kids[i], parent);
    }
    zox_tst_check(zox_get_children_count(world, parent) == zox_tst_children_count, "children count [%u]", zox_get_children_count(world, parent));
    zox_tst_check(zox_get_children_count_by_id(world, parent, zox_id(TstChildTag)) == 4, "children count by id [%u], expected 4",
        zox_get_children_count_by_id(world, parent, zox_id(TstChildTag)));
    entity buffer[32];
    uint count = zox_get_children(world, parent, buffer, 32);
    int missing = 0;
    for (int i = 0; i < zox_tst_children_count; i++) {
        missing += !zox_tst_has_entity(buffer, count, kids[i]);
    }
    zox_tst_check(count == zox_tst_children_count && !missing, "get_children returned [%u], missing [%i]", count, missing);
    count = zox_get_children(world, parent, buffer, 5);
    zox_tst_check(count == 5, "get_children at capacity 5 returned [%u]", count);
    count = zox_get_children_by_id(world, parent, buffer, 32, zox_id(TstChildTag));
    int untagged = 0;
    for (uint i = 0; i < count; i++) {
        untagged += !zox_has(buffer[i], TstChildTag);
    }
    zox_tst_check(count == 4 && !untagged, "get_children_by_id returned [%u], [%i] without the tag", count, untagged);
    const entity tagged = zox_get_child_by_id(world, parent, zox_id(TstChildTag));
    zox_tst_check(tagged && zox_has(tagged, TstChildTag), "get_child_by_id didn't find a tagged child");
    const entity first = zox_get_child_by_index(world, parent, 0);
    zox_tst_check(zox_tst_has_entity(kids, zox_tst_children_count, first), "get_child_by_index 0 isn't a child");
    // recursive, a tagged grandchild under an untagged child
    const entity middle = kids[1];
    const entity grandchild = zox_new();
    zox_add(grandchild, TstChildTag);
    zox_set_parent(world, grandchild, middle);
    zox_tst_check(zox_get_child_by_id_recursive(world, middle, zox_id(TstChildTag)) == grandchild, "recursive search missed the grandchild");
    count = zox_get_children_by_id_recursive(world, parent, buffer, 32, zox_id(TstChildTag), 0);
    zox_tst_check(count == 5 && zox_tst_has_entity(buffer, count, grandchild), "recursive children by id [%u], expected 5 with the grandchild", count);
    // walking up
    zox_add(parent, TstChildTag);
    zox_tst_check(zox_get_parent_by_id(world, grandchild, zox_id(TstChildTag)) == parent, "parent_by_id didn't find the tagged ancestor");
    zox_tst_check(zox_get_parent_root(world, grandchild) == parent, "parent_root isn't the top");
    // deleted with their parent
    zox_delete(parent);
    int alive = 0;
    for (int i = 0; i < zox_tst_children_count; i++) {
        alive += zox_valid(kids[i]);
    }
    zox_tst_check(!alive && !zox_valid(grandchild), "[%i] children (and grandchild [%i]) outlived their parent", alive, zox_valid(grandchild));
    zox_tst_check(zox_valid(child), "an unparented child was deleted with its old parent");
    zox_delete(child);
    // ordered children, insertion order survives deletes and recycled ids
    const entity ordered = zox_new();
    zox_add_ordered_children(ordered);
    entity order[64];
    int order_count = 0;
    int order_errors = 0;
    for (int round = 0; round < 20; round++) {
        // add 3
        for (int i = 0; i < 3; i++) {
            const entity e = zox_new();
            zox_set_parent(world, e, ordered);
            order[order_count++] = e;
        }
        // delete the second oldest
        if (order_count > 2) {
            zox_delete(order[1]);
            for (int i = 1; i < order_count - 1; i++) {
                order[i] = order[i + 1];
            }
            order_count--;
        }
        const ordered_children list = zox_get_ordered_children(world, ordered);
        byte same = list.length == order_count;
        for (int i = 0; same && i < order_count; i++) {
            same = list.value[i] == order[i];
        }
        if (!same && !order_errors++) {
            zox_loge("[tst] ordered children round [%i]: [%i] children, expected [%i]", round, list.length, order_count);
        }
    }
    zox_tst_check(!order_errors, "ordered children lost their order in [%i] of 20 rounds", order_errors);
    zox_delete(ordered);
    const entity unordered = zox_new();
    zox_tst_check(!zox_get_ordered_children(world, unordered).length, "ordered children of a parent without them isn't empty");
    zox_tst_check(!zox_get_ordered_children(world, ordered).length, "ordered children of a deleted parent isn't empty");
    zox_delete(unordered);
}

void zox_components_tst_children(ecs* world) {
    zoxd_tag(TstChildTag);
}
