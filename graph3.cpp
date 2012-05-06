#include "node.h"
#include "graph.h"
#include "identity.h"
#include <vector>
#include <list>
#include <iostream>
#include <ctime>
#include <cstdlib>

//---
//Graph function definitions, part III
//---


//Reduce and return identity
Identity Graph::reduceIdent(std::list<Identity>::iterator pid)
{
    std::list<Node>::iterator pleftNode, prightNode, pbaseNode;
    pleftNode = pid->getLeft();
    prightNode = pid->getRight();
    pbaseNode = pid->getBase();
    std::vector<short> leftWord = pleftNode->getElemName();
    std::vector<short> baseWord = pbaseNode->getElemName();

    bool letterSwitch = pleftNode->lastLetterIsX();  //x <-> true, y <-> false

    bool endPath = false; //detect if right reduction has reached an empty product

    while (leftWord.size() != baseWord.size()) //loop over exponents until last exp. of baseWord
    {
        while (leftWord[leftWord.size() - 1] > 0)
        {
            //check right path; if exists, move left and right back one product
            if (letterSwitch) //for letter x
            {
                if (!prightNode->isXinEmpty())
                {
                    prightNode = prightNode->getXin();
                    pleftNode = pleftNode->getXin();
                    /*
                    ///Test: use search instead of pointers
                    std::vector<short> searchWord = leftWord;
                    searchWord[leftWord.size() - 1] = leftWord[leftWord.size() - 1] - 1;
                    if (searchWord[leftWord.size() - 1] == 0)
                        searchWord.pop_back();
                    pleftNode = searchNodes(&searchWord);
                    ///Test
                    */
                }
                else
                {
                    endPath = true;
                    break;
                }
            }
            else //for letter y
            {
                if (!prightNode->isYinEmpty())
                {
                    prightNode = prightNode->getYin();
                    pleftNode = pleftNode->getYin();
                    /*
                    ///Test: use search instead of pointers
                    std::vector<short> searchWord = leftWord;
                    searchWord[leftWord.size() - 1] = leftWord[leftWord.size() - 1] - 1;
                    if (searchWord[leftWord.size() - 1] == 0)
                        searchWord.pop_back();
                    pleftNode = searchNodes(&searchWord);
                    ///Test
                    */
                }
                else
                {
                    endPath = true;
                    break;
                }
            }
            //decrement last exponent
            leftWord[leftWord.size() - 1] = leftWord[leftWord.size() - 1] - 1;
        }

        //either loop broken prematurely by end of path...
        if (endPath)
            break;

        //...or exponent is now 0, so move to next exponent
        leftWord.pop_back();
        letterSwitch = !letterSwitch;
    }

    //if not at end of path, continue reducing
    if (!endPath)
    {
        while (baseWord[baseWord.size() - 1] != leftWord[leftWord.size() - 1])
        {
            if (letterSwitch)
            {
                if (!prightNode->isXinEmpty())
                {
                    prightNode = prightNode->getXin();
                    pleftNode = pleftNode->getXin();
                    /*
                    ///Test: use search instead of pointers
                    std::vector<short> searchWord = leftWord;
                    searchWord[leftWord.size() - 1] = leftWord[leftWord.size() - 1] - 1;
                    if (searchWord[leftWord.size() - 1] == 0)
                        searchWord.pop_back();
                    pleftNode = searchNodes(&searchWord);
                    ///Test
                    */
                }
                else
                {
                    endPath = true;
                    break;
                }
            }
            else
            {
                if (!prightNode->isYinEmpty())
                {
                    prightNode = prightNode->getYin();
                    pleftNode = pleftNode->getYin();
                    /*
                    ///Test: use search instead of pointers
                    std::vector<short> searchWord = leftWord;
                    searchWord[leftWord.size() - 1] = leftWord[leftWord.size() - 1] - 1;
                    if (searchWord[leftWord.size() - 1] == 0)
                        searchWord.pop_back();
                    pleftNode = searchNodes(&searchWord);
                    ///Test
                    */
                }
                else
                {
                    endPath = true;
                    break;
                }
            }
            leftWord[leftWord.size() - 1] = leftWord[leftWord.size() - 1] - 1;
        }
    }

    //make and return reduced identity;
    Identity reducedId(pleftNode, prightNode, pbaseNode);

    /*
    std::cout << "\nIdentity ";
    pid->printIdent();
    std::cout << "\nreduced to ";
    reducedId.printIdent();
    std::cout << "\n";
    */
    return reducedId;
}


//Set nodes in path to OPEN
void Graph::preservePath(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode)
{
    //make sure iterators are valid
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (pbaseNode == pnull || ptargetNode == pnull)
    {
        //std::cout << "\nNULL ITERATORS PASSED TO preservePath()\n";
        return;
    }

    std::vector<short> baseWord = pbaseNode->getElemName();
    std::vector<short> targetWord = ptargetNode->getElemName();

    //target word must be longer than or equal length as base word
    if (targetWord.size() < baseWord.size())
    {
        //std::cout << "\nPRESERVE PATH BAD RETURN1\n";
        return;
    }
    //should never touch the identity; this is a special case which is handled in initial step
    if (baseWord[0] == 0 || targetWord[0] == 0)
    {
        //std::cout << "\nPRESERVE PATH BAD RETURN2\n";
        return;
    }
    //if words are identical there's nothing to do
    if (baseWord == targetWord)
    {
        //std::cout << "\nPRESERVE PATH BAD RETURN3\n";
        return;
    }
        //return;

    //At this point we know the nodes exist, their words have appropriate sizes, they're not the identity,
    //and they're not equal. But we could still have the problem that the base word is not contained
    //in the leftmost portion of the target word, so we check for that.
    bool baseContained = true;
    int i;
    for (i = 0; i < baseWord.size() - 1; i++)  //up until the last letter, each exponent identical
        if (baseWord[i] != targetWord[i])
            baseContained = false;
    if (baseWord[i] > targetWord[i])  //on the last letter, the exponent in base may be less or equal
        baseContained = false;
    if (baseContained == false)
    {
        std::cout << "\nPRESERVE PATH not contained:\n";
        pbaseNode->printWord();
        std::cout << "\n";
        ptargetNode->printWord();
        std::cout << "\n";
        return;
    }

    //nodes have been checked


    std::list<Node>::iterator psetNode = ptargetNode;
    while (targetWord.size() != baseWord.size()) //stop at last exponent of baseWord
    {
        int i = targetWord.size() - 1;  //always index of last exponent in targetWord
        int exp = targetWord[i];  //last exponent on targetWord
        while (exp > 0)
        {
            targetWord[i] = exp;
            psetNode = searchNodes(&targetWord);
            if (psetNode == std::list<Node>::iterator(NULL))
            {
                std::cout << "\nAttempt to set node via null iterator in preservePath()\n";
                return;
            }
            if (psetNode->getNodeType() != Node::TEMP && psetNode != ptargetNode)
            {
                //it's okay to hit a preserved node, this means the nodes before this one will also
                //be preserved
                return;
            }
            psetNode->setNodeType(Node::OPEN);
            exp--;
        }
        targetWord.pop_back(); //remove exponent just cleared
    }
    int d = targetWord.size() - 1;
    if (targetWord[d] == baseWord[d])
    {
        if(pbaseNode->isOpen())
            psetNode->setNodeType(Node::OPEN);
        else psetNode->setNodeType(Node::CLOSED);
        return;
    }
    int exp = targetWord[d];  //last exponent on targetWord
    while (exp > baseWord[d])
    {
        targetWord[d] = exp;
        psetNode = searchNodes(&targetWord);
        if (psetNode == std::list<Node>::iterator(NULL))
        {
            std::cout << "\nAttempt to set node via null iterator in preservePath()\n";
            return;
        }
        if (psetNode->getNodeType() != Node::TEMP)
        {
            //again this is not a problem
            return;
        }
        psetNode->setNodeType(Node::OPEN);
    }

    if(pbaseNode->isOpen())
        psetNode->setNodeType(Node::OPEN);
    else psetNode->setNodeType(Node::CLOSED);
    return;
}


//Simply deletes all temp nodes
void Graph::deleteTempNodes()
{
    std::list<Node>::iterator dummyIter;
    for (std::list<Node>::iterator pln = nodeList.begin();
         pln != nodeList.end(); pln++)
        if (pln->getNodeType() == Node::TEMP)
        {
            ///NOTE: possible problem here if first node in nodeList is TEMP but that should
            ///never happen since first node will always be the identity
            dummyIter = pln;
            dummyIter--;
            removeNode(pln);
            pln = dummyIter;
        }
}


//Copy edges from first node to second
void Graph::copyNodeEdges(std::list<Node>::iterator pn1, std::list<Node>::iterator pn2)
{
    std::list< std::list<Node>::iterator >::iterator pnlist;
    for (pnlist = pn1->getXinBegin(); pnlist != pn1->getXinEnd(); pnlist++)
        setXedge((*pnlist), pn2);
    for (pnlist = pn1->getYinBegin(); pnlist != pn1->getYinEnd(); pnlist++)
        setYedge((*pnlist), pn2);
    for (pnlist = pn1->getXoutBegin(); pnlist != pn1->getXoutEnd(); pnlist++)
        setXedge(pn2, (*pnlist));
    for (pnlist = pn1->getYoutBegin(); pnlist != pn1->getYoutEnd(); pnlist++)
        setYedge(pn2, (*pnlist));
}


//Clear dup. nodes pointing to passed in node, return true if such dup.s found
bool Graph::clearDuplicateNodes(std::list<Node>::iterator pn)
{
    if (pn->getSizeXin() > 1)
    {
        //choose the node you want to keep by finding the one with the least letters in its word
        std::list<Node>::iterator preservedNode = pn->getXin();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getXinBegin(); plnIter != pn->getXinEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;  //counts number of letters total in the word
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            //in this case the node you have is the identity, which you definitely want to keep
            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            //if word has less letters than the current preserved node, set it as preserved
            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        //okay so at this point the node which is to be kept has been chosen. now copy the edges from the
        //trash nodes to the preserved node and delete the trash nodes
        plnIter = pn->getXinBegin();
        while (plnIter != pn->getXinEnd())
        {
            //skip over the preserved node
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            //copy edges, remove node, set the iterator to the previous node
            std::list< std::list<Node>::iterator >::iterator prevNodeIter = plnIter;
            prevNodeIter--;
            copyNodeEdges(*plnIter, preservedNode);
            removeNode(*plnIter);
            plnIter = prevNodeIter;
        }

        return true;
    }


    //this is symmetrical to the above block
    if (pn->getSizeYin() > 1)
    {
        std::list<Node>::iterator preservedNode = pn->getYin();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getYinBegin(); plnIter != pn->getYinEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        plnIter = pn->getYinBegin();
        while (plnIter != pn->getYinEnd())
        {
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            std::list< std::list<Node>::iterator >::iterator prevNodeIter = plnIter;
            prevNodeIter--;
            copyNodeEdges(*plnIter, preservedNode);
            removeNode(*plnIter);
            plnIter = prevNodeIter;
        }

        return true;
    }


    if (pn->getSizeXout() > 1)
    {
        std::list<Node>::iterator preservedNode = pn->getXout();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getXoutBegin(); plnIter != pn->getXoutEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        plnIter = pn->getXoutBegin();
        while (plnIter != pn->getXoutEnd())
        {
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            std::list< std::list<Node>::iterator >::iterator prevNodeIter = plnIter;
            prevNodeIter--;
            copyNodeEdges(*plnIter, preservedNode);
            removeNode(*plnIter);
            plnIter = prevNodeIter;
        }

        return true;
    }


    if (pn->getSizeYout() > 1)
    {
        std::list<Node>::iterator preservedNode = pn->getYout();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getYoutBegin(); plnIter != pn->getYoutEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        plnIter = pn->getYoutBegin();
        while (plnIter != pn->getYoutEnd())
        {
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            std::list< std::list<Node>::iterator >::iterator prevNodeIter = plnIter;
            prevNodeIter--;
            copyNodeEdges(*plnIter, preservedNode);
            removeNode(*plnIter);
            plnIter = prevNodeIter;
        }

        return true;
    }


    //no duplicates found if you get to this point
    return false;
}


//Similar to clearDuplicateNodes(); inserts duplicate nodes into list instead of deleting them
void Graph::insertDuplicateNodes(std::list<Node>::iterator pn, std::list< std::list<Node>::iterator >& deleteNodes)
{
    //If there are multiple nodes in the xin list then all these nodes are identical as group elements,
    //so function will choose one to preserve and insert the rest into the given list.
    if (pn->getSizeXin() > 1)
    {
        //Select the node with the least letters to preserve.
        std::list<Node>::iterator preservedNode = pn->getXin();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getXinBegin(); plnIter != pn->getXinEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;  //counts number of letters total in the word
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            //In this case the node you have is the identity, which you definitely want to keep
            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            //If word has less letters than the current preserved node, set it as preserved
            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        //At this point the node which is to be preserved has been chosen. Now copy the edges from the
        //trash nodes to the preserved node and insert the trash nodes into the list.
        plnIter = pn->getXinBegin();
        while (plnIter != pn->getXinEnd())
        {
            //Ignore the preserved node
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            //Copy edges and add node to list
            copyNodeEdges(*plnIter, preservedNode);
            deleteNodes.push_back(*plnIter);
            plnIter++;
        }
    }


    //this is symmetrical to the above block
    if (pn->getSizeYin() > 1)
    {
        std::list<Node>::iterator preservedNode = pn->getYin();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getYinBegin(); plnIter != pn->getYinEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        plnIter = pn->getYinBegin();
        while (plnIter != pn->getYinEnd())
        {
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            copyNodeEdges(*plnIter, preservedNode);
            deleteNodes.push_back(*plnIter);
            plnIter++;
        }
    }


    if (pn->getSizeXout() > 1)
    {
        std::list<Node>::iterator preservedNode = pn->getXout();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getXoutBegin(); plnIter != pn->getXoutEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        plnIter = pn->getXoutBegin();
        while (plnIter != pn->getXoutEnd())
        {
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            copyNodeEdges(*plnIter, preservedNode);
            deleteNodes.push_back(*plnIter);
            plnIter++;
        }
    }


    if (pn->getSizeYout() > 1)
    {
        std::list<Node>::iterator preservedNode = pn->getYout();
        int presSumExp = 0;
        std::vector<short> presWord = preservedNode->getElemName();
        for (int i = 0; i < presWord.size(); i++)
            presSumExp += presWord[i];

        std::list< std::list<Node>::iterator >::iterator plnIter;
        for (plnIter = pn->getYoutBegin(); plnIter != pn->getYoutEnd(); plnIter++)
        {
            std::vector<short> word = (*plnIter)->getElemName();
            int sumExponents = 0;
            for (int i = 0; i < word.size(); i++)
                sumExponents += word[i];

            if (sumExponents == 0)
            {
                preservedNode = *plnIter;
                break;
            }

            if (sumExponents < presSumExp)
            {
                preservedNode = *plnIter;
                presSumExp = sumExponents;
            }
        }

        plnIter = pn->getYoutBegin();
        while (plnIter != pn->getYoutEnd())
        {
            if ((*plnIter) == preservedNode)
            {
                plnIter++;
                continue;
            }

            copyNodeEdges(*plnIter, preservedNode);
            deleteNodes.push_back(*plnIter);
            plnIter++;
        }
    }
}


//Find node by tracing path to given word or return pnull if not found
std::list<Node>::iterator Graph::searchPath(std::vector<short>* ptargetWord)
{
    std::vector<short> targetWord = *ptargetWord;
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    std::list<Node>::iterator pcurrentNode = nodeList.begin();  //start at the identity

    ///Ensure path begins at the identity
    std::vector<short> startWord = pcurrentNode->getElemName();
    if (startWord[0] != 0)
    {
        std::cout << "\nMISPLACED IDENTITY\n";
        return pnull;
    }

    bool letterSwitch = true; //true <-> x, false <-> y; signifies current letter in loop
    for (int i = 0; i < targetWord.size(); i++)
    {
        int letterExp = targetWord[i];
        for (int j = 1; j <= letterExp; j++)
        {
            if (letterSwitch) //if current letter is x
                pcurrentNode = pcurrentNode->getXout();
            else //if current letter is y
                pcurrentNode = pcurrentNode->getYout();

            //return pnull if path was broken
            if (pcurrentNode == pnull)
                return pnull;
        }


        //switch the letter for the next exponent
        letterSwitch = !letterSwitch;
    }

    //if you got a null iterator the function would have returned by now, so just return
    //the node you've found
    return pcurrentNode;
}


//Alt. version of buildPath()
std::list< std::list<Node>::iterator >
    Graph::buildPath2(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode)
{
    std::list< std::list<Node>::iterator > emptyPath;
    std::list< std::list<Node>::iterator > path;
    path.push_back(pbaseNode);

    //make sure iterators are valid
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (pbaseNode == pnull || ptargetNode == pnull)
    {
        std::cout << "\nFALSE BUILD2 RETURN (null iterators)";
        return path;
    }

    std::vector<short> baseWord = pbaseNode->getElemName();
    std::vector<short> targetWord = ptargetNode->getElemName();


    //target word must be longer than or equal length as base word
    if (targetWord.size() < baseWord.size())
        return emptyPath;
    //should never touch the identity; this is a special case which is handled in initial step
    if (baseWord[0] == 0 || targetWord[0] == 0)
        return emptyPath;
    //if words are identical there's nothing to do
    if (baseWord == targetWord)
        return path;

    //At this point we know the nodes exist, their words have appropriate sizes, they're not the identity,
    //and they're not equal. But we could still have the problem that the base word is not contained
    //in the leftmost portion of the target word, so we check for that.
    bool baseContained = true;
    int i;
    for (i = 0; i < baseWord.size() - 1; i++)  //up until the last letter, each exponent identical
        if (baseWord[i] != targetWord[i])
            baseContained = false;
    if (baseWord[i] > targetWord[i])  //on the last letter, the exponent in base may be less or equal
        baseContained = false;
    if (baseContained == false)
        return emptyPath;


    //Now we know that we can build a valid path from the base node to the target node.

    //this first part handles the first exponent on baseWord if it doesn't match targetWord
    std::list<Node>::iterator pcurrentNode = pbaseNode; //will be updated to point to the farthest node in the path
    if (baseWord[i] < targetWord[i])
    {
        if (i % 2 == 0) //if baseWord[i] is an exponent of x
            for (int j = baseWord[i]; j <= targetWord[i]; j++)
            {
                pcurrentNode = buildXprod(pcurrentNode);
                path.push_back(pcurrentNode);
            }
        else            //if baseWord[i] is an exponent of y
            for (int j = baseWord[i]; j <= targetWord[i]; j++)
            {
                pcurrentNode = buildYprod(pcurrentNode);
                path.push_back(pcurrentNode);
            }
    }

    //now build the path up to the last exponent of targetWord (handled specially below)
    //note: i currently points to the last letter in baseWord, so it must be incremented to start one after that
    i++;
    for (i = i; i < targetWord.size() - 1; i++)
    {
        if (i % 2 == 0) //if targetWord[i] is an exponent of x
            for (int j = 1; j <= targetWord[i]; j++)
            {
                pcurrentNode = buildXprod(pcurrentNode);
                path.push_back(pcurrentNode);
            }
        else            //if targetWord[i] is an exponent of y
            for (int j = 1; j <= targetWord[i]; j++)
            {
                pcurrentNode = buildYprod(pcurrentNode);
                path.push_back(pcurrentNode);
            }
    }

    //now build the last exponent, up to the last product. since using buildXprod, it's safe
    //to "build" the targetWord, but we won't do it anyway
    if (i % 2 == 0)
    {
        for (int j = 1; j < targetWord[i]; j++)
        {
            pcurrentNode = buildXprod(pcurrentNode);
            path.push_back(pcurrentNode);
        }

        //link final
        setXedge(pcurrentNode, ptargetNode);
    }
    else            //if targetWord[i] is an exponent of y
    {
        for (int j = 1; j < targetWord[i]; j++)
        {
            pcurrentNode = buildYprod(pcurrentNode);
            path.push_back(pcurrentNode);
        }

        //link final
        setYedge(pcurrentNode, ptargetNode);
    }

    //place final node, targetNode, at the end of the path
    path.push_back(ptargetNode);

    return path;
}


//Alt. version of reduceIdent()
Identity Graph::reduceIdent2(std::list<Identity>::iterator pid, std::list< std::list<Node>::iterator >& pidPath, bool report)
{
    std::list<Node>::iterator prightNode;
    prightNode = pid->getRight();

    //use to trace back through pidPath
    std::list< std::list<Node>::iterator >::reverse_iterator pleftPath = pidPath.rbegin();
    std::list< std::list<Node>::iterator >::reverse_iterator pleftPathPlus1 = pleftPath;
    pleftPathPlus1++;

    while (pleftPathPlus1 != pidPath.rend())
    {
        //look for pleft in pleftPlus1's xout list
        if ((*pleftPathPlus1)->isInXout(*pleftPath))
        {
            if (prightNode->isXinEmpty())
                break;
            prightNode = prightNode->getXin();
        } //look in pleftPlus1's yout list
        else if ((*pleftPathPlus1)->isInYout(*pleftPath))
        {
            if (prightNode->isYinEmpty())
                break;
            prightNode = prightNode->getYin();
        }
        else //in this case the path is not valid
        {
            std::cout << "\nBAD PATH FOR IDENTITY:\n";
            pid->printIdent();
            Identity nullid;
            return nullid;
        }

        pleftPath++;
        pleftPathPlus1++;
    }

    //make and return reduced identity;
    Identity reducedId(*pleftPath, prightNode, pid->getBase());

    if (report)
    {
        std::cout << "\nIdentity ";
        pid->printIdent();
        std::cout << "\nreduced to ";
        reducedId.printIdent();
        std::cout << "\n";
    }

    return reducedId;
}


//Alt. version of preservePath()
void Graph::preservePath2(std::list< std::list<Node>::iterator >& pidPath, std::list<Node>::iterator pendNode)
{
    std::list< std::list<Node>::iterator >::iterator pathIter = pidPath.begin();
    while (*pathIter != pendNode)
    {
        std::list<Node>::iterator pn = *pathIter;
        if (pn->isOpen())
            pn->setNodeType(Node::OPEN);
        else pn->setNodeType(Node::CLOSED);

        pathIter++;
    }

    if (pendNode->isOpen())
        pendNode->setNodeType(Node::OPEN);
    else pendNode->setNodeType(Node::CLOSED);
}


//Generate different kind of identities
void Graph::generateIdentities2(int length, int numIdents)
{
    if (length % 2 == 1) //only even lengths - easier to make identities
        length++;

    if (length <= 2) //these identities were already used in the first genIdents() function
        return;

    if (numIdents <= 0) //can't generate nonpositively many identities
        return;

    //This function generates numIdents-many identities using blocks of length-many units, raised to
    //the power of the graph's exponent, which naturally makes them equivalent to the identity.
    //Rather than forming identity objects it just sets more edges in the graph, and then
    //trusts in the user to reduce those edges later.

    short block[length]; //used to fill leftWord
    std::vector<short> leftWord; //represents the word that reduces to the identity
    std::list<Node>::iterator leftNode; //the node representing the word leftWord
    std::list<Node>::iterator ident = nodeList.begin(); //the identity of the group

    //each pass through this loop generates a new identity
    for (int i = 0; i < numIdents; i++)
    {
        //this loop fills block with random numbers in the range [1, exponent]
        for (int j = 0; j < length; j++)
            block[j] = (rand() % exponent) + 1;

        //this loop fills leftWord with exponent-many copies of block, yielding a word equal to the identity
        for (int k = 0; k < exponent; k++)
        {
            for (int f = 0; f < length; f++)
                leftWord.push_back(block[f]);
        }

        //now the node representing leftWord can be found and used to form new edges in the graph
        leftNode = searchPath(&leftWord);
        if (leftNode != std::list<Node>::iterator(NULL)) //if word found
        {
            copyNodeEdges(ident, leftNode);
            copyNodeEdges(leftNode, ident);
        }

        //clear containers for the next pass through the loop
        leftWord.clear();
    }

    //done
}
