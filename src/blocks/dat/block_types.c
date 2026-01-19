// TODO: Fix Bug: if there is non texture models  before texture bake ones, it fails ! we will get a white block.
// TODO: Make it dynamic, no need for these, link biomes up with them instead

// temp lookup for voxels
// #define zox_block_air 0
byte zox_block_sand = 3;
byte zox_block_stone = 4;
byte zox_block_obsidian = 5;
byte zox_block_ore = 6;
byte zox_block_bricks = 8;
byte zox_block_wood = 9;

// gamey
byte zox_block_dark = 6;
byte zox_block_dungeon_core = 7;

// decor
byte zox_block_dirt_vox = 10;
byte zox_block_dirt_rubble = 11;
byte zox_block_vox_grass = 12;
byte zox_block_vox_flower = 13;
byte zox_block_dirt_flowers = 14;
//#define zox_block_stone 4
//#define zox_block_obsidian 5 // apparently doesn't work for textures
//#define zox_block_ore 6 // apparently doesn't work for textures
// #define zox_block_wood 7 // apparently doesn't work for textures

// more Blocks
// #define zox_block_bricks 8

#define zox_blocks_end 15 // take 1 off for air
