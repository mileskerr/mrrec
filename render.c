#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/param.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "vmath.h"
#include "math.h"
#include "obj.h"

typedef struct {
    size_t edge_count;
    vec3 * verts;
    size_t * edges;
} Model;

typedef struct {
    vec3 view[3];
    vec3 pos;
} Camera;

typedef struct {
    SDL_Scancode cam_rot_left;
    SDL_Scancode cam_rot_right;
    SDL_Scancode cam_rot_up;
    SDL_Scancode cam_rot_down;
    SDL_Scancode cam_up;
    SDL_Scancode cam_down;
    SDL_Scancode cam_fwd;
    SDL_Scancode cam_back;
    SDL_Scancode cam_right;
    SDL_Scancode cam_left;
} Keymap;

typedef struct {
    int width;
    int height;
    int max_fps;
    float mouse_sens;
    float camera_speed;
    float camera_rot_speed;
    char * model_path;
    Keymap keymap;
} Settings;

Settings settings = {
    .width = 1024,
    .height = 768,
    .max_fps = 60,
    .mouse_sens = 0.5,
    .camera_speed = 2.0,
    .camera_rot_speed = 1.5,
    .model_path = "example_models/utah_teapot.obj",
    .keymap = {
        .cam_rot_left = SDL_SCANCODE_LEFT,
        .cam_rot_right = SDL_SCANCODE_RIGHT,
        .cam_rot_up = SDL_SCANCODE_UP,
        .cam_rot_down = SDL_SCANCODE_DOWN,
        .cam_up = SDL_SCANCODE_LSHIFT,
        .cam_down = SDL_SCANCODE_LCTRL,
        .cam_fwd = SDL_SCANCODE_W,
        .cam_back = SDL_SCANCODE_S,
        .cam_right = SDL_SCANCODE_D,
        .cam_left = SDL_SCANCODE_A,
    }
};

SDL_Window * window = NULL;
SDL_Renderer * renderer;
TTF_Font * default_font;
Model * model;


int running = 1;

float azimuth = 1.0;
float elevation = 1.0;
float zoom = 0.5;

float frame_time = 0.0; //time taken to draw last frame
float delta_time = 0.0; //time taken by last frame (including waiting)

Camera camera;


void draw_text(SDL_Renderer * renderer, char * message, TTF_Font * font, SDL_Color color, int x, int y) {

	SDL_Surface * surf = TTF_RenderText_Blended(font, message, color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);

	SDL_FreeSurface(surf);
   
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dstrect = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

}

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
    result = (vec3) { result.x * settings.height + (settings.width/2), result.y * settings.height + (settings.height/2), result.z };
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
        SDL_RenderDrawLineF(renderer, p0.x, p0.y, p1.x, p1.y);
    }
    draw_origin();
    draw_camera_origin();
    char frame_time_str[40];
    snprintf(frame_time_str, 40, "%.3f/%.3f", frame_time * 1000.0,1000.0/settings.max_fps);
    draw_text(renderer, frame_time_str, default_font, (SDL_Color) {0xff,0xff,0xff,0xff}, 20, 20);
    SDL_RenderPresent(renderer);
}



int setup_sdl() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(settings.width, settings.height, SDL_WINDOW_SHOWN, &window, &renderer);
	if (!window) {
		fprintf(stderr,"InitSetup failed to create window");
        running = 0;
        return 0;
	}
	SDL_SetWindowTitle(window, "Example One");
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(renderer);

    TTF_Init();
    default_font = TTF_OpenFont("fonts/Unifont.ttf", 18);

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
                azimuth += (float)event->motion.xrel * settings.mouse_sens * delta_time;
                elevation += (float)event->motion.yrel * settings.mouse_sens * delta_time;
            }
            camera_setrot(azimuth, elevation);
            break;
        case SDL_QUIT:
            running = 0;
            break;
    }
}


void handle_keys() {
    vec3 fwd;
    vec3 right;
    Keymap * km = &settings.keymap;
    const Uint8* kb_state = SDL_GetKeyboardState(NULL);
    if (kb_state[km->cam_rot_left]) {
        azimuth += settings.camera_rot_speed * delta_time;
    } if (kb_state[km->cam_rot_right]) {
        azimuth -= settings.camera_rot_speed * delta_time;
    } if (kb_state[km->cam_rot_up]) {
        elevation += settings.camera_rot_speed * delta_time;
    } if (kb_state[km->cam_rot_down]) {
        elevation -= settings.camera_rot_speed * delta_time;
    } if (kb_state[km->cam_up]) {
        camera.pos.y += settings.camera_speed * delta_time;
    } if (kb_state[km->cam_down]) {
        camera.pos.y -= settings.camera_speed * delta_time;
    } if (kb_state[km->cam_fwd]) {
        fwd = (vec3) { sinf(azimuth), 0, cosf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(fwd, settings.camera_speed * delta_time));
    } if (kb_state[km->cam_back]) {
        fwd = (vec3) { sinf(azimuth), 0, cosf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(fwd, -settings.camera_speed * delta_time));
    } if (kb_state[km->cam_right]) {
        right = (vec3) { cosf(azimuth), 0, -sinf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(right, settings.camera_speed * delta_time));
    } if (kb_state[km->cam_left]) {
        right = (vec3) { cosf(azimuth), 0, -sinf(azimuth) };
        camera.pos = v3add(camera.pos, v3mul(right, -settings.camera_speed * delta_time));
    }
    camera_setrot(azimuth, elevation);
}



void loop() {
    float min_secs_per_frame = 1.0/(float) settings.max_fps;
    int min_cycles_per_frame = (min_secs_per_frame) * (float) CLOCKS_PER_SEC;
    SDL_Event event;
	while (running) {
        clock_t t = clock();
        SDL_PumpEvents();
		while (SDL_PollEvent(&event)) {
            handle_event(&event);
		}
        handle_keys();
        draw_frame();
        frame_time = (float) (clock() - t) / (float) CLOCKS_PER_SEC;
        delta_time = MAX(frame_time,min_secs_per_frame);
        while (clock() - t < min_cycles_per_frame) { /*wait*/ }
	}
}

void finish() {
    TTF_CloseFont(default_font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    TTF_Quit();
	SDL_Quit();
	exit(0);
}

#define OPTS_START() \
void parse_opts(int argc, char * args[]) { \
    int argi = 1; /*0th argument is name of program*/ \
    int parami = 1; \
    for (argi = 1; argi < argc; argi++) { \
        parami = argi; \
        if (args[argi][0] == '-') { /*only doing short names for now*/ \
            int chi = 1;/*0th character is always a dash*/ \
            while (args[argi][chi]) { /*go through each short option*/ \

#define STR_OPT(NAME, STORE) \
                if (args[argi][chi] == NAME) { \
                    parami++; \
                    if (parami >= argc) { \
                        fprintf(stderr, "expected argument for option '-%c'\n", args[argi][chi]); \
                    } else { /*there is a parameter, but it's not necessarily formatted properly*/ \
                        STORE = args[parami]; \
                    } \
                    chi++; \
                    continue; \
                }
#define NUM_OPT(NAME, STORE) \
                if (args[argi][chi] == NAME) { \
                    parami++; \
                    if (parami >= argc) { \
                        fprintf(stderr, "expected argument for option '-%c'\n", args[argi][chi]); \
                    } else { /*there is a parameter, but it's not necessarily formatted properly*/ \
                        char * endptr; \
                        double param = strtod(args[parami],&endptr); \
                        if (*endptr) { /*strtod didn't read to the end of the string, so it's not a valid number*/ \
                            fprintf(stderr, "invalid argument '%s' for option '-%c'\n", args[parami], args[argi][chi]); \
                        } else { \
                            STORE = param; \
                        } \
                    } \
                    chi++; \
                    continue; \
                }

#define OPTS_END() \
                { \
                    fprintf(stderr, "invalid option '-%c'\n", args[argi][chi]); \
                } \
                chi++; \
            } \
        argi = parami; \
        } \
    } \
}

OPTS_START();
NUM_OPT('w', settings.width);
NUM_OPT('h', settings.height);
NUM_OPT('F', settings.max_fps);
STR_OPT('m', settings.model_path);
OPTS_END();

int main(int argc, char * args[]) {

    parse_opts(argc, args);

    FILE * fp;
    fp = fopen(settings.model_path, "r");

    vec3 * verts;
    size_t * edges;
    size_t edge_count = parse_obj(fp, &verts, &edges);
    model = &(Model) {
        .edges = edges,
        .verts = verts,
        .edge_count = edge_count,
    };

	setup_sdl();
    camera_init(0.5, 0.5, (vec3) { 0, 0, 0 });
	loop();
	finish();
    return 0;
}
