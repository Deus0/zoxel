// Spawn a basic invisible panel and a scrollbar
//  - used with list or grid entities

// TODO: Set scrollbar visible/invisible based on list count
// TODO: spawn list panel, and scrollbar as children of list entity

// NOTE: Set children on scrollview after spawning
entity spawn_scrollview(ecs *world, entity parent, int2 position, int2 size, float2 anchor, int visible, int max) {
    int scrollbar_width = 10 * ui_scale;
    int scrollbar_margins = 0;
    zox_instance(prefab_element_invisible);
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { anchor });
    zox_name("scrollview");
    // spawn scrollbar
    int2 scrollbar_position = (int2) { -(scrollbar_width / 2) - scrollbar_margins, 0 };
    int2 scrollbar_size = (int2) { scrollbar_width, size.y };
   spawn_scrollbar(world, e, scrollbar_position, scrollbar_size, scrollbar_margins, visible, max, visible != max, e);
    // zox_set_parent(world, scrollbar, e);
    return e;
}
