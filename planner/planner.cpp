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

enum Direction {
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
	DIR_UP
};

struct BlockGroup {
	Direction direction;
	AdjacencyType adjacencyType;
	std::vector<BlockType> blocks;
	BlockGroup(std::vector<BlockType>&& v) : blocks(std::move(v)) {}
	BlockGroup(const BlockGroup& bg, AdjacencyType at, Direction dir) : blocks(bg.blocks), adjacencyType(at), right(bg.right), left(bg.left), direction(dir) {}

	BlockGroup* right = nullptr;
	BlockGroup* left = nullptr;
	BlockGroup* overlapRightGroup = nullptr;

	void AttachGroup(BlockGroup* group) {
		right = group;
		group->left = this;
	}
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
	BlockGroup* A = new BlockGroup(A_BR, ADJ_SIDE, DIR_RIGHT);
	A->AttachGroup(new BlockGroup(A_BYsB, ADJ_FRONT, DIR_DOWN));
	A->right->AttachGroup(new BlockGroup(A_YbBYsBYb, ADJ_SIDE, DIR_LEFT));
	A->right->right->AttachGroup(new BlockGroup(A_BYsB, ADJ_FRONT, DIR_UP));
	A->right->right->right->AttachGroup(new BlockGroup(A_RB, ADJ_NONE, DIR_RIGHT));
	return A;
}

BlockGroup* MakeModelB() {
	BlockGroup* B = new BlockGroup(B_Yb, ADJ_SIDE, DIR_RIGHT);
	B->AttachGroup(new BlockGroup(B_BB, ADJ_FRONT, DIR_DOWN));
	B->right->AttachGroup(new BlockGroup(B_YbBBYb, ADJ_SIDE, DIR_LEFT));
	B->right->right->AttachGroup(new BlockGroup(B_BYs, ADJ_FRONT, DIR_UP));
	B->right->right->right->AttachGroup(new BlockGroup(B_R, ADJ_NONE, DIR_RIGHT));
	return B;
}

BlockGroup* MakeModelC() {
	BlockGroup* C = new BlockGroup(C_BR, ADJ_SIDE, DIR_DOWN);
	C->AttachGroup(new BlockGroup(C_BYsB, ADJ_FRONT, DIR_LEFT));
	C->right->AttachGroup(new BlockGroup(C_YbB, ADJ_SIDE, DIR_UP));
	return C;
}

enum Color {
	COL_RED,
	COL_YELLOW,
	COL_BLUE
};



Direction RotateClockwise(Direction dir) {
	switch (dir)
	{
	case DIR_RIGHT:
		return DIR_DOWN;

	case DIR_DOWN:
		return DIR_LEFT;

	case DIR_LEFT:
		return DIR_UP;

	case DIR_UP:
		return DIR_RIGHT;
	}
}

Direction RotateCounterClockwise(Direction dir) {
	switch (dir)
	{
	case DIR_RIGHT:
		return DIR_UP;

	case DIR_DOWN:
		return DIR_RIGHT;

	case DIR_LEFT:
		return DIR_DOWN;

	case DIR_UP:
		return DIR_LEFT;
	}
}

BlockGroup* RotateClockwiseGroup(BlockGroup* group) {
	group->direction = RotateClockwise(group->direction);
	if (group->right != nullptr) {
		RotateClockwiseGroup(group->right);
	}
	if (group->left != nullptr) {
		RotateClockwiseGroup(group->left);
	}
	return group;
}

BlockGroup* RotateCounterClockwiseGroup(BlockGroup* group) {
	group->direction = RotateCounterClockwise(group->direction);
	if (group->right != nullptr) {
		RotateCounterClockwiseGroup(group->right);
	}
	if (group->left != nullptr) {
		RotateCounterClockwiseGroup(group->left);
	}
	return group;
}

struct RenderBlock {
	Color color;
	int xPos;
	int yPos;
	int width;
	int height;
};

void ToRenderBlocks(std::vector<RenderBlock>& renderBlocks, BlockGroup* group, int startX, int startY) {
	Direction dir = group->direction;
	int currentX = startX;
	int currentY = startY;
	const auto& blockTypes = group->blocks;
	
	for (const auto& blockType : blockTypes) {
		RenderBlock rb;
		rb.xPos = currentX;
		rb.yPos = currentY;
		rb.height = 2;

		switch (blockType)
		{
		case BLOCK_BLUE:
			rb.color = COL_BLUE;
			rb.width = 1;
			break;

		case BLOCK_RED:
			rb.color = COL_RED;
			rb.width = 2;
			break;

		case BLOCK_YELLOW_SMALL:
			rb.color = COL_YELLOW;
			rb.width = 1;
			break;

		case BLOCK_YELLOW_BIG:
			rb.color = COL_YELLOW;
			rb.width = 2;
			break;
		}

		switch (dir)
		{
		case DIR_RIGHT:
			currentX += rb.width;
			break;

		case DIR_DOWN:
			std::swap(rb.width, rb.height);
			currentY += rb.height;
			break;

		case DIR_LEFT:
			rb.width *= -1;
			currentX += rb.width;
			break;

		case DIR_UP:
			std::swap(rb.width, rb.height);
			rb.height *= -1;
			currentY += rb.height;
			break;
		}
		renderBlocks.push_back(rb);
	}

	int deltaX = 0;
	int deltaY = 0;

	switch (group->adjacencyType)
	{
	case ADJ_SIDE:
		switch (dir)
		{
		case DIR_RIGHT:
			deltaX = -1;
			deltaY = 2;
			break;

		case DIR_LEFT:
			deltaX = -1;
			break;

		case DIR_UP:
			deltaX = 2;
			deltaY = -1;
			break;

		case DIR_DOWN:
			deltaY = -1;
		default:
			break;
		}
		break;

	case ADJ_FRONT:
		switch (dir)
		{
		case DIR_DOWN:
			deltaX = 1;
			break;

		case DIR_UP:
			deltaX = 1;
			deltaY = -2;
			break;

		case DIR_RIGHT:
			deltaY = 1;
			break;

		case DIR_LEFT:
			deltaX = -2;
			deltaY = 1;
			break;

		default:
			break;
		}
		break;
	}

	currentX += deltaX;
	currentY += deltaY;
	
	if (group->right != nullptr) {
		ToRenderBlocks(renderBlocks, group->right, currentX, currentY);
	}
	if (group->left != nullptr) {
		ToRenderBlocks(renderBlocks, group->left, currentX, currentY);
	}
}

int main(int argc, char** argv)
{
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	const int SCREEN_WIDTH = 1366;
	const int SCREEN_HEIGHT = 768;

	auto modelA = MakeModelA();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Could not init SDL2 Video" << std::endl;
		return -1;
	}

	window = SDL_CreateWindow("Planner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);


	std::vector<RenderBlock> renderBlocks;
	ToRenderBlocks(renderBlocks, RotateCounterClockwiseGroup(modelA), 20, 20);

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	for (const auto& block : renderBlocks) {
		SDL_Rect fillRect = { block.xPos * 20, block.yPos * 20, block.width * 20, block.height * 20 };

		switch (block.color)
		{
		case COL_BLUE:
			SDL_SetRenderDrawColor(renderer, 19, 83, 168, 0xff);
			break;

		case COL_RED:
			SDL_SetRenderDrawColor(renderer, 181, 24, 50, 0xff);
			break;

		case COL_YELLOW:
			SDL_SetRenderDrawColor(renderer, 235, 225, 150, 0xff);
			break;
		}

		SDL_RenderFillRect(renderer, &fillRect);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
		SDL_RenderDrawRect(renderer, &fillRect);
	}

	SDL_RenderPresent(renderer);

	bool quit = false;
	SDL_Event e;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
