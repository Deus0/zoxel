# Game Design

Zoxel blends favourite games together, and tries to elevate the craft of each.

## The Blend

| Game | What Zoxel takes from it |
|---|---|
| **Dwarf Fortress** | a simulated world: regions, populations, towns, npcs with lives, families and grudges |
| **World of Warcraft** | the rpg: quests from npcs, combat, skills, stats, levels, loot |
| **Minecraft** | a voxel world to dig, build and craft in, generated forever |
| **Sonic Chao** | creatures you raise: they grow, change with care, breed, and get attached to you |
| **Pokemon** | catching and summoning creatures (zoxballs), a creature catalogue to fill |

Also inspired by: Warcraft, Cube World, Age of Empires, Slime Rancher, Runescape, Hollow Knight,
Hyperlight Drifter.

## Pillars

- **A living world**: npcs aren't props. They have voices, needs, affection and hostility, clans and
  families. A world that runs without you, that you step into.
- **Everything generated**: terrain, characters, their body parts, voices, songs. Every seed a new world.
- **An alien place**: a world that feels foreign. Npcs babble their own languages (the dialogue is a
  translator), only major cities share a common tongue (see [future.md](future.md)).
- **Craft in the details**: sound and feel matter, Hollow Knight like instruments, songs composed like
  dungeons, a jingle for a level up, an item that pops when it drops.

## Systems

| System | Status | Docs |
|---|---|---|
| Voxel terrain, digging and placing | playable | [todo.md](todo.md) |
| Characters, body parts, skeletons | playable | [character.md](character.md) |
| Combat, skills, stats, levels | playable | |
| Quests and dialogue | playable | [nodes.md](nodes.md) |
| Npc voices, music, 3D sound | playable | [audio.md](audio.md) |
| Items, inventory, crafting | in progress | [new.md](new.md#crafting) |
| Towns and villager jobs | planned | [new.md](new.md#towns) |
| Genetics, breeding, affection | planned | [new.md](new.md#genetics) |
| Pets and raising creatures | planned | [new.md](new.md#pets) |
| Catching and summoning | planned | [goals.md](goals.md) |
| Alien languages | future | [future.md](future.md) |
| Multiplayer | planned | [networking_notes.md](networking_notes.md) |

## The Quest Loop

The first full loop, from [goals.md](goals.md):

1. An npc shows `!`, it has a quest.
2. Talk to them, the dialogue offers it. The marker turns to a gray `?`.
3. Slay the slimes, loot the cookies.
4. The `?` turns yellow, the quest is done. Return and talk to them.
5. Hand it in: a jingle, particles, xp and money.

Steps 1 to 4 play now (tst quest_dialogue). Rewards and the hand in jingle are next.
