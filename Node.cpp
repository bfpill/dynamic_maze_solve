#include "Node.h"
#include <functional>
#include <algorithm>


std::size_t Node::Hasher::operator()(const Node& key) const  {
    std::size_t hx = std::hash<int>()(key.loc.x);
    std::size_t hy = std::hash<int>()(key.loc.y);
    std::size_t hz = std::hash<int>()(key.loc.z);
    return hx ^ (hy << 1) ^ (hz << 2);
};

bool Node::operator==(const Node& other) const {
    bool x_eq = loc.x == other.loc.x;
    bool y_eq = loc.y == other.loc.y;
    bool z_eq = loc.z == other.loc.z;
    return x_eq && y_eq && z_eq;
};

bool Node::operator!=(const mcpp::Coordinate& other) const {
    bool x_eq = loc.x != other.x;
    bool y_eq = loc.y != other.y;
    bool z_eq = loc.z != other.z;
    return x_eq || y_eq || z_eq;
};

int getCorners(mcpp::MinecraftConnection &mc, mcpp::Coordinate& loc) {
    int result = 0;
    mcpp::Coordinate temp;

    temp = mcpp::Coordinate(loc.x + 1, loc.y, loc.z + 1);
    if (!(mc.getBlock(temp) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        result++;
    }

    temp = mcpp::Coordinate(loc.x + 1, loc.y, loc.z - 1);
    if (!(mc.getBlock(temp) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        result++;
    }

    temp = mcpp::Coordinate(loc.x - 1, loc.y, loc.z + 1);
    if (!(mc.getBlock(temp) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        result++;
    }

    temp = mcpp::Coordinate(loc.x - 1, loc.y, loc.z - 1);
    if (!(mc.getBlock(temp) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        result++;
    }

    return result;
}


//finds neighbouring blocks and returns if block is air
std::vector<Node> getNeighbours(mcpp::MinecraftConnection &mc,
        mcpp::Coordinate& loc) {
    std::vector<Node> neighbours;
    Node temp;

    temp.loc = mcpp::Coordinate(loc.x + 1, loc.y, loc.z);
    if (!(mc.getBlock(temp.loc) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        neighbours.push_back(temp);
    }

    temp.loc = mcpp::Coordinate(loc.x - 1, loc.y, loc.z);
    if (!(mc.getBlock(temp.loc) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        neighbours.push_back(temp);
    }

    temp.loc = mcpp::Coordinate(loc.x, loc.y, loc.z + 1);
    if (!(mc.getBlock(temp.loc) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        neighbours.push_back(temp);
    }

    temp.loc = mcpp::Coordinate(loc.x, loc.y, loc.z - 1);
    if (!(mc.getBlock(temp.loc) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
        neighbours.push_back(temp);
    }

    return neighbours;
}


//creates the shortest path from the unordered map of nodes
std::vector<Node> backtrace(
        std::unordered_map<Node, Node, Node::Hasher>& parent,
        Node& root, Node& currentNode) {
    std::vector<Node> path;
    Node temp;
    path.push_back(currentNode);
    //appends the parent of the current node
    while (path.back() != root.loc) {
        temp.loc = mcpp::Coordinate(parent[path.back()].loc.x,
                parent[path.back()].loc.y, parent[path.back()].loc.z);
        path.push_back(temp);
    }

    //reverses the path so its from start to end
    reverse(path.begin(), path.end());

    return path;
}

//Uses BFS to find the closest exit and holds in a unordered map
std::vector<Node> find_shortest_path(mcpp::MinecraftConnection &mc) {
    Node root;
    root.loc = mc.getPlayerPosition();
    std::unordered_map<Node, Node, Node::Hasher> parent;
    bool solved = false;
    std::vector<Node> result;
    std::queue<Node> queue;
    queue.push(root);
    //keeps looping until the all neighbours are air blocks
    while (!solved) {
        Node currentNode = queue.front();
        queue.pop();


        currentNode.neighbours = getNeighbours(mc, currentNode.loc);
        for (Node neighbour : currentNode.neighbours) {
            if (parent.find(neighbour) == parent.end()) {
                parent[neighbour] = currentNode;
                queue.push(neighbour);
            }
        }
        //knows it solved since all neighbours are air blocks
        if (currentNode.neighbours.size() == 4
                && getCorners(mc, currentNode.loc) == 4) {
            result = backtrace(parent, root, currentNode);
            solved = true;
        }
    }
    return result;
}

