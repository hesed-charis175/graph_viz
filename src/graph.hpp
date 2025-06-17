#include <algorithm>
#include <vector>
#include <iostream>
#include "matvec.hpp"
#include <string>


enum GraphType { City, Island, Maze };
enum class NodeType{ None, Start, Intermediate, End };


struct Node{
    Vec2 position;
    bool isIntermediate = false;
    bool isStart = false;
    bool isEnd = false;
    std::vector<Node*> neighbors;
    std::string debugNodeTypeString;

    Node() : position(0, 0) {}
    Node(float x, float y): position(x, y) {}
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
        if(type == NodeType::Start) {  debugNodeTypeString = "START"; isStart = true; isIntermediate = false; isEnd = false; }
        if(type == NodeType::Intermediate) {  debugNodeTypeString = "INTERMEDIATE"; isStart = false; isIntermediate = true; isEnd = false; }
        if(type == NodeType::End) {  debugNodeTypeString = "END"; isStart = false; isIntermediate = false; isEnd = true; }
        if(type == NodeType::None) {  debugNodeTypeString = "DEFAULT"; isStart = false; isIntermediate = false; isEnd = false; }
    }
    ~Node(){
        neighbors.clear();
    }
};



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

        if(type == NodeType::Start){
            if(start && start != node) setNodeType(start, NodeType::None);
            start = node;
            if (node == end) end = nullptr;
        }

        else if(type == NodeType::End){
            if(end && end != node) setNodeType(end, NodeType::None);
            end = node;
            if (node == start) start = nullptr;
        }

        else if(type == NodeType::Intermediate){
            if(std::find(intermediateNodes.begin(), intermediateNodes.end(), node) == intermediateNodes.end()) intermediateNodes.push_back(node);
        }

        else{
            intermediateNodes.erase(std::remove(
                intermediateNodes.begin(), intermediateNodes.end(), node
            ), intermediateNodes.end());
            if (node == start) start = nullptr;
            if (node == end) end = nullptr; 
        }

        node->setNodeType(type);
    }
    void addNode(const Node& copyNode){
        nodes.emplace_back(copyNode);
        setNodeType(&nodes.back(), copyNode.getNodeType());
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
            if (&node == start) {
                if (!node.isStart) node.setNodeType(NodeType::Start);
            } else if (node.isStart) {
                node.setNodeType(NodeType::None);
            }
        }
        for (auto& node : nodes) {
            if (&node == end) {
                if (!node.isEnd) node.setNodeType(NodeType::End);
            } else if (node.isEnd) {
                node.setNodeType(NodeType::None);
            }
        }
        for (auto& node : nodes) {
            bool isIntermediate = std::find(intermediateNodes.begin(), intermediateNodes.end(), &node) != intermediateNodes.end();
            if (isIntermediate) {
                if (!node.isIntermediate) node.setNodeType(NodeType::Intermediate);
            } else if (node.isIntermediate) {
                node.setNodeType(NodeType::None);
            }
        }
    }
};
