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
#include "screens/_.c"
#include "inputs/_.c"
#include "graphics/_.c"
#include "apps/_.c"
#include "names/_.c"
#include "saves/_.c"

// depth 1: basic world blocks
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
#include "animations/_.c"
#include "raycasts/_.c"
#include "shapes/_.c"
#include "textures/_.c"
#include "geometry/_.c"
#include "musics/_.c"
#include "bones/_.c"
#include "ui/_.c"

// depth 3: simulation / world
#include "plots/_.c"
#include "genetics/_.c"
#include "neurals/_.c"
// #include "blueprints/_.c"
#include "blocks/_.c"
#include "chunks/_.c"
#include "physics/_.c"
#include "lights/_.c"
#include "models/_.c"
#include "voxes/_.c"

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
#include "combat/_.c"
#include "projectiles/_.c"
#include "stats/_.c"
#include "skills/_.c"
#include "items/_.c"
#include "parts/_.c"
#include "equipment/_.c"
#include "lores/_.c"
#include "achievements/_.c"

// depth 6: gameplay
#include "actions/_.c"
#include "classes/_.c"
#include "jobs/_.c"
#include "races/_.c"
#include "clans/_.c"
#include "quests/_.c"
#include "dialogues/_.c"
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

// TODO: Remove Modules from here
// - Add to core
//      - colorz, names, slots, saves, shapes, nodes
// - Delete these
//      - vrays, screens
// - Move plots to ui
// - Move lights into chunks
// - Move Combat to Stats

// engine imports, besides sub modules, it's core is flecs
void import_zox(ecs* world) {
    byte dbg_disable = 0;
    zox_module(zox);

    // depth 0: foundation
    // core ECS / basic data
    zox_add_module(core);

    // depth 1: engine services
    // application / platform / IO / networking
    zox_add_module(networking);
    zox_add_module(inputs);
    zox_add_module(apps);
    zox_add_module(graphics);
    zox_add_module(realms);
    zox_add_module(games);
    zox_add_module(players);
    zox_add_module(transforms);

    // Misc stuff
    zox_add_module(names);
    zox_add_module(colorz);
    zox_add_module(screens);
    zox_add_module(nodes);
    zox_add_module(saves);
    zox_add_module(slots);
    zox_add_module(shapes);

    // depth 2: rendering foundations
    // things that build directly on graphics + transforms
    zox_add_module(cameras);
    zox_add_module(rendering);
    zox_add_module(sounds);
    zox_add_module(textures);
    zox_add_module(geometry);
    zox_add_module(bones);
    zox_add_module(animations);
    zox_add_module(musics);
    zox_add_module(ui);

    // More Misc
    zox_add_module(raycasts);
    zox_add_module(plots);

    // depth 3: simulation / world foundations
    zox_add_module(genetics);
    zox_add_module(neurals);
    zox_add_module(blocks);
    zox_add_module(chunks);
    zox_add_module(lights);
    zox_add_module(physics);
    zox_add_module(particles);

    // depth 4: world / environment
    zox_add_module(models);
    zox_add_module(weathers);
    zox_add_module(streaming);
    zox_add_module(regions);
    zox_add_module(tunks);
    zox_add_module(terrains);
    zox_add_module(biomes);
    zox_add_module(heights);
    zox_add_module(vegetation);
    zox_add_module(mountains);
    zox_add_module(towns);
    zox_add_module(characters);
    zox_add_module(vrays);
    zox_add_module(huds);
    zox_add_module(space);
    zox_add_module(projectiles);

    // depth 5: character data
    if (!dbg_disable) {
        zox_add_module(combat);
        zox_add_module(stats);
        zox_add_module(skills);
        zox_add_module(items);
        zox_add_module(parts);
        zox_add_module(equipment);
        zox_add_module(actions);
        zox_add_module(quests);
        zox_add_module(dialogues);
        zox_import_module(Classes);
        zox_import_module(Jobs);
        zox_import_module(Races);
        zox_import_module(Clans);
        zox_import_module(Lores);
        zox_add_module(achievements);

        // depth 6: gameplay systems
        zox_add_module(pickups);
        zox_add_module(crafting);
        zox_add_module(maps);
        zox_import_module(Farming);
        zox_import_module(Turrets);
        zox_import_module(Pets);

        // depth 7: world placement / AI
        zox_add_module(markers);
        zox_add_module(ai);
        zox_add_module(vodes);
        zox_add_module(dungeons);
        zox_add_module(npcs);
        zox_add_module(controllers);

        // depth 8: tools / development
        zox_add_module(debug);
    }

    // depth 9: top-level integration
    zox_add_module(imports);
    zox_add_module(editor);

#ifdef zox_xr
    zox_add_module(xr);
#endif
}
