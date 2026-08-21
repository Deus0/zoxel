// more fun / unused for now
static inline plane plane_from_float4(const float4 v) {
    return (plane) { { v.x, v.y, v.z }, v.w };
}

/*static void normalize_plane(plane *p) {
    double nx = p->normal.x;
    double ny = p->normal.y;
    double nz = p->normal.z;
    double magnitude = sqrt(nx*nx + ny*ny + nz*nz);
    if (!magnitude) magnitude = 1;
    double invLen = 1.0 / magnitude;
    p->normal.x = (float)(nx * invLen);
    p->normal.y = (float)(ny * invLen);
    p->normal.z = (float)(nz * invLen);
    p->distance = (float)(p->distance * invLen);
}*/