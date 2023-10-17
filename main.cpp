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

#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#ifndef FAN_INCLUDE_PATH
#define FAN_INCLUDE_PATH include
#endif
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#define loco_window
#define loco_context

#define loco_rectangle
#include _FAN_PATH(graphics/loco.h)

#include <vector>
#include <limits>
#include <cmath>
#include <map>

struct Singleton {
	static float cross_product(fan::vec2 a, fan::vec2 b) {
		return a.x * b.y - a.y * b.x;
	}

	/// <summary>
	/// Square:
	///	<para>AB</para>
	/// <para>CD</para>
	/// 
	/// </summary>
	static bool is_point_inside_square(fan::vec2 point, fan::vec2 A, fan::vec2 B, fan::vec2 C, fan::vec2 D) {
		// Calculate the cross products of the vectors formed by the point and each pair of adjacent corners
		float cross1 = cross_product({ B.x - A.x, B.y - A.y }, { point.x - A.x, point.y - A.y });
		float cross2 = cross_product({ C.x - B.x, C.y - B.y }, { point.x - B.x, point.y - B.y });
		float cross3 = cross_product({ D.x - C.x, D.y - C.y }, { point.x - C.x, point.y - C.y });
		float cross4 = cross_product({ A.x - D.x, A.y - D.y }, { point.x - D.x, point.y - D.y });

		// If all the cross products have the same sign, the point is inside the square
		return ((cross1 > 0 && cross2 > 0 && cross3 > 0 && cross4 > 0) ||
			(cross1 < 0 && cross2 < 0 && cross3 < 0 && cross4 < 0));
	}

	static bool nearlyEquals(fan::vec2 v1, fan::vec2 v2, float tolerance) {
		float dx = v1.x - v2.x;
		float dy = v1.y - v2.y;
		float distSquared = dx * dx + dy * dy;
		float tolSquared = tolerance * tolerance;
		return (distSquared <= tolSquared);
	}

	static bool nearlyEquals(double a, double b, double tolerance = 1e-9) {
		return std::abs(a - b) <= tolerance * std::max(std::abs(a), std::abs(b));
	}

	static constexpr int DEFAULT_DIVISOR = 16;
	Singleton() {
		fan::vec2 ortho_x = fan::vec2(0, loco.get_window()->get_size().x);
		fan::vec2 ortho_y = fan::vec2(0, loco.get_window()->get_size().y);

		// cache to speed up checking if a space is occupied by a cell
		// unimplemented until a need for optimization is confirmed
		// Key: camera position, Value: vector of positiopns to check for cells
		std::map<fan::vec2, std::vector<fan::vec2>> cell_space_cache;

		loco.open_camera(
			&camera,
			ortho_x,
			ortho_y
		);
		loco.get_window()->add_resize_callback([&](const fan::window_t::resize_cb_data_t& d) {
			fan::vec2 ortho_x = fan::vec2(0, loco.get_window()->get_size().x);
			fan::vec2 ortho_y = fan::vec2(0, loco.get_window()->get_size().y);
			camera.set_ortho(&loco, ortho_x, ortho_y);
			viewport.set(loco.get_context(), 0, d.size, d.size);
			});
		viewport.open(loco.get_context());
		viewport.set(loco.get_context(), 0, loco.get_window()->get_size(), loco.get_window()->get_size());

		// Setup keybinds
		loco.get_window()->add_buttons_callback([&](const fan::window_t::mouse_buttons_cb_data_t& button_data) {
			if (button_data.state != fan::mouse_state::release) {
				return;
			}

			switch (button_data.button)
			{
			case fan::mouse_left: {
				fan::vec2 mouse_pos = loco.get_mouse_position(camera, viewport);
				fan::vec2 cell_size = Singleton::Cell::get_cell_size();
				fan::vec2 wnd_size = Singleton::get_singleton().loco.get_window()->get_size();

				const float TOLERANCE = 0.1f;
				const float END_OF_COL_X = wnd_size.x - cell_size.x;
				const fan::vec2 START_POINT = { 0,0 };
				fan::vec2 end_point = wnd_size - cell_size;
				fan::vec2 current_point = START_POINT;

				while (!nearlyEquals(current_point, end_point, TOLERANCE)) 
				{
					if (nearlyEquals(current_point.x, mouse_pos.x, (double)cell_size.x - 1.0f)) 
					{
						while (!nearlyEquals(current_point.x, mouse_pos.x, (double)cell_size.y - 1.0f)) 
						{
							// Move point down
							current_point.y += cell_size.y;
						}
					}
					

					// Is the point an initialized cell?
					if (Singleton::Cell::is_cell(current_point)) break; //dosomething
				}

				// Fill cell
				Singleton::Cell new_cell(mouse_pos);
				cells.push_back(new_cell);
				break;
			}
			}
			});

		loco.get_window()->add_keys_callback([&](const fan::window_t::keyboard_keys_cb_data_t& button_data) {
			if (button_data.state != fan::keyboard_state::press) {
				return;
			}

			switch (button_data.key)
			{
				case fan::key_space: {
					ticking = !ticking;
					break;
				}
			}
		});
	}

	static Singleton& get_singleton();

	/// <summary>
	/// Represents a cell as per Conway's Game of Life
	/// </summary>
	struct Cell {
		// Per-instance
		bool is_alive = false;
		fan::vec2 position = fan::vec2(0.0f, 0.0f);

		float distance_to(fan::vec2 point) {
			float dx = point.x - point.x;
			float dy = point.y - point.y;
			return sqrt(dx * dx + dy * dy);
		}

		loco_t::shape_t drawable;

		Cell(fan::vec2 pos) {
			loco_t::rectangle_t::properties_t props;
			props.position = pos;
			props.color = fan::colors::white;
			props.size = _cell_size;
			props.camera = &get_singleton().camera;
			props.viewport = &get_singleton().viewport;
			_cell_size = get_cell_size();
			drawable = props;
		}

		// Static
	private:
		enum Era {
			Invalid,
			Past,
			Present,
			Future
		};
		
		static int _gen_curr;
		static std::vector<std::vector<Cell>> _cache;
		static inline fan::vec2 _cell_size;

		// Relative to current era: 
		// returns past, present or future
		static Era get_current_era(int gen) {
			if (gen < 0) return Invalid;
			if (gen < _gen_curr) return Past;
			if (gen == _gen_curr) return Present;
			return Future;
		}

	public:
		static bool is_cell(fan::vec2 point) {
			for (size_t i = 0; i < singleton.cells.size() - 1; i++)
			{
				if (singleton.cells[i].position == point) return true;
			}
			return false;
		}

		static fan::vec2 get_cell_size() {
			return Singleton::get_singleton().loco.get_window()->get_size() / DEFAULT_DIVISOR;
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
					tick();
				}
			}

			_gen_curr = gen;
		}

		static void tick() {
			Singleton& singleton = get_singleton();
			singleton.ticking = false;
		}
	};

	loco_t loco;
	loco_t::camera_t camera;
	loco_t::viewport_t viewport;

	bool ticking = false;
	std::vector<Cell> cells;
};

Singleton singleton;
Singleton& Singleton::get_singleton() { return singleton; }



void main() {
	singleton.loco.loop([&] {
		Singleton::Cell::tick();
	});
}