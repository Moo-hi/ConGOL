# Conway's Game of Life
## Overview
C++ adaptation of the famous Conway's Game of Life, using C++20 standard & OpenGL/Vulkan for graphics via a library made by  @reinterpret-pointer-cast.

## Controls:
- LMB : Draw cells
- RMB : Erase cells
- Space : Start/stop simulation  
- Shift+T+ScrollUp/Down : Evolve/De-evolve

## Rules
1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

Find more information on the game at [wikipedia.org](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
