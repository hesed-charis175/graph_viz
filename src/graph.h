#include <algorithm>
#include <vector>
#include <iostream>
#include "lm.h"
#include <string>
#include "config.h"


enum GraphType { City, Island, Maze };
enum class NodeType{ None, Start, Intermediate, End };


struct Node{
    Vec2 position;
    bool isIntermediate = false;
    bool isStart = false;
    bool isEnd = false;
    std::vector<Node*> neighbors;
    std::string strNodeType;

    Node() : position(0, 0) {}
    Node(float x, float y): position(x, y) {}
    NodeType getNodeType() const{
        if(isEnd) { return NodeType::End; }
        if(isIntermediate) { return NodeType::Intermediate; }
        if(isStart) { return NodeType::Start; }
        return NodeType::None;
    }

    void print(){
        std::cout << "Node [" << strNodeType << "] (" << position.x << ", " << position.y << ")" << std::endl;
    }

    void setNodeType(NodeType type){
        if(type == NodeType::Start) {  strNodeType = "START"; isStart = true; isIntermediate = false; isEnd = false; }
        if(type == NodeType::Intermediate) {  strNodeType = "INTERMEDIATE"; isStart = false; isIntermediate = true; isEnd = false; }
        if(type == NodeType::End) {  strNodeType = "END"; isStart = false; isIntermediate = false; isEnd = true; }
        if(type == NodeType::None) {  strNodeType = "DEFAULT"; isStart = false; isIntermediate = false; isEnd = false; }
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
            if(start) setNodeType(start, NodeType::None);
            start = node;
        }

        else if(type == NodeType::End){
            if(end) setNodeType(end, NodeType::Intermediate);
            end = node;
        }

        else if(type == NodeType::Intermediate){
            intermediateNodes.push_back(node);
        }

        else{
            intermediateNodes.erase(std::remove(
                intermediateNodes.begin(), intermediateNodes.end(), node
            ), intermediateNodes.end());
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
    }

    void resetNodeRoles(){
        for(auto& node: nodes){
            setNodeType(&node, NodeType::None);
        }
    }

    Node* getNodeAt(Vec2 _position){
        auto _node = std::find_if(nodes.begin(), nodes.end(), [_position](const Node& n){
            return (n.position - _position).length() <= Config::nodeSize;
        });
        return (_node != nodes.end()) ? &(*_node) : nullptr;
    }
};