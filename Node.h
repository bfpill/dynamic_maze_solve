#ifndef NODE_H
#define NODE_H

#include <vector>
#include <cstddef>
#include <mcpp/mcpp.h>
#include <queue>
#include <unordered_map>

class Node {
public:
    mcpp::Coordinate loc;
    std::vector<Node> neighbours;

    struct Hasher {
        std::size_t operator()(const Node& key) const;
    };

    bool operator==(const Node& other) const;
    bool operator!=(const mcpp::Coordinate& other) const;
};

int getCorners(mcpp::MinecraftConnection &mc, mcpp::Coordinate& loc);
std::vector<Node> getNeighbours(mcpp::MinecraftConnection &mc, mcpp::Coordinate& loc);
std::vector<Node> backtrace(std::unordered_map<Node, Node, Node::Hasher>& parent, Node& root, Node& currentNode);
std::vector<Node> find_shortest_path(mcpp::MinecraftConnection &mc);

#endif // NODE_H

