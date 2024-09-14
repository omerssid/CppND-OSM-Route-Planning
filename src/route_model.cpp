#include "route_model.h"
#include <iostream>
#include <memory>

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    // Create RouteModel nodes.
    int counter = 0;
    for (Model::Node node : this->Nodes())
    {
        // auto m_node = std::make_unique<RouteModel::Model>(counter, this, node);
        m_Nodes.emplace_back(counter, this, node);
        counter++;
    }
    CreateNodeToRoadHashmap();
}

RouteModel::Node* RouteModel::Node::FindNeighbor(std::vector<int> node_indices)
{
    Node *closest_node = nullptr;
    for (auto node_idx : node_indices)
    {
        auto &node = this->parent_model->SNodes()[node_idx];
        if(this->distance(node) > 0 && !node.visited)
        {
            if (closest_node == nullptr || this->distance(node) < closest_node->distance(node))
            {
                closest_node = &node;
            }
        }
    }
    return closest_node;
}

void RouteModel::CreateNodeToRoadHashmap()
{
    for (const auto &road : this->Roads())
    {
        if (road.type != Model::Road::Type::Footway)
        {
            for(int node_idx : Ways()[road.way].nodes)
            {
                auto it = this->node_to_road.find(node_idx);
                if (it == this->node_to_road.end())
                {
                    // the node index is not in the node_to_road
                    node_to_road[node_idx] = std::vector<const Model::Road *>();
                }
                node_to_road[node_idx].push_back(&road);
            }
        }
    }
}

void RouteModel::Node::FindNeighbors()
{
    for (const auto &road : parent_model->node_to_road[this->index])
    {
        RouteModel::Node *closest_neighbor = FindNeighbor(parent_model->Ways()[road->way].nodes);
        if( closest_neighbor != nullptr)
            this->neighbors.push_back(closest_neighbor)
    }
}

RouteModel::Node& RouteModel::FindClosestNode(float x, float y)
{
    Node t_node;
    t_node.x = x;
    t_node.y = y;
    float min_dist = std::numeric_limits<float>::max();
    int closest_idx;

    for (const auto &road : this->Roads())
    {
        if (road.type != Model::Road::Type::Footway)
        {
            for (auto node_idx : Ways()[road.way].nodes)
            {
                auto t_distance = t_node.distance(SNodes().at(node_idx));
                if (t_distance < min_dist)
                {
                    min_dist = t_distance;
                    closest_idx = node_idx;
                }
            }
        }
    }
    return SNodes().at(closest_idx);
}