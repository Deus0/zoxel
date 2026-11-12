// linear drag of Frictioned 2D entities, velocity decays as e^(-friction2D t), see physics/fun
float friction2D = 8.0f;

// keeps an angle within (-2pi, 2pi) without changing its direction
static inline float physics2_wrap_angle(float angle) {
    const float turn = (float) (2 * M_PI);
    if (angle >= turn || angle <= -turn) {
        angle = fmodf(angle, turn);
    }
    return angle;
}
