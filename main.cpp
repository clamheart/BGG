//***********************
//***********************
//   This file is messy but there are only a few things you need to know to adjust it.
//
//   First, you can adjust the exponent of the graph by setting EXPONENT
// at the top of main(). Setting it to 2 will probably cause a crash. Setting it above 4
// will produce an enormous graph.
//
//   Second, you can adjust the number of times the main loop will run by setting MAX_ITERATIONS,
// also at the top of main(). It takes less than 7 iterations to finish the graphs of B(2,3) and B(2,4).
//
//   I marked EXPONENT and MAX_ITERATIONS below with little comment asterisk bars.
//
//   There is a new constant bool, USE_ALT_PATH_SYSTEM. This should be left with the value true. If it's set
// tp false there's no telling what will happen (probably something bad).
//
//   Finally, output is currently streamed via console, and should be fairly straightforward to read
// (although there is some junk between the reports which I've been using for diagnostic purposes).
// By "x-edge" I refer to a pair (a, b) such that ax = b, and similarly for "y-edge". The graph
// reports give you all the information needed to construct the graph which the Graph object represents.
//
//   The identities generated are written to the file "identities.txt" which can be found in the project folder.
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
    const int EXPONENT = 4;
    const bool USE_ALT_PATH_SYSTEM = true;
    //************************
    //************************


    srand(time(0)); //seed rand()


    Graph graph(EXPONENT); //pass in exponent


    //Create the first base graph and report its contents
    graph.initStep();
    graph.report();

    ///Pause for input
    std::string spurn;
    std::cout << "Press any key to continue: ";
    std::cin >> spurn;
    ///Continue

    int iterationCounter = 0;  //counts how many graph iterations have occured; used to stop main loop in case
                               //it goes on for too long (whatever that may be)


    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);  //used to detect null iterators
    while(!graph.isFinished() && iterationCounter < MAX_ITERATIONS)
    {

        //Go to each open node and build its missing products; store products in identityBaseNodes

        std::list< std::list<Node>::iterator > identityBaseNodes;

        for (std::list<Node>::iterator pn = graph.getNodeListBegin(); pn != graph.getNodeListEnd(); pn++)
        {
            if (pn->getNodeType() == Node::OPEN)
            {
                std::list<Node>::iterator pprod;
                if (pn->isXoutEmpty()) //if no defined x-product
                {
                    pprod = graph.buildXprod(pn);
                    //if (pprod->isOpen())  //only use as base for identities if it's OPEN
                        identityBaseNodes.push_back(pprod);
                }
                if (pn->isYoutEmpty())
                {
                    pprod = graph.buildYprod(pn);
                    //if (pprod->isOpen())
                        identityBaseNodes.push_back(pprod);
                }
                pn->setNodeType(Node::CLOSED);
            }
        }


        //This will sort the products you built and stored in tempNodes into (probably) openNodes
        ///graph.sortTempNodes();  now, this is handled in the loop above

        //This sets the nodes whose products were just built as closed nodes, which ensures that they
        //won't be the base nodes for the identities that are about to be generated
        ///graph.setClosedNodes();  also handled in loop above


        //Generate identities from new product nodes; assign preservation to base nodes
        for (std::list< std::list<Node>::iterator >::iterator pn = identityBaseNodes.begin();
             pn != identityBaseNodes.end(); pn++)
        {
            graph.generateIdentities(*pn, true, iterationCounter);
            if ((*pn)->isOpen())
                (*pn)->setNodeType(Node::OPEN);
            else (*pn)->setNodeType(Node::CLOSED);
        }


        //Erase duplicate identities and destroy associated nodes
        for (std::list<Identity>::iterator pid = graph.getIdentBegin(); pid != graph.getIdentEnd(); pid++)
        {
            /*
            std::cout << "IDENTITY: ";
            pid->printIdent();
            std::cout << "\n";
            */
            ///Ensure identity node does not get deleted before it has been processed
            (pid->getLeft())->setNodeType(Node::IDENT);
        }


        //Loop through identities; construct paths, reduce, and eliminate excess nodes for each one,
        //assigning preservation as appropriate
        std::list<Identity>::iterator pid = graph.getIdentBegin();
        while (pid != graph.getIdentEnd())
        {
            ///Use original system: build temp nodes, use iters to reduce and preserve
            if (!USE_ALT_PATH_SYSTEM)
            {

                //Build path from base node to left node
                if (!graph.buildPath(pid->getBase(), pid->getLeft())) //if path build unsuccessful
                {
                    std::cout << "PATH BUILD UNSUCCESSFUL FOR IDENTITY:\n";
                    pid->printIdent();
                    std::cout << std::endl;
                }

                //Reduce identity - but only if its RHS node was found
                if (pid->getRight() != std::list<Node>::iterator(NULL))
                {

                    Identity reducedId = graph.reduceIdent(pid);


                    //Assign preservation to nodes occuring before (and including) left reduced node
                    graph.preservePath(reducedId.getBase(), reducedId.getLeft());


                    //Delete excess nodes, assign type to remaining nodes
                    graph.deleteTempNodes();
                    for (std::list<Node>::iterator pln = graph.getNodeListBegin(); pln != graph.getNodeListEnd(); pln++)
                        if (pln->isOpen())
                            pln->setNodeType(Node::OPEN);
                        else pln->setNodeType(Node::CLOSED);


                    //Set the edges embodying the identity to be processed later by removeDuplicateNodes()
                    graph.copyNodeEdges(reducedId.getLeft(), reducedId.getRight());
                    graph.copyNodeEdges(reducedId.getRight(), reducedId.getLeft());

                }
            }
            else ///Use alt. system; pass path between various functions to reduce and preserve nodes
            {

                std::list< std::list<Node>::iterator > identityPath = graph.buildPath2(pid->getBase(), pid->getLeft());
                if (identityPath.empty())
                {
                    std::cout << "PATH BUILD UNSUCCESSFUL FOR IDENTITY:\n";
                    pid->printIdent();
                    std::cout << std::endl;
                }
                else
                {
                    /*
                    ///TEST - ouput path
                    std::cout << "PATH BUILT from ";
                    (pid->getBase())->printWord();
                    std::cout << " to ";
                    (pid->getLeft())->printWord();
                    std::cout << ":\n";

                    for (std::list< std::list<Node>::iterator >::iterator ppath = identityPath.begin();
                         ppath != identityPath.end(); ppath++)
                    {
                        (*ppath)->printWord();
                        std::cout << "\n";
                    }
                    ///TEST
                    */

                    //Reduce identity
                    Identity reducedId = graph.reduceIdent2(pid, identityPath);


                    //Assign preservation to nodes occuring before (and including) left reduced node
                    graph.preservePath2(identityPath, reducedId.getLeft());


                    //Delete excess nodes, assign type to remaining nodes
                    graph.deleteTempNodes();
                    for (std::list<Node>::iterator pln = graph.getNodeListBegin(); pln != graph.getNodeListEnd(); pln++)
                        if (pln->isOpen())
                            pln->setNodeType(Node::OPEN);
                        else pln->setNodeType(Node::CLOSED);


                    //Set the edges embodying the identity to be processed later by removeDuplicateNodes()
                    graph.copyNodeEdges(reducedId.getLeft(), reducedId.getRight());
                    graph.copyNodeEdges(reducedId.getRight(), reducedId.getLeft());
                }

            }


            //Identity has been processed; delete IDENT node, remove it from identityBank, and reset iterator
            graph.removeNode(pid->getLeft());
            graph.eraseIdent(pid);
            pid = graph.getIdentBegin();

            ///REMOVE
            std::cout << "\nCurrent graph size: " << graph.getSize() << " nodes";

        }


        //Remove duplicate nodes; set conditions for next loop
        bool cyclesSet = true;
        while (cyclesSet)
        {
            cyclesSet = false;

            std::list<Node>::iterator pnRudolpho = graph.getNodeListBegin();
            while (pnRudolpho != graph.getNodeListEnd())
            {
                if (graph.clearDuplicateNodes(pnRudolpho)) //if duplicate nodes were found
                    pnRudolpho = graph.getNodeListBegin(); //then reset iterator
                else
                    pnRudolpho++; //otherwise go to the next node
            }

            if (graph.findXcycle())
                cyclesSet = true;
            if (graph.findYcycle())
                cyclesSet = true;

            //If cycles were found and new edges were set in find<>cycle(), then loop will repeat
        }


        ///Report and pause for input
        //graph.report();
        std::cout << "\nCurrent iteration: " << iterationCounter;
        std::cout << "\nCurrent graph size: " << graph.getSize() << " nodes\n";
        std::string spite;
        std::cout << "Press any key to continue: ";
        //std::cin >> spite;
        std::cout << "\n";
        ///Continue

        iterationCounter++;
    }


    //Print results
    if (graph.isFinished())
    {
        std::cout << "Finished graph:\n";
        //graph.report();
        std::cout << "\nGraph size: " << graph.getSize() << " nodes\n";
    }
    else
    {
        //graph.report();
        std::cout << "\nUnfinished graph size: " << graph.getSize() << " nodes\n";
    }

    ///Test; trying to get B(2,4) down to 4096 elements
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
    ///End of last identities section


    std::cout << "\nReport graph? (y/n)";
    std::string spool;
    cin >> spool;
    if (spool[0] == 'y')
        graph.report();


    std::string spark;
    std::cout << "\nPress any key to exit: ";
    std::cin >> spark;
}

