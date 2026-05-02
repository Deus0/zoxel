#ifdef zox_sdl_images

// Assuming TextureData and TextureSize are defined as they are in your save function
// NOTE: SDL considers top left origin, while opengl is bottom left - Flip the image vertically

byte load_texture_from_png(const char *filepath, TextureData* data, int2 *size) {
    SDL_Surface* surface = IMG_Load(filepath);

    if (!surface) {
        zox_log(" ! failed with [IMG_Load]: %s\n", SDL_GetError())
        return 0;
    }

    int pitch = surface->pitch;
    byte* source = (byte*) surface->pixels;
    size->x = surface->w;
    size->y = surface->h;

    resize_TextureData(data, size->x * size->y);
    for (uint y = 0; y < size->y; ++y) {
        memcpy(data->value + (size->y - 1 - y) * size->x, source + y * pitch, size->x * sizeof(color));
    }

    SDL_FreeSurface(surface);

    return 1;
}

void save_texture_as_png(const color *data, const int2 size, const char *filepath) {
    int rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = 0; // (req_format == STBI_rgb) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
#else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0; // (req_format == STBI_rgb) ? 0 : 0xff000000;
#endif

    int depth = 32;
    int pitch = size.x * 4;

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*) data, size.x, size.y, depth, pitch, rmask, gmask, bmask, amask);
    if(IMG_SavePNG(surface, filepath) != 0) {
        // Error saving bitmap
        zox_log(" ! failed with [IMG_SavePNG]: %s\n", SDL_GetError())
    }

    SDL_FreeSurface(surface);
}

#else

byte load_texture_from_png(const char *filepath, TextureData* data, int2 *size) {
    zox_log("Image Load Disabled");
    return 1;
}

void save_texture_as_png(const color *data, const int2 size, const char *filepath) {
    zox_log("Image Save Disabled");
}

#endif
