#include "Database.h"
#include "Graph.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
std::string findFile(const std::string& relativePath) {
    // Support launching from the project root or a build directory.
    for (const std::string& prefix : {"", "../", "../../", "../DSA-Project-2/"}) {
        const std::string path = prefix + relativePath;
        if (std::ifstream(path, std::ios::binary)) {
            return path;
        }
    }
    return relativePath;
}

bool containsArticle(const Graph& graph, int id) {
    if (graph.map.count(id) != 0) {
        return true;
    }
    // Articles with no outgoing links may appear only as destinations.
    for (const auto& entry : graph.map) {
        if (entry.second.count(id) != 0) {
            return true;
        }
    }
    return false;
}

void printPath(const std::vector<int>& path, Database& names) {
    if (path.empty()) {
        std::cout << "No path found in the loaded dataset.\n";
        return;
    }

    std::cout << "Shortest path (" << path.size() - 1 << " links):\n";
    for (std::size_t i = 0; i < path.size(); ++i) {
        const std::string title = names[path[i]];
        std::cout << (title.empty() ? "Article ID " + std::to_string(path[i]) : title);
        std::cout << (i + 1 == path.size() ? "\n" : " -> ");
    }
}
} // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: " << argv[0] << " [graph.bin names.db]\n"
                  << "Defaults: dataset/graphdata_small.bin and database/names.db\n";
        return 0;
    }
    if (argc != 1 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [graph.bin names.db]\n";
        return 1;
    }

    const std::string graphPath = argc == 3 ? argv[1] : findFile("dataset/graphdata_small.bin");
    const std::string databasePath = argc == 3 ? argv[2] : findFile("database/names.db");

    // The binary dataset contains consecutive pairs of integer article IDs.
    std::ifstream graphFile(graphPath, std::ios::binary | std::ios::ate);
    if (!graphFile) {
        std::cerr << "Cannot open graph file: " << graphPath << '\n';
        return 1;
    }
    const std::streamoff bytes = graphFile.tellg();
    const std::streamoff rowBytes = 2 * sizeof(int);
    if (bytes <= 0 || bytes % rowBytes != 0) {
        std::cerr << "Invalid or unsupported graph size: " << graphPath << '\n';
        return 1;
    }
    const int rows = static_cast<int>(bytes / rowBytes);
    graphFile.close();

    if (!std::ifstream(databasePath, std::ios::binary)) {
        std::cerr << "Cannot open names database: " << databasePath << '\n';
        return 1;
    }
    Database names(databasePath, "SELECT name FROM names WHERE id = ? LIMIT 1");
    if (!names.changeQuery("SELECT id FROM names WHERE name = ? COLLATE NOCASE LIMIT 1")) {
        std::cerr << "\nThe names database must contain a names table with id and name columns.\n";
        return 1;
    }

    std::cout << "Loading graph from " << graphPath << "...\n";
    Graph graph(graphPath, rows, 2);
    if (graph.map.empty()) {
        std::cerr << "The graph could not be loaded.\n";
        return 1;
    }
    std::cout << "Loaded " << rows << " links.\n"
              << "Enter article titles (case-insensitive). Enter a blank title to quit.\n";

    while (true) {
        std::string startTitle;
        std::string targetTitle;
        std::cout << "\nStarting article: ";
        if (!std::getline(std::cin, startTitle) || startTitle.empty()) {
            break;
        }
        std::cout << "Destination article: ";
        if (!std::getline(std::cin, targetTitle) || targetTitle.empty()) {
            break;
        }

        const int start = names[startTitle];
        const int target = names[targetTitle];
        if (start == -1 || target == -1) {
            if (start == -1) {
                std::cout << "Article not found: " << startTitle << '\n';
            }
            if (target == -1) {
                std::cout << "Article not found: " << targetTitle << '\n';
            }
            continue;
        }
        if (!containsArticle(graph, start) || !containsArticle(graph, target)) {
            std::cout << "One or both articles are outside the loaded graph. "
                      << "The smaller dataset contains only part of Wikipedia.\n";
            continue;
        }

        std::cout << "\nBFS\n";
        printPath(graph.BFS(start, target), names);
        std::cout << "\nDijkstra\n";
        printPath(graph.dij(start, target), names);
    }

    return 0;
}
