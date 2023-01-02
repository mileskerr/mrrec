#include "vmath.h"
#include "obj.h"

void parse_face(char * nptr, int res[16]) {
    char buf[16] = { 0 };
    int ptri, bi, ri;
    ptri = bi = ri = 0;
    while (nptr[ptri]) {
        if (nptr[ptri] == '/') {
            while (nptr[ptri] != ' ') { //skip to the next group because texture coordinates and normals aren't important to us
                ptri++;
            }
        } else if (nptr[ptri] == ' ') {
            buf[bi] = 0;
            res[ri] = atoi(buf);
            bi = 0;
            ri++;
            ptri++;
        } else {
            buf[bi] = nptr[ptri];
            bi++;
            ptri++;
        }
    }
    res[ri] = 0;
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
    {
        int vi = 0;
        int ei = 0;
        char buf[80];
        while (1) {
            if (fgets(buf, 80, fp) == NULL) { break; }
            if (buf[0] == 'v' && buf[1] == ' ') {
                //printf("%s",buf);
                char * p = buf + sizeof(char);
                errno = 0;
                float x = strtof(p, &p);
                float y = strtof(p, &p);
                float z = strtof(p, &p);
                if (!errno) {
                    (*verts)[vi] = (vec3) { x,y,z };
                    vi++;
                } else { fprintf(stderr, "error\n"); }
            } 
            if (buf[0] == 'f' && buf[1] == ' ') {
                //printf("%s",buf);
                int face[16];
                parse_face(buf + 2, face);

                int fi = 0;
                while (face[fi+1]) {
                    (*edges)[ei] = face[fi];
                    (*edges)[ei+1] = face[fi+1];
                    ei+=2;
                    fi++;
                }
            } 
        }
    }
    return face_count * 6;
}
