#ifndef zoxm_space
#define zoxm_space

// one day we will actually make this abuot space
zox_tag(Planet);
zox_tag(Asteroid);
zox_tag(Star);
#include "fun/_.c"

zox_begin_module(Space)
    zoxd_tag(Planet);
    zoxd_tag(Asteroid);
    zoxd_tag(Star);

    // TODO: Refactor per systems:
    //  - Player Characters
    //  - Game Audio
    //  - Game UI
    add_to_event_game_state((zox_game_event) { &players_game_state });

zox_end_module(Space)

#endif