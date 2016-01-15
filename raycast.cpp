#include <cmath>
#include <iostream>

#include <SDL2/SDL.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FOV 70 * M_PI / 180

#define WORLD_X 20
#define WORLD_Y 20

typedef struct {
	double x, y;
} Vector;

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
SDL_Renderer *renderer = NULL;

int world[WORLD_X][WORLD_Y] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,1},
	{1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
	{1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1},
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	{1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1},
	{1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1},
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	{1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL couldn't initialize: " << SDL_GetError() << std::endl;
		success = false;
	} else {
		window = SDL_CreateWindow("raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			std::cerr << "Window wasn't created: " << SDL_GetError() << std::endl;
			success = false;
		} else {
			surface = SDL_GetWindowSurface(window);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		}
	}

	return success;
}

void close() {
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_bg() {
	SDL_Rect sky;

	SDL_SetRenderDrawColor(renderer, 70, 70, 70, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	sky.x = 0;
	sky.y = 0;
	sky.w = 640;
	sky.h = 240;

	SDL_SetRenderDrawColor(renderer, 90, 140, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &sky);
}

void rotate(Vector *v1, Vector *v2, double angle) {
	double ix = v1->x, iy = v1->y;
	v2->x = ix * cos(angle) - iy * sin(angle);
	v2->y = ix * sin(angle) + iy * cos(angle);
}

bool is_solid(double x, double y) {
	int mx = (int)x;
	int my = (int)y;

	if (mx < WORLD_X && my < WORLD_Y)
		return !!world[mx][my];

	return true;
}

int main(int argc, char *argv[]) {
	bool initialized = init();

	if (!initialized) {
		std::cerr << "SDL failed to initialize" << std::endl;
		close();
		return 1;
	}

	bool die = false;
	SDL_Event ev;

	Vector position = {2.5, 2.5};
	Vector direction = {1.0, 0};
	Vector temp;
	double anglestep = FOV / SCREEN_WIDTH;
	int i;

	while (!die) {
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN)
				die = true;
		}

		draw_bg();

		rotate(direction, temp, FOV / 2);
		for (i = 0; i < SCREEN_WIDTH; i++) {
			
		}

		SDL_UpdateWindowSurface(window);
	}

	close();

	return 0;
}