#pragma once

#include <fan/graphics/graphics.hpp>
#include <fan/graphics/gui.hpp>
#include <fan/math/random.hpp>
#include <vector>

class Grid
{
private:
	struct Cell { // Could use refactoring, perhaps
		int index;
		bool alive;

		Cell(bool alive, uint32_t index) {
			this->alive = alive;
			this->index = index;
		}

		// Returns the upmost cell index relative to argument cell
		int upmost(Grid& grid) {
			uint32_t i = index;

			while (i - grid.get_window_divisor() > 0) {
				i -= grid.get_window_divisor();
			}
			return i;
		}

		// Returns the bottom cell index relative to argument cell
		int bottom(Grid& grid) {
			int i = index;
			while (i + grid.get_window_divisor() < grid.cells_.size()) {
				i += grid.get_window_divisor();
			}
			return i;
		}

		// Returns indices (8) around a cell (top to bottom)
		std::vector<Cell> perimeter(Grid& grid) {
			int i = index;

			// lazy solution, but scalable - definitely in need of a better one
			std::vector<Cell> perimeter;
			int vertical_increment = std::sqrt(grid.cells_.size());

			
			i = i - vertical_increment - 1;
			perimeter.push_back(grid.cells_[i]);

			i++;
			perimeter.push_back(grid.cells_[i]);

			i++;
			perimeter.push_back(grid.cells_[i]);

			i = i + vertical_increment - 2;
			perimeter.push_back(grid.cells_[i]);

			/*i++;							// the cell itself
			perimeter.push_back(i);*/

			i = i + 2;
			perimeter.push_back(grid.cells_[i]);

			i = i + vertical_increment - 2;
			perimeter.push_back(grid.cells_[i]);

			i++;
			perimeter.push_back(grid.cells_[i]);

			i++;
			perimeter.push_back(grid.cells_[i]);

			return perimeter;
		}
	};

	struct Corners {
		int UpperLeft;
		int UpperRight;
		int LowerLeft;
		int LowerRight;

		Corners(Grid grid)
			:
			UpperLeft(0), 
			UpperRight(std::sqrt(grid.cells_.size()) - 1), 
			LowerLeft(std::sqrt(grid.cells_.size()) * (std::sqrt(grid.cells_.size()) - 1)),
			LowerRight(grid.cells_.size() - 1) {}

		Corners(int UpperLeft, int UpperRight, int LowerLeft, int LowerRight) {
			this->UpperLeft = UpperLeft;
			this->UpperRight = UpperRight;
			this->LowerLeft = LowerLeft;
			this->LowerRight = LowerRight;
		}
	};

	struct Sides {
		// Horizontal sides
		std::vector<Cell> ab;
		std::vector<Cell> cd;

		// Vertical sides
		std::vector<Cell> ac;
		std::vector<Cell> bd;

		Sides(Grid& grid)
		{
			int sidelength = grid.get_window_divisor();

			for (int a_1 = Corners(grid).UpperLeft, c = Corners(grid).LowerLeft, a_2 = Corners(grid).UpperLeft, b = Corners(grid).UpperRight; a_1 < sidelength; a_1++, c++, a_2 += sidelength, b += sidelength)
			{
				//		a _ b			
				//		 |_|			
				//		c   d	

				// side ab (move from left to right with increments of 1, starting from point a)
				ab.push_back(grid.cells_[a_1]);

				// side cd (move from left to right with increments of 1, starting from point c)
				cd.push_back(grid.cells_[c]);

				// side ac (fall starting from point a with vertical_increment, starting from point a)
				ac.push_back(grid.cells_[a_2]);

				// side bd (fall starting from point b with vertical_increment, starting from point a)
				bd.push_back(grid.cells_[b]);
			}
		}

		bool CorrectOverlaps(std::vector<Cell>& cells, Corners& corners) {
			//std::vector<Cell> side_cells = FindSides(cells, corners);
		}
	};

	struct CellData {
		std::vector<Cell> cells_;
		std::vector<fan::vec2> map_;
		fan::vec2 cell_size_;
		
		CellData() {}
		CellData(std::vector<Cell> cells, std::vector<fan::vec2> map, fan::vec2 cell_size) {
			cells_ = cells;
			map_ = map;
			cell_size_ = cell_size;
		}

		CellData(Grid* grid) {
			cells_ = grid->cells_;
			map_ = grid->map_;
			cell_size_ = fan::cast<float>(grid->camera_->m_window->get_size()) / sqrt(cells_.size());
		}

		void import_file() {

		}

		void export_file() {

		}

	};


	inline static fan::camera* camera_; // includes window as a member variable
	//inline static fan_2d::graphics::gui::text_renderer* text_;
	
	fan_2d::graphics::rectangle rects_;
	fan_2d::graphics::rectangle cursor_rects_;

	// Current save slot
	uint32_t slot_ = 0;

	const int horizontal_increment_ = 1;

	// Stores each generation of cells, or more generally, each movement
	std::vector<CellData> history_;
	std::vector<fan::vec2> map_; // Grid coordinates of each cell (for graphical representation of cells)
	std::vector<Cell> cells_;	// Stores cell data
	fan::vec2 cell_size_;

	// How many times current window is divided
	int get_window_divisor() {
		return (int)sqrt(cells_.size());
	}

	void fading_text_notification(int i, fan::fstring text, fan::vec2 position, fan::color text_color=fan::colors::aqua, f32_t font_size=13) {
		//text_.set_text(i, text);
		//text_.set_position(i, position);
		//text_.set_text_color(i, text_color);
		//text_.set_font_size(i, font_size);
		
		//if (outline_color != fan::uninitialized) text_.set_outline_color(i, outline_color);

		// todo 
		//std::thread test([&r, &g, &b, &a, &testhook]() {
		//    const int MIN = 128;
		//    const int MAX = 255;
		//    const int DELAY_MS = 1000;

		//    while (true) {
		//        ThreadedColorModifiers::FlipFade(&r, NULL, NULL, &a, MIN, MAX, false); // fade down
		//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
		//        ThreadedColorModifiers::FlipFade(&r, NULL, NULL, &a, MIN, MAX, true); // r up
		//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));

		//        ThreadedColorModifiers::FlipFade(NULL, &g,NULL, &a, MIN, MAX, false); // fade down
		//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
		//        ThreadedColorModifiers::FlipFade(NULL, &g, NULL, &a, MIN, MAX, true); // g up
		//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));

		//        ThreadedColorModifiers::FlipFade(NULL, NULL, &b, &a, MIN, MAX, false); // fade down
		//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
		//        ThreadedColorModifiers::FlipFade(NULL, NULL, &b, &a, MIN, MAX, true); // b up
		//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
		//    }
		//    });
		//test.detach();
	}

	void update_cursor_highlight() { // make proper abstractions
		const int bg_rect_indice = 0;
		const int filler_rect_indice = 1;
		const int cursor_rect_indice = 2;

		int i = translate_mouse_to_gridmap();
		if (cells_[i].alive) {
			cursor_rects_.set_color(filler_rect_indice, color_alive_);
		}
		else if (!cells_[i].alive) {
			cursor_rects_.set_color(filler_rect_indice, color_dead_);
		}

		cursor_rects_.set_position(bg_rect_indice, map_[i]);
		cursor_rects_.set_position(filler_rect_indice, map_[i]);
	}

public:
	inline static bool ticking_ = false;

	fan::color color_alive_ = fan::colors::white;
	fan::color color_dead_ = fan::colors::black;
	
	Grid(); // container
	Grid(fan::camera* camera, int subdivisions); // Initialize from scatch
	Grid(fan::camera* camera, CellData cell_data); // Initialize from save

	// Initializes cell data & -mapping
	void init(int subdivisions);

	// Import another grid
	void import(int i);
	void import(CellData cell_data);

	// Change state of simulation (play/pause)
	void toggle_simulation();

	// Brings up the measuring tool for aid in drawing
	void toggle_measure();

	// Proceed a step in evolution according to the game's rules
	void evolve();

	// It's evolving, just backwards!
	void devolve();

	// Returns the corresponding cell map indice determined from mouse click point
	uint32_t translate_mouse_to_gridmap();

	void set_alive_at_click();

	void set_dead_at_click();

	std::vector<Cell> get_live_cells();
	std::vector<Cell> get_live_cells(std::vector<Cell> cells);

	void draw();
};

