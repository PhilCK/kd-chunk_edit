#ifndef CHUNK_INCLUDED
#define CHUNK_INCLUDED


#include <stdint.h>


#define MAX_CHUNK_DATA 16


struct chunk_data {
        uint32_t src_offset;
        uint32_t src_bytes;
};


struct chunk_header {
        uint64_t version;
        struct chunk_data data[MAX_CHUNK_DATA];
};


struct chunk_export {
        uint32_t chunk_idx;
        void *data;
        size_t bytes;
};


int
chunk_write_out(
        struct chunk_export *ch_ex,
        int ch_ex_count,
        uint64_t version_tag,
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
        struct chunk_export *ch_ex,
        int ch_ex_count,
        uint64_t version_tag,
        const char *file)
{
        int i;

        /* calculate size */
        int size_needed = 0;
        size_needed += sizeof(struct chunk_header);
        size_needed += CHUNK_DATA_PADDING;

        for(i = 0; i < ch_ex_count; ++i) {
                size_needed += ch_ex[i].bytes;
                size_needed += CHUNK_DATA_PADDING;
        }

        uint8_t *buf = (uint8_t*)malloc(size_needed);
        memset(buf, 0, size_needed);

        size_t used = 0;
        int cpy_bytes = 0;

        struct chunk_header *head = (struct chunk_header*)buf;
        head->version = version_tag;

        used += sizeof(*head);

        /* entities */
        for (i = 0; i < ch_ex_count; ++i) {
                used += chunk_copy(
                        &head->data[ch_ex[i].chunk_idx],
                        &buf[used],
                        used,
                        ch_ex[i].data,
                        ch_ex[i].bytes);
        }

        /* write chunk data */
        FILE *f = fopen(file, "wb");

        if(!f) {
                return 0;
        }

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

        uint8_t *buf = (uint8_t*)malloc(length);

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
