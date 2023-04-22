# Conway's Game of Life
## Overview
C++20 adaptation of the famous Conway's Game of Life.

------------

*A looping pattern that emerged in simulation as per the rules below.*

![congol](https://user-images.githubusercontent.com/57489963/124203736-0e2b8b00-dae6-11eb-8976-2f1e581ee44e.gif)


### Rules
1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

Find more information about the game [here](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life).

## Controls
- LMB : Draw cells
- RMB : Erase cells
- Space : Start/stop simulation  
- Shift+T+MousewheelUp : Evolve
- Shift+T+MousewheelDown : De-evolve
- F : Show FPS in window frame

## Credits
Graphics library: @6413
