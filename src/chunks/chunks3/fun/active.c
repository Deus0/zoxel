// what's switched on in a chunk (ActiveOctree, com/_.c): a door open, a gravity cube on; 0 off, later a wire's power
//  - nothing on: its root, one node at 0 (a chunk with nothing on is one check)
//  - saved after the block datas as a section (active_octree_section), only when something's on: an id byte, then the
//    tree as the voxels are saved (a node's value, a byte if it has children, its 8 children after)
//  - its block replaced or removed: off (voxel_update_queue_system, old_active)

// is anything on in it
static inline byte active_octree_any(const ActiveOctree* actives) {
    return actives && (actives->value || has_children_ActiveOctree(actives));
}

static byte save_active_node(FILE* out, const ActiveOctree* node) {
    const ActiveOctree* kids = get_children_ActiveOctree(node);
    const byte has_kids = kids != NULL;
    if (fwrite(&node->value, 1, 1, out) != 1 || fwrite(&has_kids, 1, 1, out) != 1) {
        return 0;
    }
    for (byte i = 0; has_kids && i < octree_length; i++) {
        if (!save_active_node(out, &kids[i])) {
            return 0;
        }
    }
    return 1;
}

byte save_active_octree(FILE* out, const ActiveOctree* actives) {
    if (!active_octree_any(actives)) {
        return 1;
    }
    const byte section = active_octree_section;
    return fwrite(&section, 1, 1, out) == 1 && save_active_node(out, actives);
}

byte load_active_octree(FILE* in, ActiveOctree* node) {
    byte has_kids = 0;
    if (fread(&node->value, 1, 1, in) != 1 || fread(&has_kids, 1, 1, in) != 1) {
        return 0;
    }
    if (!has_kids) {
        collapse_ActiveOctree(node);
        return 1;
    }
    if (!has_children_ActiveOctree(node)) {
        open_one_ActiveOctree(node);
    }
    ActiveOctree* kids = get_children_ActiveOctree(node);
    for (byte i = 0; i < octree_length; i++) {
        if (!load_active_octree(in, &kids[i])) {
            return 0;
        }
    }
    return 1;
}

// the sections after a chunk's octree, its block datas and then what's on
byte save_chunk_sections(FILE* out, const BlockDatas* datas, const ActiveOctree* actives) {
    return save_block_datas(out, datas) && save_active_octree(out, actives);
}

// its block datas alone (what's on skipped)
byte load_block_datas(FILE* in, BlockDatas* datas) {
    return load_chunk_sections(in, datas, NULL);
}
