// A list's elements are its children, ordered by ChildIndex
//  - children iterate by table, and an element changes table when a tag is added or removed
//      (i.e. Select when hovered), so iteration order shuffles, lists use this order instead

static inline uint get_list_child_order(
    ecs* world,
    entity e)
{
    return zox_has(e, ChildIndex) ? zox_getv(e, ChildIndex) : UINT32_MAX;
}

// fills children (up to max) in ChildIndex order, children without one keep their order at the end
uint get_list_children_ordered(
    ecs* world,
    entity list,
    entity* children,
    uint max)
{
    uint count = 0;
    iter it2 = zox_children(world, list);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (count < max) {
                children[count++] = it2.entities[j];
            }
        }
    }
    // stable insertion sort, lists are small
    for (uint i = 1; i < count; i++) {
        const entity e = children[i];
        const uint order = get_list_child_order(world, e);
        uint j = i;
        while (j > 0 && get_list_child_order(world, children[j - 1]) > order) {
            children[j] = children[j - 1];
            j--;
        }
        children[j] = e;
    }
    return count;
}
