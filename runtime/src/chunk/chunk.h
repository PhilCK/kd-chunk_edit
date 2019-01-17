#ifndef CHUNK_INCLUDED
#define CHUNK_INCLUDED


#include <stdint.h>


#define MAX_CHUNK_DATA 16


#define CHUNK_ENTITIES 1
#define CHUNK_RENDERABLES 2
#define CHUNK_COLLIDERS 3


struct chunk_data {
        uint32_t src_offset;
        uint32_t src_bytes;
};


#define CHUNK_VERSION 0x1


struct chunk_header {
        uint32_t version;
        struct chunk_data data[MAX_CHUNK_DATA];
};


struct chunk_entity {
        char name[16];

        int rdr_idx;
        int coll_idx;
};


struct chunk_renderable {
        float world[16];
};


struct chunk_collider {
        float aabb[3];
};



struct chunk_export {
        struct chunk_entity *ents;
        int ents_count;

        struct chunk_renderable *rdr;
        int rdr_count;

        struct chunk_collider *coll;
        int coll_count;
};


int
chunk_write_out(
        struct chunk_export *ch_ex,
        const char *file);


struct chunk_header*
chunk_read_in(
        const char *file);


#endif


/* ================================== IMPL ================================== */


#ifdef CHUNK_IMPL
#ifndef CHUNK_IMPL_INCLUDED
#define CHUNK_IMPL_INCLUDED


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define CHUNK_DATA_PADDING 16


int
chunk_copy(
        struct chunk_data *d,
        void *buf,
        int off,
        void *src,
        int cpy_bytes)
{
        if(!src) {
                return 0;
        }

        memcpy(buf, src, cpy_bytes);
        d->src_offset = off;
        d->src_bytes = cpy_bytes;

        return cpy_bytes + CHUNK_DATA_PADDING;
}


int
chunk_write_out(
        struct chunk_export *ch,
        const char *out_path)
{
        /* calculate size */
        int size_needed = 0;
        size_needed += sizeof(struct chunk_header);
        size_needed += CHUNK_DATA_PADDING;
        size_needed += sizeof(ch->ents) * ch->ents_count;
        size_needed += CHUNK_DATA_PADDING;
        size_needed += sizeof(ch->rdr) * ch->rdr_count;
        size_needed += CHUNK_DATA_PADDING;

        uint8_t *buf = (uint8_t*)malloc(size_needed);
        memset(buf, 0, size_needed);

        size_t used = 0;
        int cpy_bytes = 0;

        struct chunk_header *head = (struct chunk_header*)buf;
        head->version = CHUNK_VERSION;

        used += sizeof(*head);

        /* entities */
        used += chunk_copy(
                &head->data[CHUNK_ENTITIES],
                &buf[used],
                used,
                ch->ents,
                sizeof(ch->ents[0]) * ch->ents_count);

        /* renderables */
        used += chunk_copy(
                &head->data[CHUNK_RENDERABLES],
                &buf[used],
                used,
                ch->rdr,
                sizeof(ch->rdr[0]) * ch->rdr_count);

        /* colliders */
        used += chunk_copy(
                &head->data[CHUNK_COLLIDERS],
                &buf[used],
                used,
                ch->coll,
                sizeof(ch->coll[0]) * ch->coll_count);

        /* write chunk data */
        FILE *f = fopen(out_path, "wb");

        if(!f) {
                return 0;
        }

        int i;
        for(i = 0; i < size_needed; ++i) {
                fprintf(f, "%c", buf[i]);
        }

        free(buf);
        fclose(f);

        return 1;
}


struct chunk_header *
chunk_read_in(
        const char *file)
{
        FILE *f = fopen(file, "rb");

        if(!f) {
                return 0;
        }

        fseek(f, 0, SEEK_END);
        unsigned length = ftell(f);
        fseek (f, 0, SEEK_SET);

        uint8_t *buf = malloc(length);

        if(!buf) {
                return 0;
        }

        fread(buf, 1, length, f);
        fclose(f);

        return (struct chunk_header*)buf;
}


#undef CHUNK_DATA_PADDING


#endif
#endif
