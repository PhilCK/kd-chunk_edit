#include <string.h>
#include <stdio.h>

#define CHUNK_IMPL
#include <chunk.h>


void
print_entities_chunk(struct chunk_header *c) {
        uint8_t *dat = (uint8_t*)c;
        struct chunk_data *d = &c->data[CHUNK_ENTITIES];
        struct chunk_entity *e = (struct chunk_entity *)&dat[d->src_offset];
        int count = d->src_bytes / sizeof(*e);
        int i;

        for(i = 0; i < count; ++i) {
                printf("Entity: %s\n", e[i].name);
                printf(" -renderable: %s\n", e[i].rdr_idx >= 0 ? "YES" : "NO");
                printf(" -collider: %s\n", e[i].coll_idx >= 0 ? "YES" : "NO");

                if(i + 1 != count) {
                        printf("====================\n");
                }
        }
}


int
main() {
        /* write out some data */
        struct chunk_entity ents[2] = {0};
        memcpy(ents[0].name, "foo", sizeof("foo"));
        ents[0].rdr_idx = 0;
        ents[0].coll_idx = -1;

        memcpy(ents[1].name, "bar", sizeof("bar"));
        ents[1].rdr_idx = -1;
        ents[1].coll_idx = 0;
        
        struct chunk_renderable rdrs[1] = {0};
        struct chunk_collider colls[1] = {0};

        struct chunk_export xprt = {0};
        xprt.ents = ents;
        xprt.ents_count = 2;

        xprt.rdr = rdrs;
        xprt.rdr_count = 1;

        xprt.coll = colls;
        xprt.coll_count = 1;

        chunk_write_out(&xprt, "test.dat");

        /* read back some data */
        struct chunk_header *h = 0;
        h = chunk_read_in("test.dat");
        print_entities_chunk(h);

        return 0;
}