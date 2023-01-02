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

typedef struct {
    size_t edge_count;
    vec3 * verts;
    size_t * edges;
} Model;

typedef struct {
    vec3 view[3];
} Camera;

SDL_Window * window = NULL;
SDL_Renderer * renderer;
int running = 1;

float azimuth = 1.0;
float elevation = 1.0;
float zoom = 0.5;

Camera camera;

Model * model = NULL;
/*Model * model = &(Model) {
    .points = {
        { 0.5, 0.5, 0.5 },   //0
        { 0.5, 0.5, -0.5 },  //1
        { 0.5, -0.5, -0.5 }, //2
        { 0.5, -0.5, 0.5 },  //3
        { -0.5, 0.5, 0.5 },  //4
        { -0.5, 0.5, -0.5 }, //5
        { -0.5, -0.5, -0.5 },//6
        { -0.5, -0.5, 0.5 }, //7
    },
    .edges = {
        0,1,
        1,2,
        2,3,
        3,0,
        0,4,
        1,5,
        2,6,
        3,7,
        4,5,
        5,6,
        6,7,
        7,4
    }
};*/



void camera_init(float azimuth, float elevation) {
    vec3 fwd;
    fwd.z = cosf(azimuth) * cosf(elevation);
    fwd.x = -sinf(azimuth);
    fwd.y = -cosf(azimuth) * sinf(elevation);

    vec3 right = {
        .y = -cosf(elevation),
        .z = -sinf(elevation)
    };

    vec3 up = v3cross(fwd, right);
    
    camera = (Camera) {
        .view = { up, right, fwd },
    };
}


vec3 camera_trans(vec3 p) {
    vec3 result;
    result = m3v3mul(camera.view, p);
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
    SDL_RenderDrawLineF(renderer, origin.x, origin.y, xunit.x, xunit.y);
    SDL_SetRenderDrawColor(renderer, 0x0, 0xff, 0x0, 0xff);
    SDL_RenderDrawLineF(renderer, origin.x, origin.y, yunit.x, yunit.y);
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0xff, 0xff);
    SDL_RenderDrawLineF(renderer, origin.x, origin.y, zunit.x, zunit.y);
}
    
void draw_frame() {
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(renderer);
    for (int i = 0; i < model->edge_count; i+=2) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        vec3 p0 = camera_trans(model->verts[model->edges[i]-1]);
        vec3 p1 = camera_trans(model->verts[model->edges[i+1]-1]);
        /*for (int j = 0; j < model->edges[i][0]; j++) {
            SDL_RenderDrawPoint(renderer, p0.x + j * 4 + 4, p0.y);
        }*/
        SDL_RenderDrawLineF(renderer, p0.x, p0.y, p1.x, p1.y);
    }
    draw_origin();
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
                case SDLK_LEFT:
                    azimuth += 0.1;
                    camera_init(azimuth, elevation);
                    break;
                case SDLK_RIGHT:
                    azimuth -= 0.1;
                    camera_init(azimuth, elevation);
                    break;
                case SDLK_UP:
                    elevation += 0.1;
                    camera_init(azimuth, elevation);
                    break;
                case SDLK_DOWN:
                    elevation -= 0.1;
                    camera_init(azimuth, elevation);
                    break;
            }
            break;
        case SDL_MOUSEWHEEL:
            zoom += ((float) event->wheel.y)/20.0;
            camera_init(azimuth, elevation);
            break;
        case SDL_MOUSEMOTION:
            const float MOUSE_SENS = 0.01;
            if (event->motion.state & SDL_BUTTON_RMASK) {
                azimuth += (float)event->motion.xrel * MOUSE_SENS;
                elevation += (float)event->motion.yrel * MOUSE_SENS;
            }
            camera_init(azimuth, elevation);
            break;
        case SDL_QUIT:
            running = 0;
            break;
    }
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


	if (!setup_sdl()) return 0;
    camera_init(1.0, 1.0);
    /*
    printf("%s\n",v3fmt(camera.view[0]));
    printf("%s\n",v3fmt(camera.view[1]));
    printf("%s\n",v3fmt(camera.view[2]));
    */
    draw_frame();
	loop();
	finish();
    return 0;
}
