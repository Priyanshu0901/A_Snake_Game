/*
 * AI.c
 *
 *  Created on: 04-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "AI.h"
#include "stdlib.h"

typedef struct {
	bool up, down, left, right;
	bool visited;
} MST_Node_t;

static MST_Node_t supergrid[4][4];

// Reset Grid
void init_supergrid() {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			supergrid[y][x] =
					(MST_Node_t ) { false, false, false, false, false };
		}
	}
}

void visit_node(int x, int y) {
	supergrid[y][x].visited = true;

	// Directions: 0=Up, 1=Down, 2=Left, 3=Right
	int dirs[] = { 0, 1, 2, 3 };

	// Shuffle directions for randomness
	for (int i = 0; i < 4; i++) {
		int r = rand() % 4;
		int temp = dirs[r];
		dirs[r] = dirs[i];
		dirs[i] = temp;
	}

	for (int i = 0; i < 4; i++) {
		int nx = x, ny = y;
		if (dirs[i] == 0)
			ny--;      // Up
		else if (dirs[i] == 1)
			ny++; // Down
		else if (dirs[i] == 2)
			nx--; // Left
		else if (dirs[i] == 3)
			nx++; // Right

		// Check bounds and if the neighbor was already visited
		if (nx >= 0 && nx < 4 && ny >= 0 && ny < 4
				&& !supergrid[ny][nx].visited) {
			// Open the wall between current and neighbor
			if (dirs[i] == 0) {
				supergrid[y][x].up = true;
				supergrid[ny][nx].down = true;
			} else if (dirs[i] == 1) {
				supergrid[y][x].down = true;
				supergrid[ny][nx].up = true;
			} else if (dirs[i] == 2) {
				supergrid[y][x].left = true;
				supergrid[ny][nx].right = true;
			} else if (dirs[i] == 3) {
				supergrid[y][x].right = true;
				supergrid[ny][nx].left = true;
			}

			// Recurse into the neighbor
			visit_node(nx, ny);
		}
	}
}

void generate_mst_4x4() {
	init_supergrid();
	// Start generating from a random corner or (0,0)
	visit_node(rand() % 4, rand() % 4);
}

void expand_mst_to_hamiltonian(AI_t *const me) {
	int x = 0, y = 0;
	for (int i = 0; i < 64; i++) {
		me->ham_path[i].x = x;
		me->ham_path[i].y = y;

		int sx = x / 2; // Current super-cell X
		int sy = y / 2; // Current super-cell Y

		// Determine next step based on which corner of the 2x2 we are in:
		if (x % 2 == 0 && y % 2 == 0) { // Top-Left
			if (supergrid[sy][sx].up)
				y--;    // Move to super-cell above
			else
				x++;                         // Move to Top-Right
		} else if (x % 2 == 1 && y % 2 == 0) { // Top-Right
			if (supergrid[sy][sx].right)
				x++; // Move to super-cell right
			else
				y++;                         // Move to Bottom-Right
		} else if (x % 2 == 1 && y % 2 == 1) { // Bottom-Right
			if (supergrid[sy][sx].down)
				y++;  // Move to super-cell below
			else
				x--;                         // Move to Bottom-Left
		} else { // Bottom-Left
			if (supergrid[sy][sx].left)
				x--;  // Move to super-cell left
			else
				y--;                         // Move to Top-Left
		}
	}
}

void init_grid_to_index(AI_t *const me) {
	for (int i = 0; i < MAX_SNAKE_LEN; i++) {
		me->grid_to_index[me->ham_path[i].y][me->ham_path[i].x] = i;
	}
}

void AI_ctor(AI_t *const me, GAME_Engine_t *game_state) {
	me->game_state = game_state;
	AI_reset(me);
}

void AI_reset(AI_t *const me) {

	// 1. Build a Randomized Spanning Tree on a 4x4 grid
	generate_mst_4x4();

	// 2. Convert that 4x4 tree into the 8x8 ham_path
	expand_mst_to_hamiltonian(me);

	init_grid_to_index(me);

	me->game_state->game_over = false;
	log_message("AI", LOG_INFO, "Path Generated. Ready to play.");
}

key_action_e AI_get_action(AI_t *const me) {
	C_COORDINATES_t head = me->game_state->body[0];
	C_COORDINATES_t tail = me->game_state->body[me->game_state->length - 1];
	C_COORDINATES_t food = me->game_state->food;

	int head_idx = me->grid_to_index[head.y][head.x];
	int tail_idx = me->grid_to_index[tail.y][tail.x];
	int food_idx = me->grid_to_index[food.y][food.x];
	int current_len = me->game_state->length;

	// Only shortcut if snake is less than half the board size
	if (current_len < (MAX_SNAKE_LEN / 2)) {
		int best_dist_to_food = 1000; // Large number
		key_action_e best_action = ACTION_NONE;

		for (int i = 0; i < 4; i++) {
			key_action_e dir = (key_action_e) i;
			int nx = head.x, ny = head.y;

			// 1. Calculate neighbor coordinates
			if (dir == ACTION_UP)
				ny--;
			else if (dir == ACTION_DOWN)
				ny++;
			else if (dir == ACTION_LEFT)
				nx--;
			else if (dir == ACTION_RIGHT)
				nx++;

			// 2. Validate: Boundary Check
			if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
				continue;

			// 3. Validate: 180-degree turn check
			if ((dir == ACTION_UP && me->game_state->current_dir == ACTION_DOWN)
					|| (dir == ACTION_DOWN && me->game_state->current_dir == ACTION_UP)
					|| (dir == ACTION_LEFT && me->game_state->current_dir == ACTION_RIGHT)
					|| (dir == ACTION_RIGHT && me->game_state->current_dir == ACTION_LEFT))
				continue;

			// 4. Validate: Body Collision
			bool hits_body = false;
			for (int b = 0; b < current_len; b++) {
				if (me->game_state->body[b].x == nx && me->game_state->body[b].y == ny) {
					hits_body = true;
					break;
				}
			}
			if (hits_body)
				continue;

			int neighbor_idx = me->grid_to_index[ny][nx];

			// 5. ROBUST SAFETY RULE: Is the path from Neighbor to Tail completely empty?
			bool path_is_trapped = false;
			int walk_idx = neighbor_idx;

			// We walk the Hamiltonian path from the neighbor's index to the tail's index
			while (walk_idx != tail_idx) {
			    C_COORDINATES_t path_pos = me->ham_path[walk_idx];

			    // Check if any part of the snake body is sitting on this path index
			    for (int b = 0; b < current_len; b++) {
			        if (me->game_state->body[b].x == path_pos.x && me->game_state->body[b].y == path_pos.y) {
			            path_is_trapped = true;
			            break;
			        }
			    }

			    if (path_is_trapped) break;

			    // Advance to the next index in the 64-step loop
			    walk_idx = (walk_idx + 1) % 64;
			}

			if (path_is_trapped) continue; // This shortcut would trap the snake!

			// 6. TARGETING: Pick the move that gets us closest to food in the Hamiltonian sequence
			int dist_to_food =
					(food_idx >= neighbor_idx) ?
							(food_idx - neighbor_idx) :
							(64 - neighbor_idx + food_idx);

			if (dist_to_food < best_dist_to_food) {
				best_dist_to_food = dist_to_food;
				best_action = dir;
			}
		}

		if (best_action != ACTION_NONE)
			return best_action;
	}

	// FALLBACK: Follow the Hamiltonian Cycle strictly
	C_COORDINATES_t next = me->ham_path[(head_idx + 1) % 64];
	if (next.x > head.x)
		return ACTION_RIGHT;
	if (next.x < head.x)
		return ACTION_LEFT;
	if (next.y > head.y)
		return ACTION_DOWN;
	if (next.y < head.y)
		return ACTION_UP;

	return ACTION_NONE;
}
