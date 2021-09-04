# Conway's Game of Life
## Overview
C++ adaptation of the famous Conway's Game of Life, using C++20 standard & OpenGL for graphics via a library made by  @reinterpret-pointer-cast.

![congol](https://user-images.githubusercontent.com/57489963/124203736-0e2b8b00-dae6-11eb-8976-2f1e581ee44e.gif)

### Rules
1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

Find more information on the game at [wikipedia.org](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

## Controls:
- LMB : Draw cells
- RMB : Erase cells
- Space : Start/stop simulation  
- Shift+T+ScrollUp/Down : Evolve/de-evolve
- F : Show FPS

## A very rough description on the technical design
Class Grid contains a draw method, which draws based on values stored on a private variable named cells_ (a vector of Cell-type objects).
These Cell objects contain an index & the status (live/dead)
Grid.Evolve() contains pretty much all the vital game logic itself (modifies cells_ according to the game's rules) - if you feel like contributing, you are very much welcome!

## Known issues:
- Crash if evolution extends outside the window (overlap handling is unimplemented in 0.9)
