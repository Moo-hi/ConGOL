//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  A recreation of Conway's Game of Life.                                                                                                //
//  rewrite-v1.0: xx.xx.2023                                                                                                                                        //
//                                                                                                                                                          //
//  Rules (https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life, as of 01.06.2021):                                                                        //
//                                                                                                                                                          //
//  The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells_, each of which is in one of two possible states:       //
//  live or dead, (or populated and unpopulated, respectively).                                                                                             //
//                                                                                                                                                          //
//  Every cell interacts with its eight neighbours, which are the cells_ that are horizontally, vertically, or diagonally adjacent.                          //
//  At each step in time, the following transitions occur:                                                                                                  //
//  - Any live cell with fewer than two live neighbours dies, as if by underpopulation.                                                                     //
//  - Any live cell with two or three live neighbours lives on to the next generation.                                                                      //
//  - Any live cell with more than three live neighbours dies, as if by overpopulation.                                                                     //
//  - Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.                                                          //
//                                                                                                                                                          //
//  These rules, which compare the behavior of the automaton to real life, can be condensed into the following :                                            //
//  - Any live cell with two or three live neighbours survives.                                                                                             //
//  - Any dead cell with three live neighbours becomes a live cell.                                                                                         //
//  - All other live cells_ die in the next generation.Similarly, all other dead cells_ stay dead.                                                            //
//                                                                                                                                                          //
//  The initial pattern constitutes the seed of the system:                                                                                                 //
//  The first generation is created by applying the above rules simultaneously to every cell in the seed; births and deaths occur simultaneously,           //
//  and the discrete moment at which this happens is sometimes called a tick. Each generation is a pure function of the preceding one.                      //
//  The rules continue to be applied repeatedly to create further generations.                                                                              //
//                                                                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include fan
#include <iostream>
#include <vector>
#include <limits>

struct Vec2 {
	float x, y;

	Vec2(float x, float y) : x(x), y(y) {}

	Vec2 operator/(int i) {
		return { x/i, y/i };
	}
};

/// <summary>
/// Represents a cell as per Conway's Game of Life
/// </summary>
struct Cell {
	// Instanced
	bool is_alive = false;
	Vec2 position = Vec2(0.0f, 0.0f);

	float distance_to(Vec2 point) {
		float dx = point.x - point.x;
		float dy = point.y - point.y;
		return sqrt(dx * dx + dy * dy);
	}

	// Static
private:
	static enum Era {
		Invalid,
		Past,
		Present,
		Future
	};
	
	static int _gen_curr;
	static std::vector<std::vector<Cell>> _cache;
	static Vec2 _cell_size;


	// Relative to current era: Is this generation past, present or future?
	static Era get_current_era(int gen) {
		if (gen < 0) return Invalid;
		if (gen < _gen_curr) return Past;
		if (gen == _gen_curr) return Present;
		return Future;
	}

public:
	static Vec2 get_cell_size() {
		return _cell_size;
	}

	static void recalculate_cell_size(Vec2 window_size, int divisor) {
		_cell_size = window_size / divisor;
	}

	static int get_generation() {
		return _gen_curr;
	}

	static void set_generation(std::vector<Cell>& cells, int gen) {
		Era era = get_current_era(gen);

		if (era == Cell::Past) cells = _cache[gen];

		if (era == Cell::Future) {
			for (size_t i = _gen_curr; i < (static_cast<unsigned long long>(gen) - _gen_curr); i++)
			{
				tick(cells);
			}
		}

		_gen_curr = gen;
	}

	static std::vector<Cell> tick(std::vector<Cell>& cells) {

	}
};

float cross_product(Vec2 a, Vec2 b) {
	return a.x * b.y - a.y * b.x;
}

/// <summary>
/// Square:
///	<para>AB</para>
/// <para>CD</para>
/// 
/// </summary>
bool is_point_inside_square(Vec2 point, Vec2 A, Vec2 B, Vec2 C, Vec2 D) {
	// Calculate the cross products of the vectors formed by the point and each pair of adjacent corners
	float cross1 = cross_product({ B.x - A.x, B.y - A.y }, { point.x - A.x, point.y - A.y });
	float cross2 = cross_product({ C.x - B.x, C.y - B.y }, { point.x - B.x, point.y - B.y });
	float cross3 = cross_product({ D.x - C.x, D.y - C.y }, { point.x - C.x, point.y - C.y });
	float cross4 = cross_product({ A.x - D.x, A.y - D.y }, { point.x - D.x, point.y - D.y });

	// If all the cross products have the same sign, the point is inside the square
	return ((cross1 > 0 && cross2 > 0 && cross3 > 0 && cross4 > 0) ||
		(cross1 < 0 && cross2 < 0 && cross3 < 0 && cross4 < 0));
}

void set_keybinds() {

}

void on_mouse_click(std::vector<Cell>& cells) {
	Vec2 mouse_pos = {231,123};//GetMousePosition();

	Vec2 size = Cell::get_cell_size();


	// Determine nearest cell
	// Note to self: Could improve performance by observing change of trend (ascending/descending)
	float shortest_dist = std::numeric_limits<float>::max();
	Cell* nearest_cell;
	for (size_t i = 0; i < cells.size(); i++)
	{
		auto& cell = cells[i];

		auto dist = cell.distance_to(mouse_pos);
		if (dist < shortest_dist) {
			shortest_dist = dist;
			nearest_cell = &cell;
		}
	}

	// Fill cell
	// todo
}

void main() {
	set_keybinds();
	Vec2 wnd_size = { 1920, 1080 };//GetWindowSize();

	Cell::recalculate_cell_size(wnd_size, 16);
	std::vector<Cell> cells;


}