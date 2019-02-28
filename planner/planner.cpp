// planner.cpp : Defines the entry point for the application.
//

#include "planner.h"
#include <vector>
#include <SDL.h>

using namespace std;

enum BlockType {
	BLOCK_RED,
	BLOCK_YELLOW_SMALL,
	BLOCK_YELLOW_BIG,
	BLOCK_BLUE,
	BLOCK_MAIN,
	BLOCK_EMPTY
};

enum AdjacencyType {
	ADJ_SIDE,
	ADJ_FRONT,
	ADJ_NONE
};

struct BlockGroup {
	AdjacencyType adjacencyType;
	std::vector<BlockType> blocks;
	BlockGroup(std::vector<BlockType>&& v) : blocks(std::move(v)) {}
	BlockGroup(const BlockGroup& bg, AdjacencyType at) : blocks(bg.blocks), adjacencyType(at), adjacentGroup(bg.adjacentGroup), overlappingGroup(bg.overlappingGroup) {}

	BlockGroup* adjacentGroup = nullptr;
	BlockGroup* overlappingGroup = nullptr;
};

//given blocks in clockwise ordering

BlockGroup A_BR({ BLOCK_BLUE, BLOCK_RED });
BlockGroup A_BYsB({ BLOCK_BLUE, BLOCK_YELLOW_SMALL, BLOCK_BLUE });
BlockGroup A_YbBYsBYb({ BLOCK_YELLOW_BIG, BLOCK_BLUE, BLOCK_YELLOW_SMALL, BLOCK_BLUE, BLOCK_YELLOW_BIG });
BlockGroup A_RB({BLOCK_RED, BLOCK_BLUE});

BlockGroup B_Yb({BLOCK_YELLOW_BIG});
BlockGroup B_BB({BLOCK_BLUE, BLOCK_BLUE});
BlockGroup B_YbBBYb({BLOCK_YELLOW_BIG, BLOCK_BLUE, BLOCK_BLUE, BLOCK_YELLOW_BIG});
BlockGroup B_BYs({BLOCK_BLUE, BLOCK_YELLOW_SMALL});
BlockGroup B_R({BLOCK_RED});

BlockGroup C_BR(A_BR);
BlockGroup C_BYsB(A_BYsB);
BlockGroup C_YbB({BLOCK_YELLOW_BIG, BLOCK_BLUE});

BlockGroup* MakeModelA() {
	BlockGroup* A = new BlockGroup(A_BR, ADJ_SIDE);
	A->adjacentGroup = new BlockGroup(A_BYsB, ADJ_FRONT);
	A->adjacentGroup->adjacentGroup = new BlockGroup(A_YbBYsBYb, ADJ_SIDE);
	A->adjacentGroup->adjacentGroup->adjacentGroup = new BlockGroup(A_BYsB, ADJ_FRONT);
	A->adjacentGroup->adjacentGroup->adjacentGroup->adjacentGroup = new BlockGroup(A_RB, ADJ_NONE);
	return A;
}

BlockGroup* MakeModelB() {
	BlockGroup* B = new BlockGroup(B_Yb, ADJ_SIDE);
	B->adjacentGroup = new BlockGroup(B_BB, ADJ_FRONT);
	B->adjacentGroup->adjacentGroup = new BlockGroup(B_YbBBYb, ADJ_SIDE);
	B->adjacentGroup->adjacentGroup->adjacentGroup = new BlockGroup(B_BYs, ADJ_FRONT);
	B->adjacentGroup->adjacentGroup->adjacentGroup->adjacentGroup = new BlockGroup(B_R, ADJ_NONE);
	return B;
}

BlockGroup* MakeModelC() {
	BlockGroup* C = new BlockGroup(C_BR, ADJ_SIDE);
	C->adjacentGroup = new BlockGroup(C_BYsB, ADJ_FRONT);
	C->adjacentGroup->adjacentGroup = new BlockGroup(C_YbB, ADJ_SIDE);
	return C;
}

int main(int argc, char** argv)
{
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	const int SCREEN_WIDTH = 1366;
	const int SCREEN_HEIGHT = 768;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Could not init SDL2 Video" << std::endl;
		return -1;
	}

	window = SDL_CreateWindow("Planner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	bool quit = false;
	SDL_Event e;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		//Running code
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); 
		SDL_RenderClear(renderer);

		SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }; 
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF); 
		SDL_RenderFillRect(renderer, &fillRect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
