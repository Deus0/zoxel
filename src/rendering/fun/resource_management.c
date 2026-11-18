// gpu dispose and restore: an app minimized, android's lost context, a fullscreen alt tab
//  - one-off walks, not systems: each module adds a function that walks its gpu links (zox_each_begin)
//  - disabled entities too (a system skips them: a hidden lod mesh kept a dead buffer), prefabs hold nothing
zox_hook(gpu_dispose, (ecs* world), (world))
zox_hook(gpu_restore, (ecs* world), (world))

byte did_dispose_resources = 0;

// the entities a dispose let go of a gpu link on, for the restore to make again (one that held nothing, something
//  else makes, i.e. a mesh still to upload: making it too leaked one)
static inline void zox_disposed_add(entity_array_d** list, entity e) {
    if (!*list) {
        *list = create_entity_array_d(initial_dynamic_array_size);
    }
    entity_array_d_add(*list, e);
}

// each entity of a disposed list still alive with T: e and its T, the list is let go of after
//  - zox_disposed_begin(list, T, name); ... zox_disposed_end(list);
#define zox_disposed_begin(list, T, name)\
    if (list) {\
        for (size_t list_i = 0; list_i < list->size; list_i++) {\
            const entity e = list->data[list_i];\
            T* name = zox_valid(e) ? zox_get_mut(e, T) : NULL;\
            if (!name) continue

#define zox_disposed_end(list)\
        }\
        dispose_entity_array_d(list);\
        list = NULL;\
    }

void opengl_dispose_resources(ecs *world) {
    if (!did_dispose_resources) {
        did_dispose_resources = 1;
        rendering = 0;
        minimized = 1; // move this to a system and function for app! when implement multi apps
        updating_time = 0; // timesteps a bit low atm so pause while minimizing
        run_hook_gpu_dispose(world);
    }
}

void opengl_restore_resources(ecs *world) {
    if (did_dispose_resources) {
        did_dispose_resources = 0;
        run_hook_gpu_restore(world);
        rendering = 1;
        minimized = 0;
        updating_time = 1;
        skip_time_to_current();
    }
}
