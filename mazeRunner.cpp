#include <mcpp/mcpp.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Node.h"
#include "menuUtils.h"

#define NORMAL_MODE 0
#define TESTING_MODE 1
#define HD_MODE 2

// Better than looking at std<vec<ve...
typedef std::vector<std::vector<int>> Maze_T;
// A nice tuple we use to pass around all needed info about a maze
typedef std::tuple<std::array<int, 3>, Maze_T, std::array<int, 2>> M_Pack;
typedef mcpp::MinecraftConnection mc_con;

// We use this to reset the world at the end
class CoordKey {
   public:
    CoordKey(mcpp::Coordinate coord) : coord(coord){};
    mcpp::Coordinate coord;

    // We use an unordered map, so need to provide hash for Coorinate class
    struct Hasher {
        std::size_t operator()(const CoordKey &key) const {
            std::size_t hx = std::hash<int>()(key.coord.x);
            std::size_t hy = std::hash<int>()(key.coord.y);
            std::size_t hz = std::hash<int>()(key.coord.z);
            return hx ^ (hy << 1) ^ (hz << 2);
        };
    };

    // This is used when checking if a Coord already exists
    bool operator==(const CoordKey &other) const {
        bool x_eq = coord.x == other.coord.x;
        bool y_eq = coord.y == other.coord.y;
        bool z_eq = coord.z == other.coord.z;
        return x_eq && y_eq && z_eq;
    };
};

static std::unordered_map<CoordKey, mcpp::BlockType, CoordKey::Hasher>
                                                            edited_blocks;



void print_maze_inputs(std::array<int, 3> bp, int l, int w, Maze_T maze) {
    std::cout << std::endl;
    std::cout << "Maze read successfully" << std::endl;
    std::cout << "**Printing Maze**" << std::endl;

    std::cout << "BasePoint: (" << bp[0] << ", " << bp[1] << ", " << bp[2]
        << ")" << std::endl;

    std::cout << "Structure:" << std::endl;

    for (std::vector<int> row : maze) {
        for (int &c : row) {
            std::string val = c == 1 ? "x" : (c == 2 ? "." : " ");
            std::cout << val;
        }
        std::cout << std::endl;
    }

    std::cout << "**End Printing Maze**" << std::endl;

    std::cout << "Enter any key to continue:" << std::endl;
    std::string input = "";
    std::cin >> input;
}


std::tuple<int, int> get_length_width() {
    bool got_lw{false};

    // These need to be odd numbers
    int maze_length, maze_width = 0;

    while (!got_lw) {
        std::cout << "Enter the length and width of maze:" << std::endl;
        std::string string_l, string_w = "";
        std::cin >> string_l >> string_w;

        try {
            /// We are swapping here because i messed this up in the rest of
            /// the code. We want width along X axis
            maze_width = std::stoi(string_l);
            maze_length = std::stoi(string_w);
            if (maze_length % 2 == 1 && maze_width % 2 == 1) {
                if (maze_length < 3 || maze_width < 3) {
                    std::cout << "Length and width need to both be > 3"
                              << std::endl;
                } else {
                    got_lw = true;
                }
            } else {
                std::cout << "Length and width need to be odd" << std::endl;
            }
        } catch (...) {
            std::cout << "Something was funky with your input. Try again"
                      << std::endl;
        }
    }

    return std::make_tuple(maze_length, maze_width);
}

std::array<int, 3> get_basepoint() {
    bool got_basepoint{false};
    std::array<int, 3> bp;

    while (!got_basepoint) {
        got_basepoint = true;
        // This needs to be y + 1 above getHeight(X, Z)
        std::cout << "Enter the basePoint of maze:" << std::endl;
        for (size_t i = 0; i < 3; i++) {
            std::string coord = "";
            std::cin >> coord;
            try {
                int t = std::stoi(coord);
                bp[i] = t;
            } catch (...) {
                got_basepoint = false;
                std::cout << "Something was funky with your input. Try again"
                          << std::endl;
            }
        }
    }

    return bp;
}

Maze_T read_maze(int length, int width) {
    bool got_maze{false};
    // we need to check if this is a perfect maze + the chars in it
    // We are currently assuming the input is pasted in

    Maze_T maze_structure;

    while (!got_maze) {
        got_maze = true;  // because no break statements

        std::cout << "Enter the maze structure:" << std::endl;
        std::string row = "";
        std::cin.ignore();  // clear the cin buffer
        Maze_T possible_maze_structure(length, std::vector<int>(width, 0));

        for (size_t i = 0; i < (size_t)length; i++) {
            std::getline(std::cin, row);
            for (size_t j = 0; j < (size_t)width; j++) {
                if (row[j] == 'x' || row[j] == '.') {
                    int val = row[j] == 'x' ? 1 : 2;
                    possible_maze_structure[i][j] = val;
                } else
                    got_maze = false;
            }
        }

        if (!got_maze) {
            std::cout << "Something was funky with your input. Try again"
                      << std::endl;
        } else {
            maze_structure = possible_maze_structure;
        }
    }

    return maze_structure;
}

void show_team_info() {
    printTeamInfo();
    std::cout << "Enter any key to continue" << std::endl;
    std::string input = "";
    std::cin >> input;
}

// Purely fun !!
void animateHD() {
    std::string HD[] = {
      " /$$   /$$ /$$$$$$$        /$$      /$$  /$$$$$$  /$$$$$$$  /$$$$$$$$",
      "| $$  | $$| $$__  $$      | $$$    /$$$ /$$__  $$| $$__  $$| $$_____/",
      "| $$  | $$| $$  \\ $$      | $$$$  /$$$$| $$  \\ $$| $$  \\ $$| $$   ",
      "| $$$$$$$$| $$  | $$      | $$ $$/$$ $$| $$  | $$| $$  | $$| $$$$$   ",
      "| $$__  $$| $$  | $$      | $$  $$$| $$| $$  | $$| $$  | $$| $$__/   ",
      "| $$  | $$| $$  | $$      | $$\\  $ | $$| $$  | $$| $$  | $$| $$     ",
      "| $$  | $$| $$$$$$$/      | $$ \\/  | $$|  $$$$$$/| $$$$$$$/| "
      "$$$$$$$$",
      "|__/  |__/|_______/       |__/     |__/ \\______/ |_______/ "
      "|________/"};

    int n = sizeof(HD) / sizeof(HD[0]);

    std::srand(std::time(nullptr));

    std::string tempHD[n];
    for (int i = 0; i < n; ++i) {
        tempHD[i] = HD[i];
    }

    int cycles = 100;
    for (int cycle = 0; cycle < cycles; ++cycle) {
        int temp = system("clear");
        if (temp == -1) {
        }  // Clear terminal (Unix-like systems)

        for (int i = 0; i < n; ++i) {
            std::string tempStr = HD[i];
            for (unsigned j = 0; j < tempStr.length(); ++j) {
                if (tempStr[j] == '$') {
                    char randomChar = 33 + std::rand() % 94;
                    tempStr[j] = randomChar;
                }
                for (int k = (cycles - cycle) / 4; k > 0; k--) {
                    std::cout << " ";
                }
            }
            std::cout << tempStr << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int cycle = 0; cycle < 50; ++cycle) {
        int temp = system("clear");
        if (temp == -1) {
        }

        for (int i = 0; i < n; ++i) {
            std::string tempStr = HD[i];
            for (unsigned j = 0; j < tempStr.length(); ++j) {
                if (tempStr[j] == '$') {
                    char randomChar = 33 + std::rand() % 94;
                    tempStr[j] = randomChar;
                }
            }
            std::cout << tempStr << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int cycle = 0; cycle < 10; ++cycle) {
        int temp = system("clear");
        if (temp == -1) {
        }

        for (int i = 0; i < n; ++i) {
            for (unsigned j = 0; j < tempHD[i].length(); ++j) {
                if (tempHD[i][j] != ' ' && tempHD[i][j] != '|' &&
                    tempHD[i][j] != '/' && tempHD[i][j] != '_' &&
                    tempHD[i][j] != '\\') {
                    tempHD[i][j] = '$';
                }
            }
            std::cout << tempHD[i] << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


// Maze_T is a 2d vector of INTS, not STRINGS, we print STRINGS for humans
Maze_T get_base_maze(int length, int width) {
    Maze_T empty_maze(length, std::vector<int>(width, 0));

    // print a board
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (i % 2 == 0 || j % 2 == 0) empty_maze[i][j] = 1;
        }
    }

    return empty_maze;
}

// Manhandles the maze in place, no need to return
void recursive_backtracking(int y_pos, int x_pos, Maze_T &maze, int mode) {

    // First we set where we are to visited
    maze[y_pos][x_pos] = 2;

    // This checks if we have visited all sides around us
    bool all_sides_found = false;

    if (x_pos >= 2 && y_pos >= 2 && x_pos + 2 < (int)maze[0].size() &&
        y_pos + 2 < (int)maze.size()) {

        // All indices are in bounds
        all_sides_found =
            (maze[y_pos - 2][x_pos] == 2) && (maze[y_pos + 2][x_pos] == 2) &&
            (maze[y_pos][x_pos - 2] == 2) && (maze[y_pos][x_pos + 2] == 2);
    }

    // Then we are going to go through each side
    if (!all_sides_found) {
        std::vector<std::string> dirs = {"UP", "RIGHT", "DOWN", "LEFT"};
        while (dirs.size() > 0) {
            int dir_indx = rand() % dirs.size();
            if (mode == TESTING_MODE) {
                dir_indx = 0;
            }
            std::string dir = dirs[dir_indx];
            dirs.erase(std::remove(dirs.begin(), dirs.end(), dir), dirs.end());

            int new_cell_x = 0;
            int new_cell_y = 0;
            int wall_x = x_pos;
            int wall_y = y_pos;

            if (dir == "UP") {
                new_cell_x = x_pos;
                new_cell_y = y_pos - 2;
                wall_y = y_pos - 1;
            }

            if (dir == "RIGHT") {
                new_cell_x = x_pos + 2;
                new_cell_y = y_pos;
                wall_x = x_pos + 1;
            }

            if (dir == "DOWN") {
                new_cell_x = x_pos;
                new_cell_y = y_pos + 2;
                wall_y = y_pos + 1;
            }

            if (dir == "LEFT") {
                new_cell_x = x_pos - 2;
                new_cell_y = y_pos;
                wall_x = x_pos - 1;
            }

            // we are > 0 and + 1 < size because outer walls
            bool n_x_inbound =
                new_cell_x > 0 && new_cell_x + 1 < (int)maze[0].size();
            bool n_y_inbound =
                new_cell_y > 0 && new_cell_y + 1 < (int)maze.size();

            if (n_x_inbound && n_y_inbound) {
                if (maze[new_cell_y][new_cell_x] != 2) {
                    maze[wall_y][wall_x] = 2;
                    recursive_backtracking(new_cell_y, new_cell_x, maze, mode);
                }
            }
        }
    }
}


// Generates a random entrance on an EDGE
std::array<int, 2> gen_entrance(const int length, const int width) {
    int temp = rand() % 4;

    int start_x = 0;
    int start_y = 0;

    if (temp == 0) {
        start_x = 2 * (rand() % (length / 2)) + 1;
        start_y = 1;
    } else if (temp == 1) {
        start_x = 2 * (rand() % (length / 2)) + 1;
        start_y = width - 2;
    } else if (temp == 2) {
        start_x = 1;
        start_y = 2 * (rand() % (width / 2)) + 1;
    } else if (temp == 3) {
        start_x = length - 2;
        start_y = 2 * (rand() % (width / 2)) + 1;
    }

    return {start_x, start_y};
}

void place_gate(int start_y, int start_x, Maze_T &maze) {
    if (start_x == 1)
        maze[start_y][0] = 2;
    else if (start_x == (int)maze[0].size() - 2)
        maze[start_y][start_x + 1] = 2;

    else if (start_y == 1)
        maze[0][start_x] = 2;
    else if (start_y == (int)maze.size() - 2)
        maze[start_y + 1][start_x] = 2;
}

void setBlock(mc_con &mc, mcpp::Coordinate loc, mcpp::BlockType block) {
    mcpp::BlockType old_block = mc.getBlock(loc);
    mc.setBlock(loc, block);

    /*
     * Insert is perfect because it will not overwrite existing keys,
     * meaning we will always have a record of the FIRST block type
     * changed at the given pos, so we dont have to worry about
     * backtracking and rebuilding or generating mazes on top of other mazes
     */


    edited_blocks.insert({loc, old_block});
}

void reset_changed_blocks(mc_con &mc) {
    for (const auto &pair : edited_blocks) {
        mc.setBlock(pair.first.coord, pair.second);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }


    // Prevents mem leak on next maze build
    edited_blocks.clear();
}

void clear_base(mc_con &mc, std::array<int, 3> bp, int x_end, int z_end) {
    auto air = mcpp::Blocks::AIR;

    // Get the block type at the basepoint

    for (int x = bp[0]; x < x_end; x++) {
        for (int z = bp[2]; z < z_end; z++) {
            mcpp::Coordinate loc;
            auto y = mc.getHeight(x, z);
            auto gBlockType = mc.getBlock(mcpp::Coordinate(x, y, z));

            while (y > bp[1] - 1) {
                loc = mcpp::Coordinate(x, y, z);
                setBlock(mc, loc, air);
                y--;
            }

            while (y < bp[1]) {
                loc = mcpp::Coordinate(x, y, z);
                setBlock(mc, loc, gBlockType);
                y++;
            }
        }
    }
}


// Very simple for(for(for()))) that gets the job done
void build_maze(mc_con &mc, M_Pack maze_pack) {
    reset_changed_blocks(mc);

    auto [bp, maze, _] = maze_pack;
    mcpp::BlockType wood = mcpp::Blocks::ACACIA_WOOD_PLANK;

    // This is a bit confusing because Y in minecraft is up towards sky,
    // So baseppoint comes in x, y, z format where z is up

    int x_end = bp[0] + maze[0].size();
    int z_end = bp[2] + maze.size();
    int y_end = bp[1] + 3;

    // Clear the area!!
    clear_base(mc, bp, x_end, z_end);

    for (int x = bp[0]; x < x_end; x++) {
        for (int z = bp[2]; z < z_end; z++) {
            for (int y = bp[1]; y < y_end; y++) {
                // length is y, width is x
                if (maze[z - bp[2]][x - bp[0]] == 1) {
                    mcpp::Coordinate loc(x, y, z);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    setBlock(mc, loc, wood);
                }
            }
        }
    }
}

// This is the code that handles everything maze related when option is chosen
M_Pack handle_maze(mc_con &mc, bool from_inp, int mode) {
    auto bp = get_basepoint();
    auto [length, width] = get_length_width();

    Maze_T maze;
    std::array<int, 2> entrance = {1, 1};

    if (from_inp)
        maze = read_maze(length, width);
    else {
        maze = get_base_maze(length, width);

        if (mode == NORMAL_MODE || mode == HD_MODE) {
            entrance = gen_entrance(length, width);
        }

        place_gate(entrance[0], entrance[1], maze);
        recursive_backtracking(entrance[0], entrance[1], maze, mode);
    }

    print_maze_inputs(bp, length, width, maze);

    return std::make_tuple(bp, maze, entrance);
}

// Boring Maze Menu
M_Pack generate_maze_menu(mc_con &mc, int mode) {
    printGenerateMazeMenu();
    M_Pack result;
    bool got_choice{false};
    bool from_inp{false};

    while (!got_choice) {

        // choice can either be read maze, gen maze, or back
        std::string choice;
        std::cin >> choice;
        if (choice == "1") {
            from_inp = true;
            got_choice = true;
            result = handle_maze(mc, from_inp, mode);
        }

        else if (choice == "2") {
            got_choice = true;
            result = handle_maze(mc, from_inp, mode);
        }

        else if (choice == "3") {
            got_choice = true;
        }

        else {
            std::cout << "Input Error: Enter a number between 1 and 3..."
                      << std::endl;
        }
    }

    return result;
}

bool wood_exists_at(mc_con &mc, mcpp::Coordinate loc) {
    auto block = mc.getBlock(loc);
    return block == mcpp::Blocks::ACACIA_WOOD_PLANK;
}

// Ugliest code known to man, calculates the 'agents' dir and returns F + R
std::tuple<bool, bool, mcpp::Coordinate, mcpp::Coordinate, bool>
get_forward_and_right(mc_con &mc, int rotation, mcpp::Coordinate pLoc) {

    std::unordered_map<std::string, mcpp::Coordinate> dirs = {
        {"-x", mcpp::Coordinate(1, 0, 0)},
        {"+x", mcpp::Coordinate(-1, 0, 0)},
        {"-z", mcpp::Coordinate(0, 0, -1)},
        {"+z", mcpp::Coordinate(0, 0, 1)},
    };

    bool r_wall = false;
    bool f_wall = false;

    mcpp::Coordinate r_coord = mcpp::Coordinate(0, 0, 0);
    mcpp::Coordinate f_coord = mcpp::Coordinate(0, 0, 0);

    //     Right hand facing positive X axis
    //     0
    //     ^
    //     | ###
    //    Z| #X#
    //     | ###
    //     *--X-----> 90

    if (rotation > 360) {
        rotation = rotation % 360;
    }

    else if (rotation < -360) {
        rotation = rotation % -360;
    }

    if (rotation == 0 || rotation == -360 || rotation == 360) {
        r_wall = wood_exists_at(mc, pLoc + dirs["+x"]);
        f_wall = wood_exists_at(mc, pLoc + dirs["+z"]);
        r_coord = dirs["+x"];
        f_coord = dirs["+z"];
    }

    else if (rotation == 90 || rotation == -270) {
        r_wall = wood_exists_at(mc, pLoc + dirs["-z"]);
        f_wall = wood_exists_at(mc, pLoc + dirs["+x"]);
        r_coord = dirs["-z"];
        f_coord = dirs["+x"];
    }

    else if (rotation == 180 || rotation == -180) {
        r_wall = wood_exists_at(mc, pLoc + dirs["-x"]);
        f_wall = wood_exists_at(mc, pLoc + dirs["-z"]);
        r_coord = dirs["-x"];
        f_coord = dirs["-z"];
    }

    else if (abs(rotation) == 270 || rotation == -90) {
        r_wall = wood_exists_at(mc, pLoc + dirs["+z"]);
        f_wall = wood_exists_at(mc, pLoc + dirs["-x"]);
        r_coord = dirs["+z"];
        f_coord = dirs["-x"];
    }

    bool free{false};

    if (!r_wall && !f_wall) {
        auto f_r = pLoc + f_coord + r_coord;
        auto f_l = pLoc + f_coord + (r_coord - r_coord - r_coord);

        if (!wood_exists_at(mc, f_r) && !wood_exists_at(mc, f_l)) {
            free = true;
        }
    }

    return std::make_tuple(r_wall, f_wall, r_coord, f_coord, free);
}

// RHS follower algo
void guide_player_out(mc_con &mc, bool testing) {
    bool solved{false};
    mcpp::Coordinate pLoc = mc.getPlayerPosition();
    mcpp::Coordinate carpetPos;  // Initialize carpet position

    std::vector<mcpp::Coordinate> solution;

    int rotation = 0;
    int step = 0;

    while (!solved) {
        step += 1;
        auto walls = get_forward_and_right(mc, rotation, pLoc);

        auto [r_wall, f_wall, right_coord, forward_coord, free] = walls;

        solved = free;

        if (!testing) {
            if (step > 1) {
                setBlock(mc, carpetPos, mcpp::Blocks::AIR);
            }

            // Place the new carpet at the same level as the player
            carpetPos = mcpp::Coordinate(pLoc.x, pLoc.y, pLoc.z);
            setBlock(mc, carpetPos, mcpp::Blocks::LIME_CARPET);

            // Delay for 1 second
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else {
            solution.push_back(pLoc);
        }

        if (!r_wall) {
            pLoc = pLoc + right_coord;
            rotation += 90;
        } else if (r_wall && !f_wall) {
            pLoc = pLoc + forward_coord;
        } else if (r_wall && f_wall) {
            rotation -= 90;
        }

        if (!testing) {
            // Remove the previous carpet
            setBlock(mc, carpetPos, mcpp::Blocks::AIR);
        }

        int x = pLoc.x;
        int y = pLoc.y;
        int z = pLoc.z;

        printf("Step[%i]: (%i, %i, %i)\n", step, x, y, z);
    };

    if (testing) {
        for (auto &location : solution) {
            // Place the carpet at the same y as the player in testing mode
            mcpp::Coordinate carpetPos(location.x, location.y, location.z);
            setBlock(mc, carpetPos, mcpp::Blocks::LIME_CARPET);
        }
    }

    std::cout << "Enter any key to continue:" << std::endl;
    std::string input = "";
    std::cin >> input;
}

// This is the code for H2, fast maze solve - calls out to Node Class
void create_shortest_path(mc_con &mc) {
    std::vector<Node> result = find_shortest_path(mc);
    for (Node currentPos : result) {
        setBlock(mc, currentPos.loc, mcpp::Blocks::LIME_CARPET);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Teleports the Player into the maze
void solve_manually(mc_con &mc, int mode, M_Pack maze_pack) {
    auto [bp, maze, _] = maze_pack;
    int val1;
    int val2;
    bool success = false;
    // teleports player randomly in recently generated maze
    if (mode != TESTING_MODE) {
        while (!success) {
            val1 = bp[0] + 1 + (rand() % (maze[0].size() - 2));
            val2 = bp[2] + 1 + (rand() % (maze.size() - 2));
            mcpp::Coordinate temp = mcpp::Coordinate(val1, bp[1], val2);
            if (mc.getBlock(temp) == mcpp::Blocks::AIR) {
                mc.setPlayerPosition(temp);
                success = true;
            }
        }
    }
    // teleports player to bottom right of generated maze
    else if (mode == TESTING_MODE) {
        val1 = bp[0] + maze[0].size() - 1;
        val2 = bp[2] + maze.size() - 1;
        while (!success) {
            mcpp::Coordinate temp = mcpp::Coordinate(val1, bp[1], val2);
            if (mc.getBlock(temp) == mcpp::Blocks::AIR) {
                mc.setPlayerPosition(temp);
                success = true;
            } else {
                val1--;
                val2--;
            }
        }
    }
}

// Handles the solve menu
void start_solve_maze(mc_con &mc, int mode, M_Pack maze_pack) {
    printSolveMazeMenu();

    std::string choice;

    while (choice != "3") {
        std::cin >> choice;
        if (choice == "1") {
            solve_manually(mc, mode, maze_pack);
            choice = "3";
        }

        else if (choice == "2") {
            // rt if not in testing mode, otherwise places blocks after
            if (mode == HD_MODE) {
                create_shortest_path(mc);
            } else {
                guide_player_out(mc, mode);
            }
            choice = "3";
        }

        else {
            std::cout << "Input Error: Enter a number between 1 and 3..."
                      << std::endl;
        }
    }
}


// This function fills in any parts missed by the H1 Hill builder below
void fill_in_sides_hill(mc_con &mc, M_Pack maze_pack) {
    auto [bp, maze, _] = maze_pack;

    for (unsigned i = 0; i < maze.size(); i++) {
        for (unsigned j = 0; j < maze[0].size(); j++) {
            if (maze[i][j] == 1) {
                int x = bp[0] + i;
                int z = bp[2] + j;
                int y = mc.getHeight(x, z);

                mcpp::Coordinate l(x, y, z);
                if (!wood_exists_at(mc, l)) {
                    for (int h = 0; h < 4; h++) {
                        mcpp::Coordinate c(x, y + h, z);
                        mc.setBlock(c, mcpp::Blocks::ACACIA_WOOD_PLANK);
                    }
                }
            }
        }
    }
}


// H1 build over hills.
// Can handle cliffs, trees, anything.
void build_hill_maze(mc_con &mc, M_Pack maze_pack) {
    auto [bp, maze, entrance] = maze_pack;

    // This constructs a map of heights -> coordinates,
    std::unordered_map<int, std::vector<mcpp::Coordinate>> h_map;

    for (unsigned x = 0; x < maze.size(); x++) {
        for (unsigned z = 0; z < maze[x].size(); z++) {
            if (maze[x][z] == 2) {
                int h = mc.getHeight(x + bp[0], z + bp[2]);
                auto got = h_map.find(h);

                mcpp::Coordinate c(x + bp[0], h, z + bp[2]);
                if (got == h_map.end()) {
                    std::vector<mcpp::Coordinate> coords = {c};
                    h_map.insert({h, coords});
                } else
                    h_map.at(h).push_back(c);
            }
        }
    }

    // This is a 2D lookup table to see if we have visited node before.
    // A 3D lookup table also works, but will create overlap in X / Z at
    // different Y values. Still generates a proper maze, just get crazy
    // DNA like spiral when building over nothing at all
    std::vector<std::vector<bool>> visited;
    visited.resize(maze.size(), std::vector<bool>(maze[0].size()));


    // Lambda Recursive Function
    std::function<void(mcpp::Coordinate &, const Maze_T &,
                       std::array<int, 3> bp)>
        descend = [&descend, &mc, &visited]
                (mcpp::Coordinate &c, const Maze_T &maze,
                                  std::array<int, 3> bp) -> void {

        mcpp::BlockType wood = mcpp::Blocks::ACACIA_WOOD_PLANK;

        if (maze[c.x - bp[0]][c.z - bp[2]] == 2) {
            // Check lookup table
            bool v = visited[c.x - bp[0]][c.z - bp[2]];
            if(!v) {
                visited[c.x - bp[0]][c.z - bp[2]] = true;

                setBlock(mc, c, mcpp::Blocks::STONE);

                const std::array<std::pair<int, int>, 4> directions = {
                    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

                for (const auto &[dx, dz] : directions) {
                    int new_x = c.x + dx;
                    int new_z = c.z + dz;

                    int i_x = new_x - bp[0];
                    int i_z = new_z - bp[2];
                    if (i_x >= 0 && i_x < (int)maze.size() && i_z >= 0 &&
                            i_z < (int)maze[0].size()) {
                        if (maze[i_x][i_z] == 2) {
                            int new_y = mc.getHeight(new_x, new_z);
                            if (c.y - new_y >= 2) {

                                mcpp::Coordinate p(new_x, c.y - 1, new_z);
                                setBlock(mc, p, mcpp::Blocks::STONE);

                                if(c.y - new_y != 0) new_y = c.y - 1;
                                else new_y = c.y;
                                mcpp::Coordinate n(new_x, new_y, new_z);
                                descend(n, maze, bp);
                            }
                        } else {
                            for (int i = 0; i < 4; i++) {
                                mcpp::Coordinate wall(new_x, c.y + i, new_z);
                                setBlock(mc, wall, wood);
                            }
                        }
                    }
                }
            }
        }
    };


    // We then iterate throught the map to make sure every height gets visited
    while (!h_map.empty()) {
        auto it = std::max_element(
            h_map.begin(), h_map.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });

        for (auto &c : it->second) {
            descend(c, maze, bp);
        }


        // Remove the highest element.
        h_map.erase(it);
    }

    fill_in_sides_hill(mc, maze_pack);

    std::cout << "Enter any key to continue" << std::endl;
    std::string input = "";
    std::cin >> input;
}

int main(int argc, char *argv[]) {
    int mode = NORMAL_MODE;
    if (argc > 1) {
        if (std::strcmp(argv[1], "--testing") == 0)
            mode = TESTING_MODE;
        else if (std::strcmp(argv[1], "--hd") == 0)
            mode = HD_MODE;
    }

    // !! haha no repro
    if (mode == NORMAL_MODE) {
        srand(time(NULL));
    }

    // Repro!!
    else if (mode == TESTING_MODE) {
        srand(1);
    }

    else if (mode == HD_MODE) {
        animateHD();
    }

    // Very reasonable
    M_Pack maze_pack;
    bool maze_pack_set = false;

    printStartText();

    mc_con mc;
    mc.doCommand("time set day");

    bool exit = false;

    // State machine for menu
    while (!exit) {
        int temp = system("clear");
        if (temp == -1) {
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        printMainMenu();

        std::string input;
        std::cin >> input;

        if (input == "1") {
            maze_pack = generate_maze_menu(mc, mode);
            maze_pack_set = true;

        } else if (input == "2") {
            // Spawning player to loc inputted by user before maze being built
            if (maze_pack_set) {
                auto [bp, maze, entrance] = maze_pack;
                mcpp::Coordinate playerLoc(bp[0], bp[1] + 10, bp[2]);
                mc.setPlayerPosition(playerLoc);

                if (mode == HD_MODE)
                    build_hill_maze(mc, maze_pack);
                else
                    build_maze(mc, maze_pack);
            } else {
                std::cout << "Please generate a maze first!!" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

        } else if (input == "3") {
            if (maze_pack_set) {
                start_solve_maze(mc, mode, maze_pack);
            } else {
                std::cout << "Please generate a maze first!!" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

        } else if (input == "4") {
            show_team_info();

        } else if (input == "5") {
            reset_changed_blocks(mc);
            exit = true;
        } else {
            int temp = system("clear");
            if (temp == -1) {
            }
            std::cout << "Input Error: Enter a number between 1 and 5..."
                      << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    printExitMassage();

    return EXIT_SUCCESS;
}

