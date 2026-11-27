byte auto_player = 0;
// shit remove this atm, it doubles up for coop
double run_speed = 1.67;          // running's top speed, of walking's (the old move reached 1.67 at 60fps)
float crouch_speed = 0.45f;     // crouching, of the walk's speed
double run_accceleration = 2.2;
float backwards_multiplier = 0.8f;
float2 player_movement_power = { 17, 19 };
float2 max_velocity3D = { 0.99f, 1.43f };    // top speeds, side and forward: the old move's at 60fps (tst move_feel)
// turning to face its move (controllers3/sys/move.c character3_turn): by physics, a torque on its spin as npcs turn,
//  to the spin that closes the gap at its rate
float player_turn_rate = 15;        // radians a second of spin for each radian off (the gap ~1/15s to close by e)
float player_turn_max = 800;        // the most angular acceleration, radians a second squared
// a headset's stick (doc/tech/vr.md): its view moves at the stick's speed at once, steady, stopped at once (a view
//  speeding up or sliding sickens); the body walks after it by the move like any character (xr/fun/body.c)
float3 xr_stick_velocity = { 0, 0, 0 };     // the view's this frame, world, flat (movement.c writes it)
float3 xr_stick_lead = { 0, 0, 0 };         // how far the view is ahead of the body, world, flat (xr/fun/body.c)
float xr_stick_turn = 0;                    // the right stick's turn of the body this frame, radians about its up
                                            //  (rotation.c): xr/fun/body.c turns the view about the head by it
float xr_stick_catch = 6;                   // the body's push toward the view: metres a second for each metre behind
// the speed stat (StatSpeed, 1 a player's start) to its real speed, a curve with diminishing returns: over 1 it
//  gains up to player_speed_bonus_max more, half of that at player_speed_bonus_half over 1; under 1 it slows as is
float player_speed_bonus_max = 1;       // at most twice as fast
float player_speed_bonus_half = 1;      // stat 2: 1.5x, stat 4: 1.75x
const float min_mouse_delta3 = 0.01f;
const float min_mouse_delta2 = 24.0f;
const float max_mouse_delta3 = 60.0f;
const float max_mouse_delta4 = 120.0f;
const float max_mouse_delta2 = 200.0f;
const double max_rotate_speed = 0.2; //  0.23;
// Flymode
double fly_run_speed = 4.2;
double fly_run_acc_y = 5.2;
double fly_run_acc_xz = 15.2;
// first person, the mouse's (and right stick's) turn: at once (0, the old turn, the fallback) or by physics (1, setting
//  "Physics Turn"): it turns the way it wants to face (FaceDirection), the move turns it after by a torque
//  (character3_turn); never in a headset (a turn easing sickens)
byte player_physics_turn = 0;
byte auto_jump = 0;             // jumps up a block walked into (setting "Auto Jump"), on by default with a touchscreen
