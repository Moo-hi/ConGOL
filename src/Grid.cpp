#include <cmath>
#include "Grid.h"
#include "Utils.h"
// Container
Grid::Grid() {};

// Initialize from scratch
Grid::Grid(fan::window_t* window, fan::opengl::context_t* context, int subdivisions) {
	rects_.open(context);
	rects_.enable_draw(context);
	this->context = context;
	this->window = window;
	this->init(subdivisions);
}

// Initialize from save
Grid::Grid(fan::window_t* window, fan::opengl::context_t* context, CellData cell_data) {
	rects_.open(context);
	rects_.enable_draw(context);
	this->context = context;
	this->window = window;
	this->init(1);
	this->import(cell_data);
}

void Grid::run() {
	// Lower  value = faster simulation
	int tickrate = 25;


	//fan_2d::graphics::gui::text_renderer text(this->camera_); currently unused
	

	int count = 0;

	while (true) {

		uint32_t window_event = window->handle_events();
    if(window_event & fan::window_t::events::close){
      window->close();
      break;
    }

		if (show_fps) window->get_fps();

		if (paintingLive) set_alive_at_click();
		if (paintingDead) set_dead_at_click();

		// ugly, but works for now
		if (count > tickrate && ticking_) {
			evolve();
			count = 0;
		}
		else if (ticking_) count++;

		draw();

    context->process();
    context->render(window);

	}
}

std::vector<Grid::Cell> Grid::get_live_cells() {
	std::vector<Cell> live_cells;
	
	for (int i = 0; i < cells_.size(); i++)
	{
		if (cells_[i].alive) live_cells.push_back(cells_[i]);
	}

	return live_cells;
}

std::vector<Grid::Cell> Grid::get_live_cells(std::vector<Cell> cells) {
	std::vector<Cell> live_cells;
	
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells[i].alive) live_cells.push_back(cells[i]);
	}

	return live_cells;
}

void Grid::init(int subdivisions) {
	if (window != NULL)
	{
		// Clean previous data, whether it exists or not
		this->cells_.clear();
		this->map_.clear();

		// Determine size of a single cell
		this->cell_size_ = fan::cast<float>(window->get_size()) / subdivisions;

		// Fill current grid with dead cells
		for (size_t i = 0; i < ((long long int)subdivisions * subdivisions); i++)
		{
			this->cells_.push_back(Cell(false, cells_.size()));
		}

		// Offset drawing points by cell size
		fan::vec2 offset(cell_size_.x, cell_size_.y);

		fan::vec2 loc(0, 0);
		for (size_t i = 0; i < subdivisions; i++)
		{
			for (size_t i = 0; i < subdivisions; i++)
			{
				map_.push_back(loc + cell_size_ / 2);
				loc.x += offset.x; // wnd_x / 20
			}
			loc.x = 0;
			loc.y += offset.y; // wnd_y / 20
		}
	}
	else fan::print("Grid::init Failure: Null window pointer");

	cursor_rects_.open(context);
	cursor_rects_.enable_draw(context);
	fan_2d::graphics::rectangle_t::properties_t p;
	p.position = 0;
	p.size = cell_size_ / 2;
	p.color = fan::colors::cyan;
	cursor_rects_.push_back(context, p); // selection highlight bg (fake outline)
	p.size = (cell_size_ * 0.875) / 2;
	p.color = color_dead_;
	cursor_rects_.push_back(context, p); // selection highlight filler (completes illusion of outline)
}

void Grid::import(CellData cell_data) {
	this->cells_.clear();
	this->map_.clear();

	this->cells_ = cell_data.cells_;
	this->map_ = cell_data.map_;
	this->cell_size_ = cell_data.cell_size_;
}

void Grid::import(int i) {
	if (history_.size() >= i) { 
		this->import(history_[i]); 
		slot_ = i;
		fan::print("Current slot:", slot_);
		fan::print("History size:", history_.size());
	}
}

void Grid::toggle_simulation() {
	ticking_ = !ticking_;
}

Grid::cellvec2 Grid::cv_to_cv2D(cellvec& cv) {
	cellvec2 cv2;

	const int row_size = horizontal_increment_;

	// As long as there are cells left
	int cell_count = 0;
	cellvec row_to_be_added;
	for (size_t i = 0; i < cv.size(); i++)
	{
		// Fill rows with cells if they're not full
		if (cell_count != row_size) {
			row_to_be_added.push_back(cv[i]);
			cell_count++;
		}
		else {
			// If row is full, move on to next row & re-init the cell_count. Push the row & reset for next round
			cell_count = 0;
			cv2.push_back(row_to_be_added);
			row_to_be_added.clear();
			i--; // Offset iterator i as we didn't use a cell during this iteration
		}
	}
	cv2.push_back(row_to_be_added);

	return cv2;
}

Grid::cellvec Grid::cv2D_to_cv(cellvec2& cv2) {
	cellvec cv;

	for (size_t row = 0; row < cv2.size(); row++)
	{
		// Column
		for (size_t i = 0; i < cv2[row].size(); i++)
		{
			cv.push_back(cv2[row][i]);
		}
	}

	return cv;
}


// Apply the game rules & necessary corrections upon boundary overlaps (side overlap
void Grid::evolve() {
	// unimplemented
	//cellvec2 cells2D_ = cv_to_cv2D(this->cells_); // convert cells from 1D to 2D
	// unimplemented ^^^^^^

	std::vector<Cell> live_cells = get_live_cells();

	// Save current state
	slot_++;
	history_.push_back(CellData(this->cells_, this->map_, this->cell_size_));
	fan::print("Evolved   to slot: ", slot_);
	//

	std::vector<int> live_indices;
	std::vector<int> kill_indices; // is there a better, in this case, more readable solution?

	

	for (int i = 0; i < live_cells.size(); i++)
	{
		// Any live cell with fewer than two live neighbours dies, as if by underpopulation
		if (get_live_cells(live_cells[i].perimeter(*(this))).size() < 2) kill_indices.push_back(live_cells[i].index);

		// Any live cell with two or three live neighbours lives on to the next generation
		if (get_live_cells(live_cells[i].perimeter(*(this))).size() == 2 || get_live_cells(live_cells[i].perimeter(*(this))).size() == 3) live_indices.push_back(live_cells[i].index);

		// Any live cell with more than three live neighbours dies, as if by overpopulation
		if (get_live_cells(live_cells[i].perimeter(*(this))).size() > 3) kill_indices.push_back(live_cells[i].index);

		// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction (not added to queue as release moment doesn't matter for last one)
		std::vector<Cell> cell_perimeter = live_cells[i].perimeter(*(this)); // get perimeter of i (live_cell)
		for (int i = 0; i < cell_perimeter.size(); i++)
		{
			// Takes perimeters of perimeter cells
			if (get_live_cells(cell_perimeter[i].perimeter(*(this))).size() == 3)
			{
				live_indices.push_back(cell_perimeter[i].index);
			}
		}
	}

	// release KillIndices
	for (int i = 0; i < kill_indices.size(); i++)
	{
		cells_[kill_indices[i]].alive = false;
	}

	// release LiveIndices
	for (int i = 0; i < live_indices.size(); i++)
	{
		cells_[live_indices[i]].alive = true;
	}
}

void Grid::devolve() {
	if (slot_ != 0) {
		--slot_;
		import(history_[slot_]);
		history_.pop_back();
		fan::print("Devolved  to slot: ", slot_);
	}
}

uint32_t Grid::translate_mouse_to_gridmap() {  // could use a better; shorter name without sacrificing readability
	fan::vec2i cell_origin = (window->get_mouse_position() / cell_size_).floor();

	uint32_t index = cell_origin.y * get_window_divisor() + cell_origin.x;

	return fan::clamp(index, (uint32_t)0, (uint32_t)cells_.size() - 1); // clamp index between boundaries & return
}

void Grid::set_alive_at_click() {
	int i = translate_mouse_to_gridmap();
	cells_[i].alive = true;
	rects_.set_color(context, 1, color_alive_); // a confusing line - updates the highlight filler to match the new state ... refactor away
	update_cursor_highlight();
}

void Grid::set_dead_at_click() {
	int i = translate_mouse_to_gridmap();
	cells_[i].alive = false;
	rects_.set_color(context, 1, color_dead_); // a confusing line - updates the highlight filler to match the new state ... refactor away
	update_cursor_highlight();
}

// Draw based on object data
void Grid::draw() {
	// Initialize grid_ for drawing if uninitialized 
	if (rects_.size(context) == 0) { 
		for (int i = 0; i < cells_.size(); i++)
		{
			fan_2d::graphics::rectangle_t::properties_t p;
			p.position = map_[i] - p.size;
			p.size = cell_size_ / 2;
			p.color = color_dead_;
			rects_.push_back(context, p);
		}
	}

	// Determine and set cell color (alive? dead?)
	for (int i = 0; i < cells_.size(); i++)
	{
		if (cells_[i].alive) { rects_.set_color(context, i, color_alive_); }
		else { rects_.set_color(context, i, color_dead_); };// If cell is alive, color - else, leave black (dead)
	}

	update_cursor_highlight();
}