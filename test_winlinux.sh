#!/bin/bash

RED="\033[1;31m"
GREEN="\033[1;32m"
NOCOLOR="\033[0m"
YELLOW="\033[0;33m"

echo -e "\n\n"
echo -e "${YELLOW}Please make sure player is in world and is in flying mode before testing${NOCOLOR}"
echo -e "\n\n"

cd Tests_winlinux

make -C .. clean && make -C ..


../mazerunner --testing < test_generateMazeTerminal.input > test_generateMazeTerminal.real

diff test_generateMazeTerminal.real test_generateMazeTerminal.expout

echo -e "\n\n"
if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_generateMazeTerminal passed.${NOCOLOR}"
else
    echo -e "${RED}test_generateMazeTermina FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_readMazeTerminal.input > test_readMazeTerminal.real

diff test_readMazeTerminal.real test_readMazeTerminal.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_readMazeTerminal passed.${NOCOLOR}"
else
    echo -e "${RED}test_readMazeTerminal FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_readMazeInputError.input > test_readMazeInputError.real

diff test_readMazeInputError.real test_readMazeInputError.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_readMazeInputError passed.${NOCOLOR}"
else
    echo -e "${RED}test_readMazeInputError FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_small_maze.input > test_small_maze.real

diff test_small_maze.real test_small_maze.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_small_maze passed.${NOCOLOR}"
else
    echo -e "${RED}test_small_maze FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_big_maze.input > test_big_maze.real

diff test_big_maze.real test_big_maze.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_big_maze passed.${NOCOLOR}"
else
    echo -e "${RED}test_big_maze FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_too_small.input > test_too_small.real

diff test_too_small.real test_too_small.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_too_small passed.${NOCOLOR}"
else
    echo -e "${RED}test_too_small FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_build_n_solve.input > test_build_n_solve.real

diff test_build_n_solve.real test_build_n_solve.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_build_n_solve passed.${NOCOLOR}"
else
    echo -e "${RED}test_build_n_solve FAILED.${NOCOLOR}"
fi


../mazerunner --testing < test_solve_before_build.input > test_solve_before_build.real

diff test_solve_before_build.real test_solve_before_build.expout

if [ $? -eq 0 ]; then
    echo -e "${GREEN}test_solve_before_build passed.${NOCOLOR}"
else
    echo -e "${RED}test_solve_before_build FAILED.${NOCOLOR}"
fi

echo -e "\n\n"

