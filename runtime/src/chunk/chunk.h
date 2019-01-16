#ifndef CHUNK_INCLUDED
#define CHUNK_INCLUDED


#include <stdint.h>


#define MAX_CHUNK_DATA 16

#define CHUNK_ENTITIES 1
#define CHUNK_RENDERABLES 2


struct chunk_data {
        /* data */
        uint32_t src_offset;
        uint32_t src_bytes;

        /* entity map - optional */
        uint32_t em_offset;
        uint32_t em_bytes;
};


#define CHUNK_VERSION 0x1


struct chunk {
        uint32_t version;
        struct chunk_data data[MAX_CHUNK_DATA];
};


struct chunk_entities {
        char name[16];
};


struct chunk_renderables {
        float world[16];
};


void


#endif

#ifdef CHUNK_IMPL
#ifndef CHUNK_IMPL_INCLUDED
#define CHUNK_IMPL_INCLUDED





#endif
#endif
