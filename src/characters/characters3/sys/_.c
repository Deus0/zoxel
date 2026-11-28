#include "saving.c"
#include "children.c"
#include "debug_types.c"
#include "realm.c"
#include "light.c"

void zox_systems_characters3(ecs *world) {
    // Spawns render children
    zox_system_1(
        character_children_spawn_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] rendering.RenderDepth,
        [none] Character3,
    );
    zox_system(
        character_light_system,
        zoxp_update,
        [in] transforms3.Position3D,
        [out] rendering.Brightness,
        [out] rendering.BlockBrightness,
        [none] Character3
    );
    zox_system(
        character_save_system,
        zoxp_update,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [out] saves.SaveHash,
        [none] saves.Saver
    );
}

/*zox_system(
    character3_type_debug_system,
    zoxp_update,
    [in] lines3.DebugCubeLines,
    [in] characters3.Character3Type,
    [out] colorz.Color,
    [none] Character3
);*/
