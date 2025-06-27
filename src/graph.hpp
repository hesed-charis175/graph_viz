#include <algorithm>
#include <vector>
#include <iostream>
#include "matvec.hpp"
#include <string>
#include <cassert>
#include <cstdint>


static std::string s_toLog;


enum GraphType { City, Island, Maze };
enum class NodeType{ None, Start, Intermediate, End };


struct Node{
    static size_t global_id_counter;
    size_t node_id;
    Vec2 position;
    bool isIntermediate;
    bool isStart;
    bool isEnd;
    std::vector<Node*> neighbors;
    std::string debugNodeTypeString;

    Node() : 
        node_id(global_id_counter++),
        position(0, 0), 
        isIntermediate(false),
        isStart(false),
        isEnd(false){}
    Node(float x, float y): 
        node_id(global_id_counter++),
        position(x, y), 
        isIntermediate(false),
        isStart(false),
        isEnd(false){}
    Node(const Node& other) : 
        node_id(global_id_counter++),
        position(other.position), 
        isIntermediate(other.isIntermediate),
        isStart(other.isStart), 
        isEnd(other.isEnd), 
        neighbors(),
        debugNodeTypeString(other.debugNodeTypeString){}
    NodeType getNodeType() const{
        if(isEnd) { return NodeType::End; }
        if(isIntermediate) { return NodeType::Intermediate; }
        if(isStart) { return NodeType::Start; }
        return NodeType::None;
    }

    void print(){
        std::cout << "Node [" << debugNodeTypeString << "] (" << position.x << ", " << position.y << ")" << std::endl;
    }

    void setNodeType(NodeType type){
        if(type == NodeType::Start) { debugNodeTypeString = "START"; isStart = true; isIntermediate = false; isEnd = false; }
        else if(type == NodeType::Intermediate) { debugNodeTypeString = "INTERMEDIATE"; isStart = false; isIntermediate = true; isEnd = false; }
        else if(type == NodeType::End) { debugNodeTypeString = "END"; isStart = false; isIntermediate = false; isEnd = true; }
        else if(type == NodeType::None) { debugNodeTypeString = "DEFAULT"; isStart = false; isIntermediate = false; isEnd = false; }
    }
    ~Node(){
        neighbors.clear();
    }
};

size_t Node::global_id_counter = 0;

struct Graph{
 public:
    
    std::vector<Node> nodes;
    std::vector<std::pair<Node*, Node*>> edges;
    Node* start;
    Node* end;
    std::vector<Node*> intermediateNodes;

    Graph(){}
    ~Graph(){
        clearGraph();
    }
    void clearGraph(){
        intermediateNodes.clear();
        nodes.clear();
        edges.clear();
        start = nullptr;
        end = nullptr;
        
    }

    void setNodeType(Node* node, NodeType type){

        if(!node) return;

        for(auto& n : nodes){
            n.setNodeType(NodeType::None);
        }

        switch(type){
            case NodeType::Start: 
                if(node == end) end = nullptr;
                start = node;
            break;
            case NodeType::End:
                if(node == start) start = nullptr;
                end = node;
            break;
            case NodeType::Intermediate:
                if(node == start) start = nullptr;
                if(node == end) end = nullptr;
            break;
            case NodeType::None:
                if(node == start) start = nullptr;
                if(node == end) end = nullptr;
            break;
        }

        if (type == NodeType::Intermediate && node != start && node != end) {
            auto it = std::find(intermediateNodes.begin(), intermediateNodes.end(), node);
            if (it == intermediateNodes.end()) {
                intermediateNodes.push_back(node);
            }
        }

        if(type != NodeType::Intermediate){
            auto it = std::find(intermediateNodes.begin(), intermediateNodes.end(), node);
            if (it != intermediateNodes.end()) {
                intermediateNodes.erase(it);
            }
        }
        std::vector<Node*> updatedIntermediateNodes;

        for(auto& iNode: intermediateNodes){
            if(!iNode) continue;

            Node* updated = getNodeAt(iNode->position, 5.0f, 5.0f);
            if(updated){
                updated->setNodeType(NodeType::Intermediate);
                updatedIntermediateNodes.push_back(updated);
            }
        }

        intermediateNodes = std::move(updatedIntermediateNodes);

        if(start) {
            start = getNodeAt(start->position, 1, 1);
            start->setNodeType(NodeType::Start);
        }
        if(end) {
            end = getNodeAt(end->position, 1, 1);
            end->setNodeType(NodeType::End);
        }

    }
    
    
void addNode(const Node& copyNode){
    nodes.emplace_back(copyNode);
    s_toLog += "[DEBUG] addNode: Added node " + std::to_string(reinterpret_cast<uintptr_t>(&nodes.back())) 
           + " [id=" + std::to_string(nodes.back().node_id) + "] from copyNode " + std::to_string(reinterpret_cast<uintptr_t>(&copyNode)) + " [id=" + std::to_string(copyNode.node_id) + "] (type: " 
           + copyNode.debugNodeTypeString + ")\n";
    setNodeType(&nodes.back(), copyNode.getNodeType());
    debugPrintState();
}
    void addNode(float x, float y, NodeType type = NodeType::None){
        nodes.emplace_back(x, y);
        setNodeType(&nodes.back(), type);
    }
    void addEdge(Node* node_A, Node* node_B){

        if(!node_A || !node_B) return;

        edges.emplace_back(node_A, node_B);
        node_A->neighbors.push_back(node_B);
        node_B->neighbors.push_back(node_A);
    }

    void removeNode(Node* nodeToRemove){

        if(!nodeToRemove) return;

        for(auto neighbor: nodeToRemove->neighbors){
            neighbor->neighbors.erase(
                std::remove(neighbor->neighbors.begin(), neighbor->neighbors.end(), nodeToRemove), neighbor->neighbors.end()
            );
        }

        edges.erase(
            std::remove_if(edges.begin(), edges.end(), [nodeToRemove](const std::pair<Node*, Node*> edge){
                return edge.first == nodeToRemove || edge.second == nodeToRemove;
            }), edges.end()
        );

        auto _nodeIt = std::remove_if(nodes.begin(), nodes.end(), [nodeToRemove](const Node& n_arg){
            return &n_arg == nodeToRemove;
        });
        if(_nodeIt != nodes.end()){
            nodes.erase(_nodeIt, nodes.end());
        }

        if(nodeToRemove == start) start = nullptr;
        if(nodeToRemove == end) end = nullptr;
        intermediateNodes.erase(std::remove(intermediateNodes.begin(), intermediateNodes.end(), nodeToRemove), intermediateNodes.end());
    }

    void resetNodeRoles(){
        for(auto& node: nodes){
            setNodeType(&node, NodeType::None);
        }
    }

    Node* getNodeAt(Vec2 _position, float selectionRadius, float nodeSize){
        auto _node = std::find_if(nodes.begin(), nodes.end(), [_position, selectionRadius, nodeSize](const Node& n){
            return (n.position - _position).length() <= nodeSize + selectionRadius;
        });
        return (_node != nodes.end()) ? &(*_node) : nullptr;
    }

    void enforceNodeTypeConsistency() {
        for (auto& node : nodes) {
            bool isIntermediate = std::find(intermediateNodes.begin(), intermediateNodes.end(), &node) != intermediateNodes.end();
            if (&node == start) {
                if (!node.isStart) node.setNodeType(NodeType::None);
                else if (node.isStart && &node != end && std::find(intermediateNodes.begin(), intermediateNodes.end(), &node) == intermediateNodes.end()) {
                    node.setNodeType(NodeType::Start);
                }
            }
            else if (&node == end) {
                if (!node.isEnd) node.setNodeType(NodeType::None);
                else if (node.isEnd && &node != start && std::find(intermediateNodes.begin(), intermediateNodes.end(), &node) == intermediateNodes.end()) {
                    node.setNodeType(NodeType::End);
                }
            }
            else if (isIntermediate) {
                if (!node.isIntermediate) node.setNodeType(NodeType::Intermediate);
                else if (node.isIntermediate && &node != start && &node != end) {
                    node.setNodeType(NodeType::None);
                }
            }
            else{
                node.setNodeType(NodeType::None);
            }
        }
    }
    bool isNodePtrValid(Node* ptr) const {
    for (const auto& node : nodes) {
        if (&node == ptr) return true;
    }
    return false;
}

void debugPrintState() const {
    s_toLog += "----- GRAPH STATE -----\n";
s_toLog += "Start: " + std::to_string(reinterpret_cast<uintptr_t>(start)) +
           " End: " + std::to_string(reinterpret_cast<uintptr_t>(end)) + "\n";

for (const auto& node : nodes) {
    s_toLog += "Node " + std::to_string(reinterpret_cast<uintptr_t>(&node)) +
               " [id=" + std::to_string(node.node_id) + "] type: " + node.debugNodeTypeString;
    
    if (&node == start) s_toLog += " <-- START";
    if (&node == end) s_toLog += " <-- END";
    
    for (auto* intnode : intermediateNodes) {
        if (&node == intnode) s_toLog += " <-- INTERMEDIATE";
    }
    
    s_toLog += "\n";
}

s_toLog += "Intermediate nodes: ";
for (const auto& node : intermediateNodes) {
    if (node)
        s_toLog += std::to_string(reinterpret_cast<uintptr_t>(node)) + "[id=" +
                   std::to_string(node->node_id) + "] ";
}
s_toLog += "\n-----------------------\n";

}
};
