// more fun / unused for now

static void normalize_plane(plane *p) {
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
}