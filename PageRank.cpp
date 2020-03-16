#include "global.h"

using namespace std;

// The main method
int main(int argc, char const *argv[]){   
    Graph graph("Tests/diamond.txt");
    graph.print();

    Column pageRanks = PageRank::calculatePageRank(graph);
    PageRank::printPageRank(pageRanks);
    return 0;
}
