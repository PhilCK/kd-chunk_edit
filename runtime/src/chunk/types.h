#ifndef CHUNK_TYPES_INCLUDED
#define CHUNK_TYPES_INCLUDED


#define CHUNK_HEADER 123
#define CHUNK_TRANSFORMS 1
#define CHUNK_VERSION 0xdead

struct ch_transform {
        float x, y, z;
};


#endif
