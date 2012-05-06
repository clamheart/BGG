#include "node.h"
#include "graph.h"
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

//---
//Graph function definitions
//---


//Constructors
Graph::Graph()
{
    exponent = 0;
}

Graph::Graph(int exp)
{
    exponent = exp;
}
//Note re. constructors: the initial constructon of the Graph object doesn't matter, because the initial step is what
//will really set the graph up to begin the iterative process of extending the graph to encompass the whole group.


//report; prints graph information
void Graph::report(std::ostream& fout)
{
    fout << std::endl
              << "Report:\n"
              << "******\n"
              << "1) Open nodes:\n";
    for(std::list<Node>::iterator pv = nodeList.begin(); pv != nodeList.end(); pv++)
    {
        if (pv->getNodeType() == Node::OPEN)
        {
            pv->printWord(fout);
            fout << "\n";
        }
    }


    fout << "---\n"
         << "2) Closed nodes:\n";
    for(std::list<Node>::iterator pv = nodeList.begin(); pv != nodeList.end(); pv++)
    {
        if (pv->getNodeType() == Node::CLOSED)
        {
            pv->printWord(fout);
            fout << "\n";
        }
    }


    fout << "---\n"
         << "3) Temp nodes:\n";
    for(std::list<Node>::iterator pv = nodeList.begin(); pv != nodeList.end(); pv++)
    {
        if (pv->getNodeType() == Node::TEMP)
        {
            pv->printWord(fout);
            fout << "\n";
        }
    }


    fout << "---\n";
    fout << "Y-edges:\n";
    for(std::list<Node>::iterator pv = nodeList.begin(); pv != nodeList.end(); pv++)
    {
        if (!pv->isYoutEmpty()) //if Y-edge exists
        {
            for (std::list< std::list<Node>::iterator >::iterator pprod = pv->getYoutBegin();
                 pprod != pv->getYoutEnd(); pprod++)
            {
                fout << "(";
                pv->printWord(fout);
                fout << ", ";
                (*pprod)->printWord(fout);
                fout << ")\n";
            }
        }
    }


    fout << "---\n";
    fout << "X-edges:\n";
    for(std::list<Node>::iterator pv = nodeList.begin(); pv != nodeList.end(); pv++)
    {
        if (!pv->isXoutEmpty()) //if Y-edge exists
        {
            for (std::list< std::list<Node>::iterator >::iterator pprod = pv->getXoutBegin();
                 pprod != pv->getXoutEnd(); pprod++)
            {
                fout << "(";
                pv->printWord(fout);
                fout << ", ";
                (*pprod)->printWord(fout);
                fout << ")\n";
            }
        }
    }

    fout << std::endl;
}


//setXedge and setYedge; these set appropriate pointers in the argument nodes to reflect the existence of an x-edge (or
//y-edge respectively) from first to second node
bool Graph::setXedge(std::list<Node>::iterator pn1, std::list<Node>::iterator pn2)
{
    if (pn1 == std::list<Node>::iterator(NULL) || pn2 == std::list<Node>::iterator(NULL)) //ensure nodes are valid
        return false;

    bool noDup = true;
    std::list< std::list<Node>::iterator >::iterator pln = pn1->getXoutBegin();
    while (pln != pn1->getXoutEnd())
    {
        if ((*pln) == pn2)  //if pn2 is already listed as a product node
            noDup = false;  //use this to prevent adding it again

        pln++;
    }
    if (noDup)
        pn1->setXout(pn2);

    noDup = true;
    pln = pn2->getXinBegin();
    while (pln != pn2->getXinEnd())
    {
        if ((*pln) == pn1)  //if pn1 is already listed as am inverse product node
            noDup = false;  //use this to prevent adding it again

        pln++;
    }
    if (noDup)
        pn2->setXin(pn1);

    return true;
}

bool Graph::setYedge(std::list<Node>::iterator pn1, std::list<Node>::iterator pn2)
{
    if (pn1 == std::list<Node>::iterator(NULL) || pn2 == std::list<Node>::iterator(NULL)) //check that nodes have valid addresses
        return false;

    bool noDup = true;
    std::list< std::list<Node>::iterator >::iterator pln = pn1->getYoutBegin();
    while (pln != pn1->getYoutEnd())
    {
        if ((*pln) == pn2)  //if pn2 is already listed as a product node
            noDup = false;  //use this to prevent adding it again

        pln++;
    }
    if (noDup)
        pn1->setYout(pn2);

    noDup = true;
    pln = pn2->getYinBegin();
    while (pln != pn2->getYinEnd())
    {
        if ((*pln) == pn1)  //if pn1 is already listed as am inverse product node
            noDup = false;  //use this to prevent adding it again

        pln++;
    }
    if (noDup)
        pn2->setYin(pn1);
    return true;
}


//searchNodes; searches nodeList for a node whose elemname member matches the given word
std::list<Node>::iterator Graph::searchNodes(std::vector<short>* pvs)
{
    std::vector<short> givenWord = *pvs;
    for (std::list<Node>::iterator pn = nodeList.begin(); pn != nodeList.end(); pn++)
    {
        bool matchFound = true;
        std::vector<short> currentWord = pn->getElemName();
        if (currentWord.size() != givenWord.size())
            matchFound = false;
        else
            for (int i = 0; i < currentWord.size(); i++)
                if (currentWord[i] != givenWord[i])
                {
                    matchFound = false;
                    break;
                }

        if(matchFound)
            return pn;
    }

    return std::list<Node>::iterator(NULL);
}


//initStep; the initial step which creates the first graph
void Graph::initStep()
{
    //First, use some initial identities to form the first graph. These have the form (xy^k)^n = 1. From this we
    //derive (xy^k)^(n-1)x = y^(n-k). Finding these identities for 0 < k < n expresses all powers of y as products
    //beginning with x. We can now construct the y-cycle of 1.

    //create first two nodes of the graph, 1 and x
    std::vector<short> tempElemName;
    tempElemName.push_back(0);   //set to the identity
    Node identity(tempElemName); //create identity node
    nodeList.push_back(identity);
    tempElemName[0] = 1;   //set to x^1
    Node x1(tempElemName); //create x^1 node
    nodeList.push_back(x1);

    //these iterators used to set edges between nodes
    std::list<Node>::iterator pln = nodeList.begin();
    std::list<Node>::iterator plntarget = pln;
    plntarget++;
    setXedge(pln, plntarget);  //set x-edge from 1 to x^1


    //use aforementioned identities to build branches of the graph
    //first, build xy, xy^2, ... , xy^(n-1)
    std::vector<short> tempWord(2);
    tempWord[0] = 1;
    tempWord[1] = 1; //initialize tempWord to xy
    Node tempNode(tempWord); //make xy a node
    nodeList.push_back(tempNode); //stick it in openNodes
    pln++;
    plntarget++;
    setYedge(pln, plntarget); //form y-edge (x, xy)


    int i;
    for (i = 2; i < exponent; i++)
    {
        tempWord[1] = i; //set y-exponent
        Node tempNode(tempWord);
        nodeList.push_back(tempNode); //insert new node in nodeList
        pln++;
        plntarget++;
        setYedge(pln, plntarget); //connect it by y to previous node
    }
    pln++;
    plntarget = nodeList.begin();
    plntarget++;
    setYedge(pln, plntarget);  //final y-edge (xy^(n-1), x)


    //now build paths to target words in the identities
    //loop over exponents of k, or rather, over identities (xy^k)^(n-1)
    std::vector<std::list<Node>::iterator> targetNodeVec;  //this will be used to create the y-cycle of 1
    for (int k = 1; k < exponent; k++)
    {
        //choose the first node in the path
        std::vector<short> startWord(2);
        startWord[0] = 1;
        startWord[1] = k;  //startWord is now set to x^1y^k

        //find start node in openNodes and set pstartNode iterator to it
        std::list<Node>::iterator pstartNode = searchNodes(&startWord);
        if (pstartNode == std::list<Node>::iterator(NULL)) //this indicates the word was not found in nodeList;
            break;                                         //shouldn't happen


        //create target word
        std::vector<short> targetWord;
        for (int i = 1; i <= exponent - 1; i++)
        {
            targetWord.push_back(1);  //append x^1
            targetWord.push_back(k);  //append y^k
        }
        //do this n - 1 times, and now targetWord is (xy^k)^(n-1)
        targetWord.push_back(1);  //targetWord = (xy^k)^(n-1)x; targetWord is complete


        //build path from *pstartNode to targetWord
        bool letterSwitch = 1;   //x = 1, y = 0
        //loop over letters in the target word
        for (std::vector<short>::iterator pv = targetWord.begin() + 2; pv != targetWord.end(); pv++)
        {
            int letterExponent = *pv;  //the exponent of the current letter

            //loop over exponents in the letter
            for (int i = 1; i <= letterExponent; i++)
            {
                std::vector<short> newWord;
                //copy stuff before the current letter into newWord
                for (std::vector<short>::iterator pvv = targetWord.begin(); pvv != pv; pvv++)
                    newWord.push_back(*pvv);
                //insert new exponent
                newWord.push_back(i);
                Node newNode(newWord);
                nodeList.push_back(newNode);  //stick new node into openNodes

                //form edge from previous node to the node you just stuck in openNodes
                std::list<Node>::iterator pnextNode = nodeList.end();
                pnextNode--;
                if (letterSwitch) //if x
                    setXedge(pstartNode, pnextNode);
                else
                    setYedge(pstartNode, pnextNode);

                //move up the current node to the one you just stuck in openNodes
                pstartNode = pnextNode;


                //this is a special case that creates the edge ( (xy^(n-1))^r, (xy^(n-1))^(r-1)x )
                if (i == letterExponent && letterExponent == exponent - 1 && letterSwitch == 0) //letter is y
                {
                    //build target node and use searchNode to find your target
                    std::vector<short> specTargetWord;
                    int d = pv - targetWord.begin();  //d measures how far the current letter is in targetWord
                    for (int j = 1; j <= (d - 1) / 2; j++)
                    {
                        specTargetWord.push_back(1);
                        specTargetWord.push_back(exponent - 1);
                    }
                    specTargetWord.push_back(1);
                    //now specTargetWord is (xy^(n-1))^(r-1)x, which is what we wanted
                    std::list<Node>::iterator pspecTargetNode = searchNodes(&specTargetWord);
                    setYedge(pnextNode, pspecTargetNode);
                }

                //another special case; if you are now creating the targetWord node, you want to store it
                //in order to create the y-cycle of 1; naturally, the targetWord is a distance of 1 from
                //the 1-past-the-end of targetWord
                if (i == letterExponent && (targetWord.end() - pv) == 1)
                    targetNodeVec.push_back(pnextNode);
            }

            //flip the letter switch and you're ready for the next loop
            letterSwitch = !letterSwitch;
        }
    }

    //Now just link together the y-cycle of 1, an iterator to each node of which you've stored in targetNodeVec
    setYedge(nodeList.begin(), targetNodeVec[targetNodeVec.size() - 1]); //creates y-edge (1, y)
    int counter;
    for (counter = targetNodeVec.size() - 1; counter > 0; counter--)
        setYedge(targetNodeVec[counter], targetNodeVec[counter - 1]);
    setYedge(targetNodeVec[counter], nodeList.begin());


    //In order to circumvent an issue with buildXprod(), create the x-cycle of 1.
    std::list<Node>::iterator pXstart = nodeList.begin();
    pXstart++; //pXstart now points to x^1
    for (int cntr = 2; cntr < exponent; cntr++)
    {
        std::vector<short> xWord;
        xWord.push_back(cntr);
        Node xNode(xWord);
        nodeList.push_back(xNode);
        std::list<Node>::iterator pXtarget = nodeList.end();
        pXtarget--; //pXtarget now points to node just inserted, iie. x^(cntr)
        setXedge(pXstart, pXtarget);
        pXstart = pXstart->getXout(); //pXstart now points to x^(cntr)
    }
    //set last xedge (x^(n-1), 1)
    setXedge(pXstart, nodeList.begin());


    //All edges and nodes have been created. Now search openNodes for those which are closed, erase
    //them, and put them in closedNodes.
    for (std::list<Node>::iterator pp = nodeList.begin(); pp != nodeList.end(); pp++)
        if (pp->isOpen())
            pp->setNodeType(Node::OPEN);
        else pp->setNodeType(Node::CLOSED);
}


//remove all ties to a node, then delete it
void Graph::removeNode(std::list<Node>::iterator pn)
{
    //remove all edges between adjacent nodes
    if (!pn->isXinEmpty())
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = pn->getXinBegin();
             pln != pn->getXinEnd(); pln++)
            (*pln)->removeFromXout(pn);
    }
    if (!pn->isYinEmpty())
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = pn->getYinBegin();
             pln != pn->getYinEnd(); pln++)
            (*pln)->removeFromYout(pn);
    }
    if (!pn->isXoutEmpty())
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = pn->getXoutBegin();
             pln != pn->getXoutEnd(); pln++)
            (*pln)->removeFromXin(pn);
    }
    if (!pn->isYoutEmpty())
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = pn->getYoutBegin();
             pln != pn->getYoutEnd(); pln++)
            (*pln)->removeFromYin(pn);
    }

    nodeList.erase(pn);
}


//setClosedNodes; set nodes that have become closed to type closed
void Graph::setClosedNodes()
{
    for (std::list<Node>::iterator pp = nodeList.begin(); pp != nodeList.end(); pp++)
        if (pp->getNodeType() == Node::OPEN && !pp->isOpen())
            pp->setNodeType(Node::CLOSED);
}


//isFinished; determines whether graph is finished by checking if all nodes are closed
bool Graph::isFinished()
{
    for (std::list<Node>::iterator pp = nodeList.begin(); pp != nodeList.end(); pp++)
        if (pp->getNodeType() != Node::CLOSED)
            return false;
    return true;
}


//buildXprod; find or build and link to the x product of the passed-in node; return product node
std::list<Node>::iterator Graph::buildXprod(std::list<Node>::iterator pn)
{
    //if an x product is already defined in the node then return immediately
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (!pn->isXoutEmpty())
        return *(pn->getXoutBegin());

    std::vector<short> originalWord = pn->getElemName();
    std::vector<short> newWord = originalWord;


    //used to handle a special case - see "else if" comment below
    bool specialEdge = false;

    //determine what happens when x is appended to the given word
    if (originalWord.size() % 2 == 1) //in this case word ends in x
    {
        //get last exponent to see what happens
        int letterExponent = originalWord[originalWord.size() - 1];

        if (letterExponent == exponent - 1) //in this case the last letter reduces to the identity
        {
            //Note: x^(n-1) must be handled as a special case, since below would give it length 0;
            //x^(n-1) will be handled in the initial step, so will have a defined product, so won't get here

            //erase last letter from newWord
            newWord.pop_back();
        }
        else //otherwise the exponent on the last letter (which is x) increases
        {
            if (letterExponent == exponent - 2) //in this case word will be gx^(n-1) which we can connect
                specialEdge = true;             //by a special x-edge to g, if g exists

            //increment the last letter
            newWord[newWord.size() - 1] = newWord[newWord.size() - 1] + 1;
        }
    }
    else //in this case word ends in y
    {
        //just stick an x^1 on the end of the word
        newWord.push_back(1);
    }


    //So now we have a good target word to look for but maybe the product already exists
    //in some other form, so let's look for that first.


    //First, we could be at the end of an x-cycle, in which case we can go back by n-1 x-edges
    //to find the x-product.
    int edgesTraversed = 1;
    std::list<Node>::iterator pcurrentNode = pn;
    std::list<Node>::iterator pnextNode = pn->getXin();  //"next" is the one with an edge IN to "current"
    //if more chain to go...
    while (pnextNode != pnull && edgesTraversed < exponent - 1)
    {
        //...then move up the chain
        pcurrentNode = pnextNode;
        pnextNode = pcurrentNode->getXin();
        edgesTraversed++;
    }
    //if you traversed the whole chain, you've found the x product, so set it and return
    if (edgesTraversed == exponent - 1 && pnextNode != pnull)
    {
        //pnextNode points to the last node in the chain, which is your x-product
        if(setXedge(pn, pnextNode))
        {
            ///print info to file
        }

        //handle special case
        if (specialEdge)
        {
            //take off the last letter and find this word
            newWord.pop_back();
            std::list<Node>::iterator pspecTarget = searchNodes(&newWord);
            if (pspecTarget != std::list<Node>::iterator(NULL)) //if found
                if (setXedge(pnextNode, pspecTarget)) //set special edge
                {
                    ///print info to file
                }
        }

        return pnextNode;
    }


    ///
    ///This block searches by word, and could be restored
    ///
    /*
    //At this point we know the product, if it exists, is the word we made above. So search
    //for that word and if found, connect an x-edge.
    std::list<Node>::iterator searchResult;
    searchResult = searchNodes(&newWord);
    if (searchResult != pnull) //if you found it in one of the above...
    {
        //...then link and return
        if (setXedge(pn, searchResult))
        {
            ///print info to file
        }

        //handle special case
        if (specialEdge)
        {
            //take off the last letter and find this word
            newWord.pop_back();
            std::list<Node>::iterator pspecTarget = searchNodes(&newWord);
            if (pspecTarget != std::list<Node>::iterator(NULL)) //if found
                if (setXedge(searchResult, pspecTarget)) //set special edge
                {
                    ///print to file
                }
        }

        return searchResult;
    }
    */
    ///
    ///End of search block
    ///


    //Now we know that the x-product is not a built node of any form, so we must build it,
    //place it in tempNodes, and link to it.
    Node newNode(newWord);
    nodeList.push_back(newNode);
    std::list<Node>::iterator pnewNode = nodeList.end();
    pnewNode--;
    //now pnewNode points to the x-product, which has been placed in tempNodes
    setXedge(pn, pnewNode);

    //handle special case
    if (specialEdge)
    {
        //take off the last letter and find this word
        newWord.pop_back();
        std::list<Node>::iterator pspecTarget = searchNodes(&newWord);
        if (pspecTarget != std::list<Node>::iterator(NULL)) //if found
            setXedge(pnewNode, pspecTarget); //set special edge
    }

    return pnewNode;
}


//buildYprod; find or build and link to the y product of the passed-in node; for comments on procedure
//see above function buildXprod
std::list<Node>::iterator Graph::buildYprod(std::list<Node>::iterator pn)
{
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (pn->getYout() != pnull)
        return pn->getYout();

    std::vector<short> originalWord = pn->getElemName();
    std::vector<short> newWord = originalWord;

    bool specialEdge = false;

    if (originalWord.size() % 2 == 0)
    {
        int letterExponent = originalWord[originalWord.size() - 1];

        if (letterExponent == exponent - 1)
        {
            newWord.pop_back();
        }
        else
        {
            if (letterExponent == exponent - 2)
                specialEdge = true;

            newWord[newWord.size() - 1] = newWord[newWord.size() - 1] + 1;
        }
    }
    else
    {
        newWord.push_back(1);
    }

    int edgesTraversed = 1;
    std::list<Node>::iterator pcurrentNode = pn;
    std::list<Node>::iterator pnextNode = pn->getYin();
    while (pnextNode != pnull && edgesTraversed < exponent - 1)
    {
        pcurrentNode = pnextNode;
        pnextNode = pcurrentNode->getYin();
        edgesTraversed++;
    }
    if (edgesTraversed == exponent - 1 && pnextNode != pnull)
    {
        setYedge(pn, pnextNode);

        if (specialEdge)
        {
            newWord.pop_back();
            std::list<Node>::iterator pspecTarget = searchNodes(&newWord);
            if (pspecTarget != std::list<Node>::iterator(NULL))
                setYedge(pnextNode, pspecTarget);
        }

        return pnextNode;
    }



    ///
    ///This block searches by word, and could be restored
    ///
    /*
    std::list<Node>::iterator searchResult;
    searchResult = searchNodes(&newWord);
    if (searchResult != pnull)
    {
        setYedge(pn, searchResult);

        if (specialEdge)
        {
            newWord.pop_back();
            std::list<Node>::iterator pspecTarget = searchNodes(&newWord);
            if (pspecTarget != std::list<Node>::iterator(NULL))
                setYedge(searchResult, pspecTarget);
        }

        return searchResult;
    }
    */
    ///
    ///End of search block
    ///

    Node newNode(newWord);
    nodeList.push_back(newNode);
    std::list<Node>::iterator pnewNode = nodeList.end();
    pnewNode--;
    setYedge(pn, pnewNode);

    if (specialEdge)
    {
        newWord.pop_back();
        std::list<Node>::iterator pspecTarget = searchNodes(&newWord);
        if (pspecTarget != std::list<Node>::iterator(NULL))
            setYedge(pnewNode, pspecTarget);
    }

    return pnewNode;
}


//findXcycle; search for x-cycles and close them off
bool Graph::findXcycle()
{
    std::list<Node>::iterator pnodeList = nodeList.begin();
    bool edgeSet = false;
    while (pnodeList != nodeList.end())
    {
        std::list<Node>::iterator plastNode = pnodeList;
        int productsFound = 0;

        //stop loop when you fail to find a product or when you've found enough to close cycle
        while (!plastNode->isXoutEmpty() && productsFound < exponent - 1)
        {
            plastNode = plastNode->getXout();
            productsFound++;
        }

        if (productsFound == exponent - 1) //in this case you can close the cycle
        {
            //first make sure the edge hasn't already been formed
            //if either component is missing, set the edge and ensure function returns true
            if ( !(pnodeList->isInXin(plastNode)) || !(plastNode->isInXout(pnodeList)) )
            {
                setXedge(plastNode, pnodeList);
                edgeSet = true;
            }
        }

        pnodeList++;
    }

    return edgeSet;
}


//findXcycle; search for y-cycles and close them off
bool Graph::findYcycle()
{
    std::list<Node>::iterator pnodeList = nodeList.begin();
    bool edgeSet = false;
    while (pnodeList != nodeList.end())
    {
        std::list<Node>::iterator plastNode = pnodeList;
        int productsFound = 0;

        //stop loop when you fail to find a product or when you've found enough to close cycle
        while (!plastNode->isYoutEmpty() && productsFound < exponent - 1)
        {
            plastNode = plastNode->getYout();
            productsFound++;
        }

        if (productsFound == exponent - 1) //in this case you can close the cycle
        {
            //first make sure the edge hasn't already been formed
            //if either component is missing, set the edge and ensure function returns true
            if ( !(pnodeList->isInYin(plastNode)) || !(plastNode->isInYout(pnodeList)) )
            {
                setYedge(plastNode, pnodeList);
                edgeSet = true;
            }
        }

        pnodeList++;
    }

    return edgeSet;
}

