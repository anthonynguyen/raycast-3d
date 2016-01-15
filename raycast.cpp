#include <cmath>
#include <iostream>

#include <SDL2/SDL.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FOV 70 * M_PI / 180

#define WORLD_X 20
#define WORLD_Y 20

#define MAXDIST 7

typedef struct {
	double x, y;
} Vector;

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
SDL_Renderer *renderer = NULL;

int world[WORLD_Y][WORLD_X] = {
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
		return !!world[my][mx];

	return true;
}

void draw_slice(int i, double dist, int side) {
	static SDL_Rect slice = {0, 0, 1, 1};

	if (dist > MAXDIST)
		return;

	if (side)
		SDL_SetRenderDrawColor(renderer, 90, 220, 20, SDL_ALPHA_OPAQUE);
	else
		SDL_SetRenderDrawColor(renderer, 60, 190, 0, SDL_ALPHA_OPAQUE);

	slice.x = i;
	slice.h = SCREEN_HEIGHT * (1 - (dist - 1) / MAXDIST);
	slice.y = (SCREEN_HEIGHT - slice.h) / 2;

	SDL_RenderFillRect(renderer, &(slice));
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

	Vector ppos = {2.5, 2.5}, rpos;
	Vector pdir = {0, 1.0}, rdir; // pdir: player direction, rdir: ray direction

	Vector next, step, rdelta;
	double dist;
	double anglestep = FOV / SCREEN_WIDTH;
	int i;
	int side;

	while (!die) {
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN)
				die = true;
		}

		draw_bg();

		rotate(&pdir, &rdir, FOV / 2);

		for (i = 0; i < SCREEN_WIDTH; i++) {
			rpos.x = (int)ppos.x;
			rpos.y = (int)ppos.y;

			rdelta.x = sqrt((rdir.y * rdir.y) / (rdir.x * rdir.x) + 1);
			rdelta.y = sqrt((rdir.x * rdir.x) / (rdir.y * rdir.y) + 1);

			if (rdir.x > 0) {
				step.x = 1;
				next.x = rdelta.x * ((int)(ppos.x + 1) - ppos.x);
			} else {
				step.x = -1;
				next.x = rdelta.x * (ppos.x - (int)(ppos.x));
			}

			if (rdir.y > 0) {
				step.y = 1;
				next.y = rdelta.y * ((int)(ppos.y + 1) - ppos.y);
			} else {
				step.y = -1;
				next.y = rdelta.y * (ppos.y - (int)(ppos.y));
			}

			while (!is_solid(rpos.x, rpos.y)) {
				if (next.x < next.y) {
					next.x += rdelta.x;
					rpos.x += step.x;
					side = 0;
				} else {
					next.y += rdelta.y;
					rpos.y += step.y;
					side = 1;
				}
			}

			// if (side) {
			// 	rpos.x = next.x + rdelta.x;
			// 	rpos.y = next.y - rdelta.y;
			// } else {
			// 	rpos.x = next.x - rdelta.x;
			// 	rpos.y = next.y + rdelta.y;
			// }
			
			dist = sqrt((rpos.x - ppos.x) * (rpos.x - ppos.x) + (rpos.y - ppos.y) * (rpos.y - ppos.y));

			std::cout << i << ": " << dist << ", " << side << std::endl;
			std::cout << rpos.x << ", " << rpos.y << std::endl;
			std::cout << next.x - rdelta.x << ", " << next.y + rdelta.y << std::endl;
			draw_slice(i, dist, side);

			rotate(&rdir, &rdir, -anglestep);
			//die = 1;
		}

		//die = 1;

		rotate(&pdir, &pdir, 0.002);
		//std::cout << atan2(pdir.y, pdir.x)*180/M_PI << std::endl;

		SDL_UpdateWindowSurface(window);
	}

	close();

	return 0;
}