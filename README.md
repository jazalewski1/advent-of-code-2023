My solutions to Advent of Code 2023 puzzles. Nothing fancy, just an exercice for my brain :)

Project is CMake based. To generate files (for example with ninja) run:
```
cmake -G Ninja -B build -S .
```

Convention for day numbering is as follows: 01, 02, 03, ..., 10, 11, 12, ...

Code with solutions can be found in `day.../src/Task.cpp`.

Each day is a separate target named `d<day_number>p<part_number>`, for example `day 3 part 2 = d03p2`.
So to compile run:
```
ninja all
ninja d03p2
```

Executable files are divided into parts stored in `build/bin/day...` directories.
To execute a solution provide input file and pass it to executable as first argument, for example:
```
build/bin/day03/part2 day3/input.txt
```

There is also a bash script to run each solution, which by default takes the `input.txt` file as input,
but optional third option can be provided with other file:
```
./run day03 part2
./run day03 part2 day03/example-input.txt
```

There is also a bash script to test each solution against an expected answer, which looks for line with the answer
and compares them.
Just pipe solution output to this script like so:
```
./run day03 part2 | ./test 12345
```
