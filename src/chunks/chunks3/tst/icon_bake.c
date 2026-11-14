// tst: icon_bake, vox textures (item icons) bake from their model once it is generated (vox_texture_system), not before, not long after
//  - bash bsh/tst.sh icon_bake
//  - tracks every vox texture whose model was seen Generating (a blueprint running, spawn_process_model)
//  - baked: it has pixels (TextureData) and isn't waiting to run (vox_texture_system resizes it as it bakes)
//  - catches the old 1s delay_event in spawn_texture_from_vox: a model done sooner was baked late,
//      one done later was baked from unfinished voxels (blank or partial icons)
#define zox_tst_icons_max 256
#define zox_tst_icons_late_frames 10    // baked within this many frames of its model finishing
#define zox_tst_icons_min 4             // a realm's body parts, clothes and food: plenty more than this

typedef struct {
    entity texture;
    entity model;
    byte baked;
    byte baked_early;       // its model still Generating when it baked
    int done_frame;         // first frame its model wasn't Generating, -1 before
    int baked_frame;
} zox_tst_icon;

zox_tst_icon zox_tst_icons[zox_tst_icons_max];
int zox_tst_icons_count = 0;

byte zox_tst_icon_bake_ready(ecs* world) {
    (void) world;
    int baked = 0;
    for (int i = 0; i < zox_tst_icons_count; i++) {
        baked += zox_tst_icons[i].baked;
    }
    static char waiting[128];
    snprintf(waiting, sizeof(waiting), "icons of generating models to bake: [%i/%i] baked", baked, zox_tst_icons_count);
    zox_tst_waiting = waiting;
    zox_tst_progress(zox_tst_icons_count * 1000 + baked);
    return zox_tst_icons_count >= zox_tst_icons_min && baked == zox_tst_icons_count;
}

void zox_tst_icon_bake_check(ecs* world) {
    int early = 0, late = 0;
    for (int i = 0; i < zox_tst_icons_count; i++) {
        const zox_tst_icon* icon = &zox_tst_icons[i];
        const char* name = zox_valid(icon->texture) ? zox_getn(icon->texture) : "deleted";
        const int after = icon->done_frame >= 0 ? icon->baked_frame - icon->done_frame : -1;
        if (icon->baked_early) {
            early++;
            zox_tst_check(0, "icon [%s] baked while its model was still generating", name);
        } else if (after > zox_tst_icons_late_frames) {
            late++;
            zox_tst_check(0, "icon [%s] baked [%i] frames after its model finished (most [%i])",
                name, after, zox_tst_icons_late_frames);
        } else {
            zox_tst_check(1, "");
        }
    }
    zox_log("[tst] icons [%i]: early [%i] late [%i]", zox_tst_icons_count, early, late);
}

// watches vox textures each frame, only while the test runs
void tst_icon_bake_probe_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_active || zox_tst_active->check != zox_tst_icon_bake_check) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateTexture);
    zox_sys_in(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateTexture, generate);
        zox_sys_i(TextureData, data);
        const entity model = zox_get_link(world, e, ModelLink);
        if (!zox_valid(model)) {
            continue;
        }
        const byte generating = zox_has(model, Generating);
        zox_tst_icon* icon = NULL;
        for (int j = 0; j < zox_tst_icons_count && !icon; j++) {
            if (zox_tst_icons[j].texture == e) {
                icon = &zox_tst_icons[j];
            }
        }
        if (!icon) {
            // only icons seen while their model generates: models loaded or generated before can't be told apart
            if (!generating || zox_tst_icons_count >= zox_tst_icons_max) {
                continue;
            }
            icon = &zox_tst_icons[zox_tst_icons_count++];
            *icon = (zox_tst_icon) { e, model, 0, 0, -1, -1 };
        }
        if (icon->baked) {
            continue;
        }
        if (!generating && icon->done_frame < 0) {
            icon->done_frame = (int) ecs_run_count;
        }
        if (data->length && generate->value != zox_generate_texture_run) {
            icon->baked = 1;
            icon->baked_frame = (int) ecs_run_count;
            icon->baked_early = generating;
        }
    }
    zox_sys_on_end();
} zoxd_system(tst_icon_bake_probe_system);

void zox_tsts_icon_bake(ecs* world) {
    zox_tst_add("icon_bake", zox_tst_icon_bake_ready, zox_tst_icon_bake_check, 90);
    zox_system_1(
        tst_icon_bake_probe_system,
        zoxp_render,
        [in] textures.GenerateTexture,
        [in] textures.TextureData,
        [none] textures.VoxTexture
    );
}
