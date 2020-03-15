#include "global.h"

using namespace std;

// The main method
int main(int argc, char const *argv[]){   
    Graph graph("Tests/coxeter.txt");
    graph.print();
    SparseMatrix h = createHyperlinkMatrix(graph);
    h.print();
    return 0;
}
