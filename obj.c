#include "vmath.h"
#include "obj.h"

int parse_face(char * nptr, int res[16]) {
    char buf[16] = { 0 };
    int ptri, bi, ri;
    ptri = bi = ri = 0;
    while (nptr[ptri]) {
        if (nptr[ptri] == '/') {
            buf[bi] = 0;
            res[ri] = atoi(buf);
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
    res[ri] = 0;
    return ri;
}


size_t parse_obj(FILE * fp, vec3 ** verts, size_t ** edges) {

    rewind(fp);

    int vert_count = 0;
    int face_count = 0;

    { //count verts and faces
        char ch;
        char buf[4] = { 0, 0, 0, 0 };
        while ((ch = fgetc(fp)) != EOF) {
            buf[0] = buf[1];
            buf[1] = buf[2];
            buf[2] = ch;
            if (!strcmp(buf,"\nv ")) {
                vert_count++;
            }
            if (!strcmp(buf,"\nf ")) {
                face_count++;
            }
        }
    }


    *verts = malloc(vert_count * sizeof(vec3));
    *edges = malloc(face_count * 6 * sizeof(size_t));

    rewind(fp);
    int ei = 0;
    {
        int vi = 0;
        char buf[201];
        while (1) {
            if (fgets(buf, 200, fp) == NULL) { break; }
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
                int face[16] = {0};
                int vert_count = parse_face(buf + 2, face);

                for (int fi = 0; fi < vert_count; fi++) {
                    size_t p0 = face[fi];
                    size_t p1 = face[(fi+1) % vert_count];
                    for (int ci = 0; ci < ei; ci+=2) {
                        if (
                            (((*edges)[ci+1] == p0) && ((*edges)[ci] == p1)) ||
                            (((*edges)[ci] == p0) && ((*edges)[ci+1] == p1))
                        ) {
                            goto edge_is_a_duplicate;
                        }
                    }
                    (*edges)[ei] = p0;
                    (*edges)[ei+1] = p1;
                    ei+=2;
edge_is_a_duplicate:
                }
            } 
        }
    }
    return ei;
}
