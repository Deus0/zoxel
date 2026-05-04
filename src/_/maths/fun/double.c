static inline double double_div(double v, double d) {
    if (!d) {
        return v;
    }
    return v / d;
}