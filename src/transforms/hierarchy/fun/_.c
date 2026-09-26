static inline entity get_child_at_index(
    ecs* world,
    entity* children,
    uint length,
    uint index)
{
    for (int i = 0; i < length; i++) {
        entity child = children[i];
        if (!zox_valid(child) ||
            !zox_has(child, ChildIndex)) {
            continue;
        }
        uint child_index = zox_getv(child, ChildIndex);
        if (index == child_index) {
            return child;
        }
    }
    return 0;
}
