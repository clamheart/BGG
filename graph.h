#ifndef GRAPH_H_
#define GRAPH_H_
#include <list>
#include "identity.h"
#include "node.h"

//The Graph represents a graph, although not exactly. A Graph holds a list of nodes, the vertex set, but no edge set.
//This is because all information about edges between nodes is contained in the four lists within each node.
//A Graph object contains its exponent, a list of nodes, and a list of Identity objects.

class Graph
{
private:
    int exponent;   //The exponent of the group; any element raised to this power is the identity.

    std::list<Node> nodeList;  //Stores all nodes in the graph.

    std::list<Identity> identityBank;  //Stores identities after generation.

public:
    //Constructors
    Graph();
    Graph(int exp);


    //Get functions
    std::list<Node>::iterator getNodeListBegin()
        { return nodeList.begin(); }
    std::list<Node>::iterator getNodeListEnd()
        { return nodeList.end(); }
    std::list<Identity>::iterator getIdentBegin()
        { return identityBank.begin(); }
    std::list<Identity>::iterator getIdentEnd()
        { return identityBank.end(); }


    //Erase an identity from identityBank
    void eraseIdent(std::list<Identity>::iterator pid)
        { identityBank.erase(pid); }


    //Report; this prints the preserved nodes and edges currently in the graph.
    void report(std::ostream& fout = std::cout);


    //Initial Step; this creates the first identities which map the y-branch into the x-branch and create the
    //first section of the graph, as well as declare open and closed nodes and remove waste nodes.
    void initStep();


    //isFinished; checks if graph is finished
    bool isFinished();


    //These functions dajust the lists in the argument nodes to reflect that the first node multiplied by x
    //is the second node, or similarly for y.
    bool setXedge(std::list<Node>::iterator pn1, std::list<Node>::iterator pn2);
    bool setYedge(std::list<Node>::iterator pn1, std::list<Node>::iterator pn2);


    //Search nodeList for a node with passed in elemName.
    std::list<Node>::iterator searchNodes(std::vector<short>* pvs);


    //These functions manage nodeList.
    //removeNode; removes all ties to the passed-in node and then deletes it
    void removeNode(std::list<Node>::iterator pn);
    //setClosedNodes; finds closed nodes and sets their type
    void setClosedNodes();
    //sortTempNodes; assigns open and closed status to temporary nodes
    void sortTempNodes();


    //These functions determine the x or y product of the given node and find or create that node in the graph.
    //buildXprod; finds or constructs and links the node to its x product; returns the product
    std::list<Node>::iterator buildXprod(std::list<Node>::iterator pn);
    //buildYprod; finds or constructs and links the node to its y product; returns the product
    std::list<Node>::iterator buildYprod(std::list<Node>::iterator pn);


    //Builds a path from the base node to the target node. The function presumes that the base word
    //is entirely contained in the leftmost part of the target word; if not it returns false
    bool buildPath(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode);
    //Destroys the path from the base node to the target node, including target but NOT base;
    //function makes same assumptions as buildPath
    bool destroyPath(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode);


    //These functions look for x- or y-cycles in the graph and closes them if found.
    bool findXcycle();
    bool findYcycle();


    //Generates identities using a given base node and stores them in identityBank.
    void generateIdentities(std::list<Node>::iterator baseNode, bool useSearchPath = false, int iterationCounter = 0);
    //Erase duplicate identities and associated nodes
    void clearDuplicateIdentities();


    ///The need for this function has been erased by fixing setXedge() and setYedge() to set only new edges
    //Fix a problem where some edges in initStep() are created twice
    void clearDuplicateEdges();


    //Creates new product nodes without attaempting to find them in the graph.
    std::list<Node>::iterator buildXprodTemp(std::list<Node>::iterator pn);
    std::list<Node>::iterator buildYprodTemp(std::list<Node>::iterator pn);


    //Reduce identity and return reduced identity.
    Identity reduceIdent(std::list<Identity>::iterator pid);


    //Sets a path of nodes to type OPEN in order that they be preserved from deletion.
    void preservePath(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode);


    //Delete all temp nodes.
    void deleteTempNodes();


    //Copies the edges from the first node and connects them to the second node.
    void copyNodeEdges(std::list<Node>::iterator pn1, std::list<Node>::iterator pn2);


    //Clear duplicate nodes based on multiple entries in one of the given node's edge lists;
    //return true if duplicate found, false otehrwise.
    bool clearDuplicateNodes(std::list<Node>::iterator pn);


    //Similar to clearDuplicateNodes(); instead of deleting nodes it inserts them into the
    //given list.
    void insertDuplicateNodes(std::list<Node>::iterator pn, std::list< std::list<Node>::iterator >& deleteNodes);


    //Return number of nodes in graph
    int getSize()
        { return nodeList.size(); }


    //Find a node by tracing the path from 1 to the passed in node; return pnull if path ends prematurely
    std::list<Node>::iterator searchPath(std::vector<short>* ptargetWord);


    //Alt. version of buildPath() that returns the path built.
    std::list< std::list<Node>::iterator >
        buildPath2(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode);


    //Alt. version of reduceIdent() that uses passed in path to reduce identity.
    Identity reduceIdent2(std::list<Identity>::iterator pid, std::list< std::list<Node>::iterator >& pidPath, bool report = false);


    //Alt. version of preservePath() that uses a passed in path list.
    void preservePath2(std::list< std::list<Node>::iterator >& pidPath, std::list<Node>::iterator pendNode);


    //Generates second type of identities.
    void generateIdentities2(int length, int numIdents);
};

#endif
