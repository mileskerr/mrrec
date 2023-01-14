#include "vmath.h"
#include "obj.h"
#include <stdint.h>

#define MAX_NGON 30

int parse_face(char * nptr, size_t res[MAX_NGON]) {
    char buf[16] = { 0 };
    int ptri, bi, ri;
    ptri = bi = ri = 0;
    while (nptr[ptri]) {
        if (nptr[ptri] == '/') {
            buf[bi] = 0;
            if (res != NULL) res[ri] = atoi(buf);
            bi = 0;
            ri++;
            while ((nptr[ptri] != ' ') && nptr[ptri]) { //skip to the next group because texture coordinates and normals aren't important to us
                ptri++;
            }
        } else {
            buf[bi] = nptr[ptri];
            bi++;
            ptri++;
        }
    }
    if (res != NULL) res[ri] = 0;
    return ri;
}

struct EdgeLL {
    float p0;
    float p1;
    struct EdgeLL * next;
};

typedef struct EdgeLL EdgeLL;

uint32_t hash_edge(float p0, float p1) {
    return (*(uint32_t*) &p0) + (*(uint32_t*) &p1);
}

int edge_ll_find(EdgeLL * ptr, float p0, float p1) {
    for (;;) {
        if (ptr == NULL) return 0;
        if (((p0 == ptr->p0) && (p1 == ptr->p1)) || ((p1 == ptr->p0) && (p0 == ptr->p1))) {
            return 1;
        }
        ptr = ptr->next;
    }
}

void edge_ll_insert(EdgeLL ** head, float p0, float p1) {
    EdgeLL * node = *head;
    *head = malloc(sizeof(EdgeLL));
    **head = (EdgeLL) {
        .p0 = p0,
        .p1 = p1,
        .next = node,
    };
}

size_t parse_obj(FILE * fp, vec3 ** verts, size_t ** edges) {

    rewind(fp);

    size_t vert_count = 0;
    size_t face_count = 0;
    size_t edge_count = 0;

    {
        size_t linenr = 0;
        char buf[200] = { 0 };
        while (fgets(buf, 200, fp) != NULL) {
            if ((buf[0] == 'v') && (buf[1] == ' ')) {
                vert_count++;
            } else if ((buf[0] == 'f') && (buf[1] == ' ')) {
                face_count++;
                edge_count += parse_face(buf + 2, NULL);
            }
            linenr++;
        }
    }

    *verts = malloc(vert_count * sizeof(vec3));
    *edges = malloc(edge_count * 2 * sizeof(size_t));

    const int EDGE_SET_LEN = 50000;

    EdgeLL * edge_set[EDGE_SET_LEN];
    for (int i = 0; i<EDGE_SET_LEN; i++) edge_set[i] = NULL;


    rewind(fp);
    size_t iteration = 0;
    size_t ei = 0;
    {
        size_t vi = 0;
        char buf[201];
        while (fgets(buf,200,fp) != NULL) {
            if (buf[0] == 'v' && buf[1] == ' ') {
                char * p = buf + sizeof(char);
                errno = 0;
                float x = strtof(p, &p);
                float y = strtof(p, &p);
                float z = strtof(p, &p);
                if (!errno) {
                    (*verts)[vi] = (vec3) { x,y,z };
                    vi++;
                } else { fprintf(stderr, "error\n"); }
            } else if (buf[0] == 'f' && buf[1] == ' ') {
                size_t face[MAX_NGON] = {0};
                size_t vert_count = parse_face(buf + 2, face);

                for (int fi = 0; fi < vert_count; fi++) {
                    size_t p0 = face[fi];
                    size_t p1 = face[(fi+1) % vert_count];
                    uint32_t hash = hash_edge(p0,p1) % EDGE_SET_LEN;
                    if (!edge_ll_find(edge_set[hash],p0,p1)) {
                        edge_ll_insert(&edge_set[hash],p0,p1);
                        (*edges)[ei] = p0;
                        (*edges)[ei+1] = p1;
                        ei+=2;
                    }
                }
            } 
            iteration++;
        }
    }
    return ei;
}
