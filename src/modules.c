/*
 * +------------------------------------------------------------------+
 * | Zox Module: Zox Itself                                           |
 * |                                                                  |
 * |  Cores - Engine - Simulation - Gameplay                          |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */

// depth 0: foundation
#include "core/_.c"
#include "transforms/_.c"
#include "networking/_.c"
#include "triggers/_.c"
#include "screens/_.c"
#include "inputs/_.c"
#include "graphics/_.c"
#include "apps/_.c"
#include "names/_.c"
#include "saves/_.c"

// depth 1: basic world blocks
#include "assets/_.c"
#include "realms/_.c"
#include "games/_.c"
#include "players/_.c"
#include "colors/_.c"
#include "nodes/_.c"
#include "slots/_.c"

// depth 2: engine / rendering
#include "cameras/_.c"
#include "rendering/_.c"
#include "sounds/_.c"
#include "raycasts/_.c"
#include "shapes/_.c"
#include "textures/_.c"
#include "geometry/_.c"
#include "musics/_.c"
#include "animations/_.c"
#include "bones/_.c"
#include "ui/_.c"

// depth 3: simulation / world
#include "plots/_.c"
#include "genetics/_.c"
#include "neurals/_.c"
#include "blueprints/_.c"
#include "blocks/_.c"
#include "chunks/_.c"
#include "voxes/_.c"
#include "physics/_.c"
#include "lights/_.c"
#include "models/_.c"

// depth 4: environment
#include "weathers/_.c"
#include "streaming/_.c"
#include "regions/_.c"
#include "particles/_.c"
#include "tunks/_.c"
#include "terrain/_.c"
#include "biomes/_.c"
#include "heights/_.c"
#include "vegetation/_.c"
#include "mountains/_.c"
#include "towns/_.c"
#include "vrays/_.c"
#include "characters/_.c"
#include "pets/_.c"
#include "huds/_.c"
#include "space/_.c"

// depth 5: user data
#include "users/_.c"
#include "combat/_.c"
#include "projectiles/_.c"
#include "stats/_.c"
#include "skills/_.c"
#include "items/_.c"
#include "parts/_.c"
#include "equipment/_.c"
#include "lores/_.c"
#include "achievements/_.c"
#include "dialogues/_.c"

// depth 6: gameplay
#include "actions/_.c"
#include "classes/_.c"
#include "jobs/_.c"
#include "races/_.c"
#include "clans/_.c"
#include "quests/_.c"
#include "pickups/_.c"
#include "crafting/_.c"
#include "farming/_.c"
#include "turrets/_.c"
#include "maps/_.c"
#include "ai/_.c"

// depth 7: placement / top-level
#include "markers/_.c"
#include "vodes/_.c"
#include "dungeons/_.c"
#include "npcs/_.c"
#include "controllers/_.c"
#include "imports/_.c"
#include "editor/_.c"
#include "debug/_.c"
#include "xr/_.c"

// engine imports, besides sub modules, it's core is flecs
void import_zox(ecs* world) {
    byte dbg_disable = 0;
    zox_module(zox);

    // depth 0: foundation
    // core ECS / basic data
    zox_add_module(core);
    zox_import_module(Colorz);
    zox_add_module(transforms);
    zox_import_module(Names);

    // depth 1: engine services
    // application / platform / IO / networking
    zox_add_module(networking);
    zox_import_module(Screens);
    zox_add_module(inputs);
    zox_import_module(Apps);
    zox_import_module(Saves);
    zox_import_module(Graphics);
    zox_import_module(Assets);
    zox_import_module(Realms);
    zox_import_module(Games);
    zox_import_module(Players);
    zox_import_module(Triggers);
    zox_import_module(Nodes);
    zox_import_module(Slots);

    // depth 2: rendering foundations
    // things that build directly on graphics + transforms
    zox_add_module(cameras);
    zox_add_module(rendering);
    zox_add_module(sounds);
    zox_add_module(textures);
    zox_add_module(geometry);
    zox_add_module(musics);
    zox_import_module(Animations);
    zox_import_module(Bones);
    zox_import_module(Raycasts);
    zox_add_module(ui);
    // zox_import_module(Lines2);
    zox_import_module(Plots);
    zox_import_module(Shapes);

    // depth 3: simulation / world foundations
    zox_import_module(Genetics);
    zox_import_module(Neurals);
    zox_import_module(Blueprints);
    zox_add_module(blocks);
    zox_add_module(chunks);
    zox_add_module(voxes);
    zox_add_module(physics);
    zox_add_module(particles);
    zox_import_module(Lights);

    // depth 4: world / environment
    zox_import_module(Models);
    zox_add_module(weathers);
    zox_add_module(streaming);
    zox_import_module(Regions);
    zox_import_module(Tunks);
    zox_import_module(Terrains);
    zox_import_module(Biomes);
    zox_add_module(heights);
    zox_import_module(Vegetation);
    zox_import_module(Mountains);
    zox_import_module(Towns);
    zox_import_module(Vrays);
    zox_add_module(characters);
    zox_add_module(huds);
    zox_import_module(Space);

    // depth 5: character data
    if (!dbg_disable) {
        zox_import_module(Users);   // Remove users
        zox_import_module(Users);
        zox_import_module(Combat);
        zox_import_module(Projectiles);
        zox_add_module(stats);
        zox_add_module(skills);
        zox_add_module(items);
        zox_add_module(parts);
        zox_add_module(equipment);
        zox_add_module(actions);
        zox_add_module(dialogues);
        zox_add_module(quests);
        zox_import_module(Classes);
        zox_import_module(Jobs);
        zox_import_module(Races);
        zox_import_module(Clans);
        zox_import_module(Lores);
        zox_import_module(Achievements);

        // depth 6: gameplay systems
        zox_add_module(pickups);
        zox_add_module(crafting);
        zox_import_module(Farming);
        zox_import_module(Turrets);
        zox_import_module(Maps);
        zox_import_module(Pets);

        // depth 7: world placement / AI
        zox_add_module(markers);
        zox_add_module(ai);
        zox_add_module(vodes);
        zox_add_module(dungeons);
        zox_add_module(npcs);
        zox_add_module(controllers);

        // depth 8: tools / development
        zox_import_module(Debug);
    }

    // depth 9: top-level integration
    zox_import_module(Imports);
    zox_add_module(editor);

    #ifdef zox_xr
    zox_add_module(xr);
    #endif
}
