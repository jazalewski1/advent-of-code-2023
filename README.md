My solutions to Advent of Code 2023 puzzles. Nothing fancy, just an exercice for my brain :)

Project is CMake based. To generate files (for example with ninja) run:
```
cmake -G Ninja -B build -S .
```

Each day is a separate target named `d<day_number>p<part_number>`, for example `day 3 part 2 = d3p2`.
So to compile run:
```
ninja all
# OR
ninja d3p2
```

Executable files are divided into parts stored in `build/bin/day...` directories.
To execute a solution provide input file and pass it to executable as first argument, for example:
```
build/bin/day3/part2 day3/input.txt
```

There is also a bash script to run each solution, which requires the `input.txt` file to be present in day directory.
```
./run day3 part2
```
