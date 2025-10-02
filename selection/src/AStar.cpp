#include "AStar.h"
#include "Mesh.h"
#include <iostream>
#include <set>

void AStar::buildFromMesh(const Mesh& mesh) {
    clear();
    
    // Ajouter tous les sommets comme nœuds
    for (unsigned int i = 0; i < mesh.V.size(); i++) {
        addNode(i, mesh.V[i].p);
    }
    
    //set pour éviter les arêtes dupliquées
    std::set<std::pair<int, int>> edges;
    
    // Ajouter les arêtes basées sur les triangles
    for (unsigned int i = 0; i < mesh.T.size(); i++) {
        const MeshTriangle& triangle = mesh.T[i];
        
        // Pour chaque triangle, ajouter les 3 arêtes
        for (int j = 0; j < 3; j++) {
            int v1 = triangle.v[j];
            int v2 = triangle.v[(j + 1) % 3];
            
            // S'assurer que v1 < v2 pour éviter les doublons
            if (v1 > v2) {
                std::swap(v1, v2);
            }
            
            edges.insert(std::make_pair(v1, v2));
        }
    }
    
    // Ajouter toutes les arêtes uniques au graphe
    for (const auto& edge : edges) {
        addEdge(edge.first, edge.second);
    }
    
    std::cout << "Built A* graph from mesh with " << mesh.V.size() 
              << " vertices and " << edges.size() << " edges." << std::endl;
}