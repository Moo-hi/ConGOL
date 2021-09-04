//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConGOL.cpp : This file contains the 'main' function. Program execution begins and ends there.                                                            //
//                                                                                                                                                          //
//  PURPOSE:                                                                                                                                                //
//  A recreation of Conway's Game of Life, powered by OpenGL                                                                                                //
//  v1.0: 12.06.2021                                                                                                                                        //
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

#include "Grid.h"
#include "Utils.h"

#include <fan/graphics/graphics.hpp>
#include <fan/graphics/gui.hpp>
#include <fan/math/random.hpp>
#include <thread>


//* To-Do *\\
// 
// GENERAL:
// - Zoom +/-
// - Measuring tape
// - Convert grid cell vec to 2D vec
// 
//  Known bugs:
//  - Crash if a tick causes a cell generation that overlaps a corner .. corners overlap handling, TBD!
//  - Not a bug, but tickrate works counter-intuitively; lowering increases simulation speed & vice versa


int main()
{
    // // // // // // // // // // // // //
    //      *** USER VARIABLES ***      //
    //  Base grid subdivisions:         //
    int subdivs = 60;
    //                                  //
    //  Lower equals faster             //
    int tickrate = 25;
    //                                  //
    // // // // // // // // // // // // //

    fan::set_console_visibility(false);
    fan::window window(Utils::RoundToLowerPerfectSquare(fan::get_resolution()) - 100, "Conway's Game of Life");
    
    window.set_max_fps(165);
    window.set_vsync(false);

    fan::camera camera(&window);

    fan_2d::graphics::gui::text_renderer text(&camera);
    
    Grid grid(&camera, subdivs);

    // General settings
    bool show_fps = false;

    // Utility variables - due to be refactored away
    bool paint_live = false; // painting live cells .. not sure if user-related
    bool paint_dead = false;  // painting dead cells .. not sure if user-related
    int count = 0;
    
    /* Key bindings */ 
    // Mousefan::key_state::presspaint_live = false;
    window.add_key_callback(fan::mouse_left, fan::key_state::press, [&]() { paint_live = true; });
    window.add_key_callback(fan::mouse_left, fan::key_state::release, [&]() { paint_live = false; });

    window.add_key_callback(fan::mouse_right, fan::key_state::press, [&]() { paint_dead = true; });
    window.add_key_callback(fan::mouse_right, fan::key_state::release, [&]() { paint_dead = false; });

    // ESC: Close game
    window.add_key_callback(fan::key_escape, fan::key_state::press, [&]() { window.close(); });

    // F: Toggle FPS
    window.add_key_callback(fan::key_f, fan::key_state::press, [&]() { show_fps = !show_fps; window.set_name("Conway's Game of Life"); });

    // Space: Toggle simulation
    window.add_key_callback(fan::key_space, fan::key_state::press, [&]() { grid.toggle_simulation(); });

    // Shift+T+ScrollUp: Evolve or forward to next generation depending on if the generation is already recorded or not. 
    // Shift+T+ScrollDown: Devolve to earlier generation if it exists
    window.set_keys_callback([&](uint16_t key, fan::key_state state) {
        if (window.key_press(fan::key_t) && key == fan::mouse_scroll_up) {
            grid.evolve();
        }
        else if (window.key_press(fan::key_t) && key == fan::mouse_scroll_down) {
            grid.devolve();
        }
        });

    window.loop([&] {
        
        if (show_fps) window.get_fps();
        
        if (paint_live) grid.set_alive_at_click();
        if (paint_dead) grid.set_dead_at_click();

        // ugly & in terms of setting tickrate a bit counterintuitive, but works for now
        if (count > tickrate && grid.ticking_) {
            grid.evolve();
            count = 0; 
        }
        else if (grid.ticking_) count++;

        grid.draw();
    }); 
}