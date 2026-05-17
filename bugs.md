Bugs:

- At start a terrain chunk lights didn't load
-x Fix set_octree_value Warnings
- Fix Raycast Error: Invalid Vox warning
- Fix dungeon core texture issues
- Fix DungeonCore not showing texture properly
- Sometimes npcs spawn in same chunk twice
- Sometimes character missing a mesh too
- Characters sometimes small - due to block depth
- Characters sometimes fall through map
- Audio missing from main menu - need to spawn a realm for it

Polish:

- Fix little teleport when loading player character
- set the slider text block depth [5] when sliding
    
Refactors:
    uint children_capacity = zox_children_capacity;
    entity children[children_capacity];
    uint children_length = zox_get_children(world, e, children, children_capacity);
    zox_set_parent(world, e, e2);
