//***********************
//***********************
//   Notes for use:
//
//   First, you can adjust the exponent of the graph by setting EXPONENT
// at the top of main(). Setting it to 2 will probably cause a crash. Setting it above 4
// will produce a very large graph.
//
//   Second, you can adjust the number of times the main loop will run by setting MAX_ITERATIONS,
// also at the top of main(). It takes less than 7 iterations to finish the graphs of B(2,3) and B(2,4).
//
//   I marked EXPONENT and MAX_ITERATIONS below with comment asterisk bars.
//
//   Finally, output is currently streamed via console, and should be fairly straightforward to read
// (although there is some junk between the reports which I've been using for diagnostic purposes).
// By "x-edge" I refer to a pair (a, b) such that ax = b, and similarly for "y-edge". The graph
// reports give you all the information needed to construct the graph which the Graph object represents.
//***********************
//***********************


#include <iostream>
#include "declarations.h"
#include "node.h"
#include "graph.h"
#include "identity.h"
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

int main()
{
    //************************
    //************************
    const int MAX_ITERATIONS = 100;
    const int EXPONENT = 5;
    //************************
    //************************


    //Seed the random number generator
    srand(time(0));

    //Create the Graph object with user-defined exponent
    Graph graph(EXPONENT);

    //Create the first base graph and report its contents
    graph.initStep();
    graph.report();

    //Pause for input
    std::string spurn;
    std::cout << "Press any key to continue: ";
    std::cin >> spurn;
    //Continue

    int iterationCounter = 0;  //counts how many graph iterations have occured; used to stop main loop in case
                               //it goes on for too long (whatever that may be)

    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);  //used to detect null iterators


    //This is the main loop that will build the graph. It terminates when either every node has a defined x product
    //and y product, or the loop has continued for MAX_ITERATIONS-many iterations.
    while(!graph.isFinished() && iterationCounter < MAX_ITERATIONS)
    {

        std::list< std::list<Node>::iterator > identityBaseNodes;

        //Loop over all nodes in the graph; if a node does no have a defined x or y product, build it and store
        //a pointer to the new node(s) in identityBaseNodes. Then set the original node to CLOSED.
        for (std::list<Node>::iterator pn = graph.getNodeListBegin(); pn != graph.getNodeListEnd(); pn++)
        {
            if (pn->getNodeType() == Node::OPEN)
            {
                std::list<Node>::iterator pprod;
                if (pn->isXoutEmpty()) //if no defined x-product
                {
                    pprod = graph.buildXprod(pn);
                    identityBaseNodes.push_back(pprod);
                }
                if (pn->isYoutEmpty())
                {
                    pprod = graph.buildYprod(pn);
                    identityBaseNodes.push_back(pprod);
                }
                pn->setNodeType(Node::CLOSED);
            }
        }


        //Use the new nodes in identityBaseNodes as bases for identities and assign the correct
        //type (OPEN or CLOSED) to these nodes.
        for (std::list< std::list<Node>::iterator >::iterator pn = identityBaseNodes.begin();
             pn != identityBaseNodes.end(); pn++)
        {
            graph.generateIdentities(*pn, true, iterationCounter);
            if ((*pn)->isOpen())
                (*pn)->setNodeType(Node::OPEN);
            else (*pn)->setNodeType(Node::CLOSED);
        }


        //The identityBank data member of our Graph object is now populated with Identity objects. This
        //loop sets the left node of each Identity to type IDENT, ensuring that it is not deleted
        //prematurely.
        for (std::list<Identity>::iterator pid = graph.getIdentBegin(); pid != graph.getIdentEnd(); pid++)
            (pid->getLeft())->setNodeType(Node::IDENT);


        //Loop through identities. For each identity, build the path from the base node to the left node,
        //use this path to algebraically reduce the identity, delete excess nodes in the path, and copy
        //the edges between nodes which are equal as group elements. Details below.
        std::list<Identity>::iterator pid = graph.getIdentBegin();
        while (pid != graph.getIdentEnd())
        {
            //Each identity has a base node and a left node. Build a path from the base node to the left node
            //and store that path in identityPath.
            std::list< std::list<Node>::iterator > identityPath = graph.buildPath2(pid->getBase(), pid->getLeft());
            if (identityPath.empty()) //this would be bad; it has never happened
            {
                std::cout << "PATH BUILD UNSUCCESSFUL FOR IDENTITY:\n";
                pid->printIdent();
                std::cout << std::endl;
            }
            else
            {
                //reduceIdent2() uses the fact that the left node and the right node are equal as group elements
                //and traces backwards through the path we just built to derive a new Identity, which is
                //saved as reducedId. This is a crucial step; details in the function definition and project paper.
                Identity reducedId = graph.reduceIdent2(pid, identityPath);

                //Set all the nodes before the left node in reducedId to either OPEN or CLOSED, ensuring that
                //they are not deleted in the next step.
                graph.preservePath2(identityPath, reducedId.getLeft());

                //Delete all nodes which were not preserved above. Then loop through the remaining nodes
                //and assign to each node the correct type (either OPEN or CLOSED).
                graph.deleteTempNodes();
                for (std::list<Node>::iterator pln = graph.getNodeListBegin(); pln != graph.getNodeListEnd(); pln++)
                    if (pln->isOpen())
                        pln->setNodeType(Node::OPEN);
                    else pln->setNodeType(Node::CLOSED);


                //The left node and right node of reducedId are the same as group elements. These functions copy
                //the nodes between each node's product lists, making them larger; the program will later detect
                //that there are multiple nodes in these product lists and use them to reduce the graph.
                graph.copyNodeEdges(reducedId.getLeft(), reducedId.getRight());
                graph.copyNodeEdges(reducedId.getRight(), reducedId.getLeft());
            }


            //Identity has been processed; delete IDENT node, remove it from identityBank, and reset iterator
            graph.removeNode(pid->getLeft());
            graph.eraseIdent(pid);
            pid = graph.getIdentBegin();


            //Just letting the user know what's going on
            std::cout << "\nCurrent graph size: " << graph.getSize() << " nodes";

        }


        //Implements several identities of a different form. generateIdentities2() randomly
        //generates identities of a certain form and implements them in the graph by adjusting
        //the product lists of appropriate nodes. For details see function definition in graph3.cpp
        graph.generateIdentities2(4, 1000);
        graph.generateIdentities2(6, 1000);
        graph.generateIdentities2(8, 1000);
        graph.generateIdentities2(10, 1000);
        graph.generateIdentities2(12, 1000);


        //Detect nodes in the graph that are the same as group elements, and delete all but one of them,
        //reducing the graph.
        bool cyclesSet = true;
        while (cyclesSet)
        {
            cyclesSet = false;

            std::list<Node>::iterator pnRudolpho = graph.getNodeListBegin();
            while (pnRudolpho != graph.getNodeListEnd())
            {
                //clearDuplicateNodes() will check the node's product lists to see if there is more
                //than one node; if so it deletes all but one of these nodes and returns the value true
                if (graph.clearDuplicateNodes(pnRudolpho)) //if duplicate nodes were found
                    pnRudolpho = graph.getNodeListBegin(); //then reset iterator
                else
                    pnRudolpho++; //otherwise go to the next node
            }

            //Search for "x-cycles" and "y-cycles" in the graph and set edges to close them. Definition found
            //in project paper.
            if (graph.findXcycle())
                cyclesSet = true;
            if (graph.findYcycle())
                cyclesSet = true;

            //If cycles were found and new edges were set then there may be repeated nodes in the graph,
            //so the loop repeats in order to further reduce the graph.
        }


        //Report status
        std::cout << "\nCurrent iteration: " << iterationCounter;
        std::cout << "\nCurrent graph size: " << graph.getSize() << " nodes\n";

        iterationCounter++;
    }


    //Print results
    if (graph.isFinished())
        std::cout << "\nFinished graph size: " << graph.getSize() << " nodes\n";
    else
        std::cout << "\nUnfinished graph size: " << graph.getSize() << " nodes\n";


    ///Reducing the closed graph
    ///
    std::string spuds;
    std::cout << "\nImplement last identities? (y/n): ";
    std::cin >> spuds;
    ///Creates random identities of a certain length
    while (spuds[0] == 'y')
    {
        int length;
        int numIdents;

        std::cout << "\nEnter length: ";
        cin >> length;
        std::cout << "\nEnter number of identities: ";;
        cin >> numIdents;

        graph.generateIdentities2(length, numIdents);

        ///Clear duplicate nodes
        bool cyclesSet2 = true;
        while (cyclesSet2)
        {
            cyclesSet2 = false;

            std::list<Node>::iterator pnRudolpho2 = graph.getNodeListBegin();
            while (pnRudolpho2 != graph.getNodeListEnd())
            {
                if (graph.clearDuplicateNodes(pnRudolpho2)) //if duplicate nodes were found
                    pnRudolpho2 = graph.getNodeListBegin(); //then reset iterator
                else
                    pnRudolpho2++; //otherwise go to the next node
            }

            if (graph.findXcycle())
                cyclesSet2 = true;
            if (graph.findYcycle())
                cyclesSet2 = true;

            //If cycles were found and new edges were set in find<>cycle(), then loop will repeat
        }

        std::cout << "\nGraph size: " << graph.getSize() << "\n";
        std::cout << "\nImplement more identities? (y/n): ";
        spuds.clear();
        cin >> spuds;
    }
    ///
    ///End of graph reduction


    std::cout << "\nReport graph? (y/n)";
    std::string spool;
    cin >> spool;
    if (spool[0] == 'y')
        graph.report();

    ///REMOVE
    ///(for serious)
    std::fstream fout("graph.txt");
    graph.report(fout);
    ///REMOVE
    ///(really for serious)

    std::string spark;
    std::cout << "\nPress any key to exit: ";
    std::cin >> spark;
}

