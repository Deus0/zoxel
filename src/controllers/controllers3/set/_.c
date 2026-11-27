byte auto_player = 0;
// shit remove this atm, it doubles up for coop
double run_speed = 1.67;          // running's top speed, of walking's (the old move reached 1.67 at 60fps)
float crouch_speed = 0.45f;     // crouching, of the walk's speed
double run_accceleration = 2.2;
float backwards_multiplier = 0.8f;
float2 player_movement_power = { 17, 19 };
float2 max_velocity3D = { 0.99f, 1.43f };    // top speeds, side and forward: the old move's at 60fps (tst move_feel)
// turning to face its move (controllers3/sys/move.c): by physics, a torque on its spin as npcs turn (a spring to the
//  way it wants, damped), or at once (the old turn, the fallback if play tests don't like the new one)
byte player_turn_instant = 0;
float player_turn_strength = 400;   // the spring: angular acceleration a radian off
float player_turn_damping = 30;     // against its spin (critically damped near 2 * sqrt(strength))
float player_turn_max = 800;        // the most angular acceleration, radians a second squared
// a headset's stick (doc/tech/vr.md): its view moves at the stick's speed at once, steady, stopped at once (a view
//  speeding up or sliding sickens); the body walks after it by the move like any character (xr/fun/body.c)
float3 xr_stick_velocity = { 0, 0, 0 };     // the view's this frame, world, flat (movement.c writes it)
float3 xr_stick_lead = { 0, 0, 0 };         // how far the view is ahead of the body, world, flat (xr/fun/body.c)
float xr_stick_turn = 0;                    // the right stick's turn of the body this frame, radians about its up
                                            //  (rotation.c): xr/fun/body.c turns the view about the head by it
float xr_stick_catch = 6;                   // the body's push toward the view: metres a second for each metre behind
// the speed stat (StatSpeed, 1 a player's start) over 1 raises its top speed and push by this much of it
float player_speed_stat_scale = 0.5f;
const float min_mouse_delta3 = 0.01f;
const float min_mouse_delta2 = 24.0f;
const float max_mouse_delta3 = 60.0f;
const float max_mouse_delta4 = 120.0f;
const float max_mouse_delta2 = 200.0f;
const double max_rotate_speed = 0.2; //  0.23;
#define disable_player_rotate_alpha_force
// Flymode
double fly_run_speed = 4.2;
double fly_run_acc_y = 5.2;
double fly_run_acc_xz = 15.2;
byte auto_jump = 0;             // jumps up a block walked into (setting "Auto Jump"), on by default with a touchscreen
