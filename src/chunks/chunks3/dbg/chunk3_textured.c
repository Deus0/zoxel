entity dbg_chunk3_textured;
extern entity spawn_inspector(ecs*, entity, entity, entity);

void zox_dbg_spawn_chunk3_textured(ecs* world, ClickEventData data) {
    if (zox_valid(dbg_chunk3_textured)) {
        zox_log("+ Deleting [Chunk3 Textured]");
        zox_delete(dbg_chunk3_textured);
        return;
    }
    zox_log("+ Spawning [Chunk3 Textured] (todo)");
}
