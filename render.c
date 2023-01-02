#include "vmath.h"
#include "math.h"
#include "obj.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define WIDTH 1024
#define HEIGHT 768
#define MAX_FPS 60
#define MOUSE_SENS 0.005
#define CAMERA_SPEED 0.05
#define CAMERA_ROT_SPEED 0.05

typedef struct {
    size_t edge_count;
    vec3 * verts;
    size_t * edges;
} Model;

typedef struct {
    vec3 view[3];
    vec3 pos;
} Camera;

SDL_Window * window = NULL;
SDL_Renderer * renderer;
int running = 1;

float azimuth = 1.0;
float elevation = 1.0;
float zoom = 0.5;

Camera camera;

Model * model = NULL;

void camera_setrot(float azimuth, float elevation) {
    vec3 fwd;
    fwd.z = cosf(azimuth) * cosf(elevation);
    fwd.x = -sinf(azimuth);
    fwd.y = -cosf(azimuth) * sinf(elevation);

    vec3 right = {
        .y = -cosf(elevation),
        .z = -sinf(elevation)
    };

    vec3 up = v3cross(fwd, right);
    
    camera.view[0] = up;
    camera.view[1] = right;
    camera.view[2] = fwd;
}

void camera_init(float azimuth, float elevation, vec3 position) {
    camera = (Camera) {
        .pos = position
    };
    camera_setrot(azimuth, elevation);
}


vec3 camera_trans(vec3 p) {
    vec3 result;
    result = m3v3mul(camera.view, v3sub(p,camera.pos));
    result = v3mul(result, zoom);
    result = (vec3) { result.x * HEIGHT + (WIDTH/2), result.y * HEIGHT + (HEIGHT/2), result.z };
    return result;
}

void draw_origin() {
    vec3 origin = camera_trans((vec3) { 0, 0, 0 });
    vec3 xunit = camera_trans((vec3) { 1, 0, 0 });
    vec3 yunit = camera_trans((vec3) { 0, 1, 0 });
    vec3 zunit = camera_trans((vec3) { 0, 0, 1 });
    SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
    SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
    SDL_RenderDrawLineF(renderer, origin.x, origin.y, xunit.x, xunit.y);
    SDL_SetRenderDrawColor(renderer, 0x0, 0xff, 0x0, 0xff);
    SDL_RenderDrawLineF(renderer, origin.x, origin.y, yunit.x, yunit.y);
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0xff, 0xff);
    SDL_RenderDrawLineF(renderer, origin.x, origin.y, zunit.x, zunit.y);
}
void draw_camera_origin() {
    vec3 p0 = camera_trans(v3add(camera.pos,(vec3) {-0.25,0,0}));
    vec3 p1 = camera_trans(v3add(camera.pos,(vec3) {0.25,0,0}));
    vec3 p2 = camera_trans(v3add(camera.pos,(vec3) {0,-0.25,0}));
    vec3 p3 = camera_trans(v3add(camera.pos,(vec3) {0,0.25,0}));
    vec3 p4 = camera_trans(v3add(camera.pos,(vec3) {0,0,-0.25}));
    vec3 p5 = camera_trans(v3add(camera.pos,(vec3) {0,0,0.25}));
    SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xff);
    SDL_RenderDrawLineF(renderer, p0.x, p0.y, p1.x, p1.y);
    SDL_RenderDrawLineF(renderer, p2.x, p2.y, p3.x, p3.y);
    SDL_RenderDrawLineF(renderer, p4.x, p4.y, p5.x, p5.y);
}
    
void draw_frame() {
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderClear(renderer);
    for (int i = 0; i < model->edge_count; i+=2) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xff);
        vec3 p0 = camera_trans(model->verts[model->edges[i]-1]);
        vec3 p1 = camera_trans(model->verts[model->edges[i+1]-1]);
        /*for (int j = 0; j < model->edges[i][0]; j++) {
            SDL_RenderDrawPoint(renderer, p0.x + j * 4 + 4, p0.y);
        }*/
        SDL_RenderDrawLineF(renderer, p0.x, p0.y, p1.x, p1.y);
    }
    draw_origin();
    draw_camera_origin();
    SDL_RenderPresent(renderer);
}



int setup_sdl() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
	if (!window) {
		fprintf(stderr,"InitSetup failed to create window");
        running = 0;
        return 0;
	}
	SDL_SetWindowTitle(window, "Example One");
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return 1;
}



void handle_event(SDL_Event * event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = 0;
                    break;
            }
            break;
        case SDL_MOUSEWHEEL:
            zoom += ((float) event->wheel.y)/20.0;
            camera_setrot(azimuth, elevation);
            break;
        case SDL_MOUSEMOTION:
            if (event->motion.state & SDL_BUTTON_RMASK) {
                azimuth += (float)event->motion.xrel * MOUSE_SENS;
                elevation += (float)event->motion.yrel * MOUSE_SENS;
            }
            camera_setrot(azimuth, elevation);
            break;
        case SDL_QUIT:
            running = 0;
            break;
    }
}

#define CAM_ROT_LEFT SDL_SCANCODE_LEFT
#define CAM_ROT_RIGHT SDL_SCANCODE_RIGHT
#define CAM_ROT_UP SDL_SCANCODE_UP
#define CAM_ROT_DOWN SDL_SCANCODE_DOWN

#define CAM_UP SDL_SCANCODE_LSHIFT
#define CAM_DOWN SDL_SCANCODE_LCTRL
#define CAM_FWD SDL_SCANCODE_W
#define CAM_BACK SDL_SCANCODE_S
#define CAM_RIGHT SDL_SCANCODE_D
#define CAM_LEFT SDL_SCANCODE_A

void handle_keys() {
    vec3 fwd;
    vec3 right;
    const Uint8* kb_state = SDL_GetKeyboardState(NULL);
    if (kb_state[CAM_ROT_LEFT]) {
        azimuth += CAMERA_ROT_SPEED;
    } if (kb_state[CAM_ROT_RIGHT]) {
        azimuth -= CAMERA_ROT_SPEED;
    } if (kb_state[CAM_ROT_UP]) {
        elevation += CAMERA_ROT_SPEED;
    } if (kb_state[CAM_ROT_DOWN]) {
        elevation -= CAMERA_ROT_SPEED;
    } if (kb_state[CAM_UP]) {
        camera.pos.y += CAMERA_SPEED;
    } if (kb_state[CAM_DOWN]) {
        camera.pos.y -= CAMERA_SPEED;
    } if (kb_state[CAM_FWD]) {
        fwd = (vec3) { sinf(azimuth), 0, cosf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(fwd, CAMERA_SPEED));
    } if (kb_state[CAM_BACK]) {
        fwd = (vec3) { sinf(azimuth), 0, cosf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(fwd, -CAMERA_SPEED));
    } if (kb_state[CAM_RIGHT]) {
        right = (vec3) { cosf(azimuth), 0, -sinf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(right, CAMERA_SPEED));
    } if (kb_state[CAM_LEFT]) {
        right = (vec3) { cosf(azimuth), 0, -sinf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(right, -CAMERA_SPEED));
    }
    camera_setrot(azimuth, elevation);
}


const int MIN_CYCLES_PER_FRAME = (1.0/MAX_FPS) * (float) CLOCKS_PER_SEC;

void loop() {
    SDL_Event event;
	while (running) {
        clock_t t = clock();
        SDL_PumpEvents();
		while (SDL_PollEvent(&event)) {
            handle_event(&event);
		}
        handle_keys();
        draw_frame();
        while (clock() - t < MIN_CYCLES_PER_FRAME) { /*wait*/ }
	}
}

void finish() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	exit(0);
}

int main(int argc,char * args[])
{
    FILE * fp;
    fp = fopen("example_models/utah_teapot.obj", "r");

    vec3 * verts;
    size_t * edges;
    size_t edge_count = parse_obj(fp, &verts, &edges);
    model = &(Model) {
        .edges = edges,
        .verts = verts,
        .edge_count = edge_count,
    };


	setup_sdl();
    camera_init(1.0, 1.0, (vec3) { 0, 0, 0 });
	loop();
	finish();
    return 0;
}
