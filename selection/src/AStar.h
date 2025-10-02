#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <functional>
#include "Vec3.h"

// Structure pour représenter un nœud dans l'algorithme A*
struct AStarNode {
    int id;                 // ID du sommet
    Vec3 position;          // Position 3D du sommet
    float gCost;            // Coût depuis le début
    float hCost;            // Heuristique vers la fin
    float fCost;            // Coût total (g + h)
    int parent;             // ID du parent (-1 si aucun)
    
    AStarNode() : id(-1), gCost(0), hCost(0), fCost(0), parent(-1) {}
    
    AStarNode(int _id, const Vec3& _pos) 
        : id(_id), position(_pos), gCost(0), hCost(0), fCost(0), parent(-1) {}
    
    // Comparateur pour la priority_queue (min-heap)
    bool operator>(const AStarNode& other) const {
        return fCost > other.fCost;
    }
};

// Classe pour l'algorithme A*
class AStar {
private:
    // Graph représenté par des listes d'adjacence
    std::unordered_map<int, std::vector<int>> adjacencyList;
    std::unordered_map<int, Vec3> nodePositions;
    
    // Fonction heuristique (distance euclidienne)
    float heuristic(const Vec3& a, const Vec3& b) const {
        return (a - b).length();
    }
    
    // Calcule la distance entre deux nœuds adjacents
    float getDistance(int nodeA, int nodeB) const {
        auto itA = nodePositions.find(nodeA);
        auto itB = nodePositions.find(nodeB);
        if (itA == nodePositions.end() || itB == nodePositions.end()) {
            return std::numeric_limits<float>::max();
        }
        return (itA->second - itB->second).length();
    }
    
public:
    AStar() {}
    
    // Ajoute un nœud au graphe
    void addNode(int id, const Vec3& position) {
        nodePositions[id] = position;
        if (adjacencyList.find(id) == adjacencyList.end()) {
            adjacencyList[id] = std::vector<int>();
        }
    }
    
    // Ajoute une arête bidirectionnelle entre deux nœuds
    void addEdge(int nodeA, int nodeB) {
        adjacencyList[nodeA].push_back(nodeB);
        adjacencyList[nodeB].push_back(nodeA);
    }
    
    // Construit le graphe à partir d'un mesh (utilise les arêtes des triangles)
    void buildFromMesh(const class Mesh& mesh);
    
    // Algorithme A*
    std::vector<int> findPath(int startId, int goalId) {
        std::vector<int> path;
        
        // Vérifier que les nœuds existent
        if (nodePositions.find(startId) == nodePositions.end() || 
            nodePositions.find(goalId) == nodePositions.end()) {
            return path; // Chemin vide si nœuds invalides
        }
        
        // Priority queue pour les nœuds à explorer (min-heap basé sur fCost)
        std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
        
        // Sets pour tracker les nœuds explorés et à explorer
        std::unordered_set<int> openSetIds;
        std::unordered_set<int> closedSet;
        
        // Map pour stocker les meilleurs coûts g pour chaque nœud
        std::unordered_map<int, float> gCosts;
        std::unordered_map<int, int> parents;
        
        // Initialiser le nœud de départ
        AStarNode startNode(startId, nodePositions[startId]);
        startNode.gCost = 0;
        startNode.hCost = heuristic(nodePositions[startId], nodePositions[goalId]);
        startNode.fCost = startNode.gCost + startNode.hCost;
        
        openSet.push(startNode);
        openSetIds.insert(startId);
        gCosts[startId] = 0;
        
        while (!openSet.empty()) {
            // Prendre le nœud avec le plus petit fCost
            AStarNode current = openSet.top();
            openSet.pop();
            openSetIds.erase(current.id);
            
            // Si on a atteint le but
            if (current.id == goalId) {
                // Reconstruire le chemin
                int currentId = goalId;
                while (currentId != -1) {
                    path.push_back(currentId);
                    auto it = parents.find(currentId);
                    currentId = (it != parents.end()) ? it->second : -1;
                }
                std::reverse(path.begin(), path.end());
                return path;
            }
            
            closedSet.insert(current.id);
            
            // Explorer les voisins
            auto neighbors = adjacencyList.find(current.id);
            if (neighbors != adjacencyList.end()) {
                for (int neighborId : neighbors->second) {
                    // Ignorer si déjà exploré
                    if (closedSet.find(neighborId) != closedSet.end()) {
                        continue;
                    }
                    
                    float tentativeGCost = current.gCost + getDistance(current.id, neighborId);
                    
                    // Si ce chemin vers le voisin est meilleur
                    if (gCosts.find(neighborId) == gCosts.end() || 
                        tentativeGCost < gCosts[neighborId]) {
                        
                        gCosts[neighborId] = tentativeGCost;
                        parents[neighborId] = current.id;
                        
                        AStarNode neighbor(neighborId, nodePositions[neighborId]);
                        neighbor.gCost = tentativeGCost;
                        neighbor.hCost = heuristic(nodePositions[neighborId], nodePositions[goalId]);
                        neighbor.fCost = neighbor.gCost + neighbor.hCost;
                        neighbor.parent = current.id;
                        
                        // Ajouter à openSet si pas déjà présent
                        if (openSetIds.find(neighborId) == openSetIds.end()) {
                            openSet.push(neighbor);
                            openSetIds.insert(neighborId);
                        }
                    }
                }
            }
        }
        
        return path; // Chemin vide si aucun chemin trouvé
    }
    
    // Obtient les voisins d'un nœud
    std::vector<int> getNeighbors(int nodeId) const {
        auto it = adjacencyList.find(nodeId);
        if (it != adjacencyList.end()) {
            return it->second;
        }
        return std::vector<int>();
    }
    
    // Calcule la distance géodésique entre deux points
    float getGeodesicDistance(int startId, int goalId) const {
        // Vérifier que les nœuds existent
        if (nodePositions.find(startId) == nodePositions.end() || 
            nodePositions.find(goalId) == nodePositions.end()) {
            return std::numeric_limits<float>::max();
        }
        
        // Si c'est le même point, distance = 0
        if (startId == goalId) {
            return 0.0f;
        }
        
        // Utiliser Dijkstra pour trouver la plus courte distance
        std::unordered_map<int, float> distances;
        std::unordered_set<int> visited;
        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
        
        // Initialiser les distances
        for (const auto& pair : nodePositions) {
            distances[pair.first] = std::numeric_limits<float>::max();
        }
        distances[startId] = 0.0f;
        pq.push({0.0f, startId});
        
        while (!pq.empty()) {
            float dist = pq.top().first;
            int u = pq.top().second;
            pq.pop();
            
            // Si on a trouvé la destination, retourner la distance
            if (u == goalId) {
                return dist;
            }
            
            if (visited.find(u) != visited.end()) continue;
            visited.insert(u);
            
            // Explorer les voisins
            auto neighbors = adjacencyList.find(u);
            if (neighbors != adjacencyList.end()) {
                for (int v : neighbors->second) {
                    if (visited.find(v) == visited.end()) {
                        float edgeWeight = getDistance(u, v);
                        float newDist = dist + edgeWeight;
                        
                        if (newDist < distances[v]) {
                            distances[v] = newDist;
                            pq.push({newDist, v});
                        }
                    }
                }
            }
        }
        
        // Si aucun chemin trouvé
        return std::numeric_limits<float>::max();
    }
    
    // Affiche les statistiques du graphe
    void printStats() const {
        std::cout << "Graph stats:" << std::endl;
        std::cout << "- Nodes: " << nodePositions.size() << std::endl;
        int totalEdges = 0;
        for (const auto& pair : adjacencyList) {
            totalEdges += pair.second.size();
        }
        std::cout << "- Edges: " << totalEdges / 2 << std::endl; // Division par 2 car bidirectionnel
    }
    
    // Vide le graphe
    void clear() {
        adjacencyList.clear();
        nodePositions.clear();
    }
};

#endif // ASTAR_H
