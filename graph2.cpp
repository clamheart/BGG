#include "node.h"
#include "graph.h"
#include "identity.h"
#include <vector>
#include <list>
#include <iostream>

//---
//Graph function definitions, part II
//---


//Generate identities from the given open node and store them in identityBank
void Graph::generateIdentities(std::list<Node>::iterator baseNode, bool useSearchPath, int iterationCounter)
{
    //don't try to manipulate a nonexistent node
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (baseNode == pnull)
    {
        std::cout << "\nNOPEnull\n";
        return;
    }

    std::vector<short> baseWord = baseNode->getElemName();

    //don't generate identities if given node 1 (the identity)
    std::vector<short> one;
    one.push_back(0);
    if (baseNode->getElemName() == one)
    {
        std::cout << "\nNOPEidentity\n";
        return;
    }

    int letterExponent = baseWord[0];

    //handle words of length 1 and length 2 as separate cases
    if (baseWord.size() == 1)
    {
        //In this case g = x^k for some 0 < k < n. We generate identities by splitting this product
        //and forming a block (x^ry^s)^n which reduces to the identity, and sort of wedging it into g


        //would loop from 0 to letterExponent - 1, but 0 is a special case handled first
        int i = 0;
        //loop over exponent on y^r, forming an identity for each one
        for (int r = 1; r < exponent; r++)
        {
            baseWord = baseNode->getElemName();
            std::vector<short> targetWord = baseWord;
            baseWord.push_back(r); //append y^r; baseWord is now x^lEy^r

            for (int k = 1; k < exponent; k++)
            {
                baseWord.push_back(letterExponent); //append x^lE
                baseWord.push_back(r);  //append y^r; baseWord is now (x^lEy^r)^(k+1)
            }
            //baseWord is now (x^1Ey^r)^n = 1.

            //first build the left node
            Node lftNode(baseWord);
            nodeList.push_back(lftNode);
            //now form an identity with these words
            std::list<Node>::iterator plft = nodeList.end();
            plft--;
            Identity id(plft, nodeList.begin(), baseNode);  //1 is always the first closed node
            //and, finally, store it
            identityBank.push_back(id);
        }


        //now loop over i from 1 to letterExponent - 1, this time splitting x^lE
        for (i = 1; i < letterExponent; i++)
        {
            //x^i is the part separated from the first letter, so x^lE = x^(lE-i)x^i
            std::vector<short> targetWord;
            targetWord.push_back(letterExponent - i);

            //loop over exponent of y^r, forming an identity for each r
            for (int r = 1; r < exponent; r++)
            {
                baseWord = baseNode->getElemName(); //clear baseWord for the new identity
                baseWord.push_back(r);  //append y^r; baseWord is now x^lEy^r = x^(lE-i)(x^iy^r)
                for (int k = 1; k < exponent; k++)
                {
                    baseWord.push_back(i); //append x^i
                    baseWord.push_back(r);  //append y^r; baseWord is now x^(lE-i)(x^iy^r)^(k+1)
                }
                //baseWord is now x^(lE-i)(x^iy^r)^n = x^(lE-i). first build left node
                Node lftNode(baseWord);
                nodeList.push_back(lftNode);

                //now form identity
                std::list<Node>::iterator plft = nodeList.end();
                plft--;  //points to lftNode we just stuck in tempNodes
                std::list<Node>::iterator prt = searchNodes(&targetWord);  //points to RHS node
                if (prt == pnull)
                {
                    //Function will return, or attempt to use searchPath() to find the RHS node,
                    //depending on the value of the bool passed in
                    if (useSearchPath)
                    {
                        prt = searchPath(&targetWord);
                        if (prt == pnull)
                        {
                            std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                            Node dummyNode(targetWord);
                            dummyNode.printWord();
                            std::cout << "\n";
                            return;
                        }
                    }
                    else
                    {
                        std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                        Node dummyNode(targetWord);
                        dummyNode.printWord();
                        std::cout << "\n";
                        return;
                    }
                }
                Identity id(plft, prt, baseNode);
                //and, finally, store identity
                identityBank.push_back(id);
            }
        }

        return; //end of case length = 1
    }


    //case word has length 2
    if (baseWord.size() == 2)
    {
        int xletterExp = baseWord[0];
        int yletterExp = baseWord[1];

        for (int i = 1; i < xletterExp; i++)
        {
            baseWord = baseNode->getElemName(); //reset baseWord to x^(xlE)y^(ylE)
            std::vector<short> targetWord;      //reset targetWord
            targetWord.push_back(i);

            for (int j = 1; j < exponent; j++)
            {
                baseWord.push_back(xletterExp - i);
                baseWord.push_back(yletterExp);
            }
            //baseWord is now x^ix^(xlE-i)y^(ylE)(x^(xlE-i)y^(ylE))^(n-1) = x^i

            Node lftNode(baseWord);
            nodeList.push_back(lftNode);
            std::list<Node>::iterator plft = nodeList.end();
            plft--;

            std::list<Node>::iterator prt = searchNodes(&targetWord);
            if (prt == pnull)
            {

                //Function will return, or attempt to use searchPath() to find the RHS node,
                //depending on the value of the bool passed in
                if (useSearchPath)
                {
                    prt = searchPath(&targetWord);
                    if (prt == pnull)
                    {
                        std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                        Node dummyNode(targetWord);
                        dummyNode.printWord();
                        std::cout << "\n";
                        return;
                    }
                }
                else
                {
                    std::cout << "\nFAILED TO FIND RHS FOR targetWord = ";
                    Node dummyNode(targetWord);
                    dummyNode.printWord();
                    std::cout << "\n";
                    return;
                }
            }

            Identity id(plft, prt, baseNode);
            identityBank.push_back(id);
        }

        return; //end of case length 2
    }

    //
    //Here begins case length >= 3
    //

    //This determines whether the last letter is x or y (x is 1/true, y is 0/false)
    bool lastLetterIndicator = baseWord.size() % 2;

    //The exponents on the last three letters, in order from left to right
    int exp1 = baseWord[baseWord.size() - 3];
    int exp2 = baseWord[baseWord.size() - 2];
    int exp3 = baseWord[baseWord.size() - 1];

    if (exp3 < exp1)
    {
        //need to stick all but last 3 letters on targetWord
        std::vector<short> targetWord = baseWord;
        targetWord.pop_back();
        targetWord.pop_back();
        targetWord.pop_back();
        targetWord.push_back(exp1 - exp3);

        baseWord.push_back(exp2);
        for (int i = 1; i <= exponent - 2; i++)
        {
            baseWord.push_back(exp3);
            baseWord.push_back(exp2);
        }
        //words now set

        Node lftNode(baseWord);
        nodeList.push_back(lftNode);
        std::list<Node>::iterator plft = nodeList.end();
        plft--;

        std::list<Node>::iterator prt = searchNodes(&targetWord);
        if (prt == pnull)
        {
            //Function will return, or attempt to use searchPath() to find the RHS node,
            //depending on the value of the bool passed in
            if (useSearchPath)
            {
                prt = searchPath(&targetWord);
                if (prt == pnull)
                {
                    std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                    Node dummyNode(targetWord);
                    dummyNode.printWord();
                    std::cout << "\n";
                    return;
                }
            }
            else
            {
                std::cout << "\nFAILED TO FIND RHS FOR targetWord = ";
                Node dummyNode(targetWord);
                dummyNode.printWord();
                std::cout << "\n";
                return;
            }
        }

        Identity id(plft, prt, baseNode);
        identityBank.push_back(id);

    }
    else if (exp1 == exp3)
    {
        std::vector<short> startSeg = baseWord;
        startSeg.pop_back();
        startSeg.pop_back();
        startSeg.pop_back();

        //startSeg is the segment of baseWord before the last three letters. You need to
        //search this segment from right to left to find the first letter that is neither
        //r^k1 nor s^k2

        bool letterSwitch = true;
        std::vector<short>::reverse_iterator prv;
        int lettersTraversed = 3; //counts num letters from nonconf letter to end (not including nonconf)
        for (prv = startSeg.rbegin(); prv != startSeg.rend(); prv++)
        {
            if (letterSwitch)
            {
                if (*prv != exp2)
                    break;
            }
            else  //if (!letterSwitch)
            {
                if (*prv != exp1)
                    break;
            }
            lettersTraversed++;
            letterSwitch = !letterSwitch;
        }

        //if looped through whole word without finding a nonconforming letter
        if (prv == startSeg.rend())
        {
            //particulars of base word depend on last letter

            if (lastLetterIndicator) //if last letter is x
            {
                std::vector<short> targetWord = baseWord;
                int m = (targetWord.size() - 1) / 2;  //number of blocks already present
                targetWord.push_back(exp2);
                for (int i = 1; i <= exponent - m - 1; i++)
                {
                    targetWord.push_back(exp1);
                    targetWord.push_back(exp2);
                }
                //now targetWord is (x^e1y^e2)^n = 1

                Node lftNode(targetWord);
                nodeList.push_back(lftNode);
                std::list<Node>::iterator plft = nodeList.end();
                plft--;
                Identity id (plft, nodeList.begin(), baseNode);

                identityBank.push_back(id);
            }
            else
            {
                std::vector<short> targetWord = baseWord;
                int m = targetWord.size() / 2;  //number of blocks already present
                for (int i = 1; i <= exponent - m; i++)
                {
                    targetWord.push_back(exp2);
                    targetWord.push_back(exp1);
                }
                //now targetWord is (x^e2y^e1)^n = 1

                Node lftNode(targetWord);
                nodeList.push_back(lftNode);
                std::list<Node>::iterator plft = nodeList.end();
                plft--;
                Identity id (plft, nodeList.begin(), baseNode); //note: presumes first node is 1

            }
        }
        else //found a nonconforming letter, to which prv now points
        {
            //determine what letter prv points to
            int prvIndex = 1;
            //awkward use of reverse_iterator enables comparison to prv, which is a reverse iterator
            std::vector<short>::reverse_iterator pv = startSeg.rend();
            pv--;  //pv points to startSeg[0]
            for (pv = pv; pv != prv; pv--)
                prvIndex++;

            //if true, prv points to x; if false, prv points to y
            bool nonConfLetterType = prvIndex % 2;

            if (nonConfLetterType != lastLetterIndicator) //if nonconforming letter is different from last letter
            {
                //now do different things depending on exponent of nonconforming letter
                if (*prv > exp2)
                {
                    //in this case we can separate s^*prv

                    std::vector<short> rightWord;
                    std::vector<short>::reverse_iterator pv = startSeg.rend();
                    pv--;  //pv now points to startSeg[0]
                    for (pv = pv; pv != prv; pv--)
                        rightWord.push_back(*pv);
                    rightWord.push_back(*prv - exp2);

                    std::vector<short> targetWord = baseWord;
                    int m = (lettersTraversed + 1) / 2; //number of blocks already present
                    for (int i = 1; i <= exponent - m; i++)
                    {
                        targetWord.push_back(exp2);
                        targetWord.push_back(exp1);
                    }

                    Node lftNode(targetWord);
                    nodeList.push_back(lftNode);

                    std::list<Node>::iterator plft = nodeList.end();
                    plft--;
                    std::list<Node>::iterator prt = searchNodes(&rightWord);
                    if (prt == pnull)
                    {

                        //Function will return, or attempt to use searchPath() to find the RHS node,
                        //depending on the value of the bool passed in
                        if (useSearchPath)
                        {
                            prt = searchPath(&rightWord);
                            if (prt == pnull)
                            {
                                std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                                Node dummyNode(rightWord);
                                dummyNode.printWord();
                                std::cout << "\n";
                                return;
                            }
                        }
                        else
                        {
                            std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                            Node dummyNode(rightWord);
                            dummyNode.printWord();
                            std::cout << "\n";
                            return;
                        }
                    }

                    Identity id (plft, prt, baseNode);
                    identityBank.push_back(id);

                }
                else //if *prv < exp2; note that it cannot be equal, as that would put it in a previous case
                {
                    //in this case we cannot separate s^*prv so we leave it. note block is inverted

                    std::vector<short> rightWord;
                    std::vector<short>::reverse_iterator pv = startSeg.rend();
                    pv--;
                    for (pv = pv; pv != prv; pv--)
                        rightWord.push_back(*pv);
                    rightWord.push_back(*prv);

                    std::vector<short> targetWord = baseWord;
                    int m = (lettersTraversed - 1) / 2; //number of blocks already present
                    targetWord.push_back(exp2);
                    for (int i = 1; i <= exponent - m - 1; i++)
                    {
                        targetWord.push_back(exp1);
                        targetWord.push_back(exp2);
                    }

                    Node lftNode(targetWord);
                    nodeList.push_back(lftNode);

                    std::list<Node>::iterator plft = nodeList.end();
                    plft--;
                    std::list<Node>::iterator prt = searchNodes(&rightWord);
                    if (prt == pnull)
                    {

                        //Function will return, or attempt to use searchPath() to find the RHS node,
                        //depending on the value of the bool passed in
                        if (useSearchPath)
                        {
                            prt = searchPath(&rightWord);
                            if (prt == pnull)
                            {
                                std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                                Node dummyNode(rightWord);
                                dummyNode.printWord();
                                std::cout << "\n";
                                return;
                            }
                        }
                        else
                        {
                            std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                            Node dummyNode(rightWord);
                            dummyNode.printWord();
                            std::cout << "\n";
                            return;
                        }
                    }

                    Identity id (plft, prt, baseNode);
                    identityBank.push_back(id);

                }
            }
            else //if nonconforming letter is the same as last letter
            {
                if (*prv > exp1)
                {
                    //in this case we can separate sr^*prv

                    std::vector<short> rightWord;
                    std::vector<short>::reverse_iterator pv = startSeg.rend();
                    pv--;
                    for (pv = pv; pv != prv; pv--)
                        rightWord.push_back(*pv);
                    rightWord.push_back(*prv - exp1);

                    std::vector<short> targetWord = baseWord;
                    int m = lettersTraversed / 2; //number of blocks already present
                    ///baseWord.push_back(exp2); Replaced by line below
                    targetWord.push_back(exp2);
                    for (int i = 1; i <= exponent - m - 1; i++)
                    {
                        targetWord.push_back(exp1);
                        targetWord.push_back(exp2);
                    }

                    Node lftNode(targetWord);
                    nodeList.push_back(lftNode);

                    std::list<Node>::iterator plft = nodeList.end();
                    plft--;
                    std::list<Node>::iterator prt = searchNodes(&rightWord);
                    //Function will return, or attempt to use searchPath() to find the RHS node,
                    //depending on the value of the bool passed in
                    if (useSearchPath)
                    {
                        prt = searchPath(&rightWord);
                        if (prt == pnull)
                        {
                            std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                            Node dummyNode(rightWord);
                            dummyNode.printWord();
                            std::cout << "\n";
                            return;
                        }
                    }
                    else
                    {
                        std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                        Node dummyNode(rightWord);
                        dummyNode.printWord();
                        std::cout << "\n";
                        return;
                    }

                    Identity id (plft, prt, baseNode);
                    identityBank.push_back(id);

                }
                else //if *prv < exp1; note that it cannot be equal, as that would put it in a previous case
                {
                    //in this case we cannot separate r^*prv so we leave it. note block is inverted

                    std::vector<short> rightWord;
                    std::vector<short>::reverse_iterator pv = startSeg.rend();
                    pv--;
                    for (pv = pv; pv != prv; pv--)
                        rightWord.push_back(*pv);
                    rightWord.push_back(*prv);

                    std::vector<short> targetWord = baseWord;
                    int m = lettersTraversed / 2; //number of blocks already present
                    for (int i = 1; i <= exponent - m; i++)
                    {
                        targetWord.push_back(exp2);
                        targetWord.push_back(exp1);
                    }

                    Node lftNode(targetWord);
                    nodeList.push_back(lftNode);

                    std::list<Node>::iterator plft = nodeList.end();
                    plft--;
                    std::list<Node>::iterator prt = searchNodes(&rightWord);
                    //Function will return, or attempt to use searchPath() to find the RHS node,
                    //depending on the value of the bool passed in
                    if (useSearchPath)
                    {
                        prt = searchPath(&rightWord);
                        if (prt == pnull)
                        {
                            std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                            Node dummyNode(rightWord);
                            dummyNode.printWord();
                            std::cout << "\n";
                            return;
                        }
                    }
                    else
                    {
                        std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                        Node dummyNode(rightWord);
                        dummyNode.printWord();
                        std::cout << "\n";
                        return;
                    }

                    Identity id (plft, prt, baseNode);
                    identityBank.push_back(id);

                }

            }
        }
    }
    else //if exp1 < exp3
    {
        //separate s^exp2 into s^(exp2-i)s^i


        //handle i=0 as special case
        std::vector<short> rightWord = baseWord;
        rightWord.pop_back();
        rightWord.pop_back();

        std::vector<short> targetWord = baseWord;
        for (int j = 1; j <= exponent - 1; j++)
        {
            targetWord.push_back(exp2);
            targetWord.push_back(exp3);
        }

        Node lftNode(targetWord);
        nodeList.push_back(lftNode);

        std::list<Node>::iterator plft = nodeList.end();
        plft--;
        std::list<Node>::iterator prt = searchNodes(&rightWord);
        if (prt == pnull)
        {

            //Function will return, or attempt to use searchPath() to find the RHS node,
            //depending on the value of the bool passed in
            if (useSearchPath)
            {
                prt = searchPath(&rightWord);
                if (prt == pnull)
                {
                    std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                    Node dummyNode(rightWord);
                    dummyNode.printWord();
                    std::cout << "\n";
                    return;
                }
            }
            else
            {
                std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                Node dummyNode(rightWord);
                dummyNode.printWord();
                std::cout << "\n";
                return;
            }
        }

        Identity id (plft, prt, baseNode);
        identityBank.push_back(id);

        //now do i >= 1
        for (int i = 1; i < exp2; i++)
        {
            std::vector<short> rightWord = baseWord;
            rightWord.pop_back();
            rightWord.pop_back();
            rightWord.push_back(exp2 - i);

            std::vector<short> targetWord = baseWord;
            for (int j = 1; j <= exponent - 1; j++)
            {
                targetWord.push_back(i);
                targetWord.push_back(exp3);
            }

            Node lftNode(targetWord);
            nodeList.push_back(lftNode);

            std::list<Node>::iterator plft = nodeList.end();
            plft--;
            std::list<Node>::iterator prt = searchNodes(&rightWord);
            if (prt == pnull)
            {
                //Function will return, or attempt to use searchPath() to find the RHS node,
                //depending on the value of the bool passed in
                if (useSearchPath)
                {
                    prt = searchPath(&rightWord);
                    if (prt == pnull)
                    {
                        std::cout << "\nFAILED TO FIND RHS PATH FOR targetWord = ";
                        Node dummyNode(rightWord);
                        dummyNode.printWord();
                        std::cout << "\n";
                        return;
                    }
                }
                else
                {
                    std::cout << "\nFAILED TO FIND RHS FOR rightWord = ";
                    Node dummyNode(rightWord);
                    dummyNode.printWord();
                    std::cout << "\n";
                    return;
                }
            }

            Identity id (plft, prt, baseNode);
            identityBank.push_back(id);
        }
    }
}


//Looks at each temporary node and moves it either to closedNodes or openNodes
void Graph::sortTempNodes()
{
    std::list<Node>::iterator ptemp = nodeList.begin();
    while (ptemp != nodeList.end())
    {
        if (ptemp->isOpen())
            ptemp->setNodeType(Node::OPEN);
        else ptemp->setNodeType(Node::CLOSED);
    }
}


//Builds a path, letter by letter, from baseNode to targetNode
bool Graph::buildPath(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode)
{
    //make sure iterators are valid
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (pbaseNode == pnull || ptargetNode == pnull)
    {
        std::cout << "\nFALSE BUILD RETURN line 603";
        return false;
    }

    std::vector<short> baseWord = pbaseNode->getElemName();
    std::vector<short> targetWord = ptargetNode->getElemName();

    /*
    std::cout << "\nbuildPath() REPORT\n";
    pbaseNode->printWord();
    std::cout << " to ";
    ptargetNode->printWord();
    std::cout << "\n";
    */

    //target word must be longer than or equal length as base word
    if (targetWord.size() < baseWord.size())
        return false;
    //should never touch the identity; this is a special case which is handled in initial step
    if (baseWord[0] == 0 || targetWord[0] == 0)
        return false;
    //if words are identical there's nothing to do
    if (baseWord == targetWord)
        return true;

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
        return false;

    //Now we know that we can build a valid path from the base node to the target node.

    //this first part handles the first exponent on baseWord if it doesn't match targetWord
    std::list<Node>::iterator pcurrentNode = pbaseNode; //will be updated to point to the farthest node in the path
    if (baseWord[i] < targetWord[i])
    {
        if (i % 2 == 0) //if baseWord[i] is an exponent of x
            for (int j = baseWord[i]; j <= targetWord[i]; j++)
            {
                ///TEST
                pcurrentNode = buildXprodTemp(pcurrentNode);
                //pcurrentNode = buildXprod(pcurrentNode);
                //std::cout << "Built ";
                //pcurrentNode->printWord();
                //std::cout << "\n";
            }
        else            //if baseWord[i] is an exponent of y
            for (int j = baseWord[i]; j <= targetWord[i]; j++)
            {
                ///TEST
                pcurrentNode = buildYprodTemp(pcurrentNode);
                //pcurrentNode = buildYprod(pcurrentNode);
                //std::cout << "Built ";
                //pcurrentNode->printWord();
                //std::cout << "\n";
            }
    }

    //now build the path up to the last exponent of targetWord (handled specially below)
    //note: i currently points to the last letter in baseWord, so it must be incremented to start one after that
    for (i = i + 1; i < targetWord.size() - 1; i++)
    {
        if (i % 2 == 0) //if targetWord[i] is an exponent of x
            for (int j = 1; j <= targetWord[i]; j++)
            {
                ///TEST
                pcurrentNode = buildXprodTemp(pcurrentNode);
                //pcurrentNode = buildXprod(pcurrentNode);
                //std::cout << "Built ";
                //pcurrentNode->printWord();
                //std::cout << "\n";
            }
        else            //if targetWord[i] is an exponent of y
            for (int j = 1; j <= targetWord[i]; j++)
            {
                ///TEST
                pcurrentNode = buildYprodTemp(pcurrentNode);
                //pcurrentNode = buildYprod(pcurrentNode);
                //std::cout << "Built ";
                //pcurrentNode->printWord();
                //std::cout << "\n";
            }
    }

    //now build the last exponent, up to the last product. don't build this because it's the target
    //word, which we know is already an extant node
    if (i % 2 == 0)
    {
        for (int j = 1; j < targetWord[i]; j++)
        {
            ///TEST
            pcurrentNode = buildXprodTemp(pcurrentNode);
            //pcurrentNode = buildXprod(pcurrentNode);
            //std::cout << "Built ";
            //pcurrentNode->printWord();
            //std::cout << "\n";
        }

        //link final
        setXedge(pcurrentNode, ptargetNode);
    }
    else            //if targetWord[i] is an exponent of y
    {
        for (int j = 1; j < targetWord[i]; j++)
        {
            ///TEST
            pcurrentNode = buildYprodTemp(pcurrentNode);
            //pcurrentNode = buildYprod(pcurrentNode);
            //std::cout << "Built ";
            //pcurrentNode->printWord();
            //std::cout << "\n";
        }

        //link final
        setYedge(pcurrentNode, ptargetNode);
    }

    return true;
}


//destroy path from base node to target node, if they fit the same assumptions as in  buildPath()
bool Graph::destroyPath(std::list<Node>::iterator pbaseNode, std::list<Node>::iterator ptargetNode)
{
    //make sure iterators are valid
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (pbaseNode == pnull || ptargetNode == pnull)
        return false;

    std::vector<short> baseWord = pbaseNode->getElemName();
    std::vector<short> targetWord = ptargetNode->getElemName();

    //target word must be longer than or equal length as base word
    if (targetWord.size() < baseWord.size())
        return false;
    //should never touch the identity
    if (baseWord[0] == 0 || targetWord[0] == 0)
        return false;
    //if words are identical, function prioritizes keeping the base node over destroying the target node,
    //so it does nothing and returns true (because path is destroyed, sort of)
    if (baseWord == targetWord)
        return true;

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
        return false;

    //Path is valid and length > 1, so begin deletion
    /*
    std::cout << "\ndestroyPath() REPORT:\n";
    pbaseNode->printWord();
    std::cout << " to ";
    ptargetNode->printWord();
    std::cout << "\n";
    */

    /*
    std::list<Node>::iterator pdelNode, pnextNode;

    //two cases depending on letter of last exponent
    bool isBase = false;
    if (i % 2 == 0) //if last exponent is on x
    {
        if (baseWord[i] < targetWord[i])
        {
            //handle first exponent
            pdelNode = pbaseNode->getXout();
            //j is the exponent being deleted during the loop
            for (int j = baseWord[i] + 1; j < targetWord[i]; j++)  //start 1 past base, stop at target
            {
                pnextNode = pdelNode->getXout();
                std::cout << "About to destroy: ";
                pdelNode->printWord();
                std::cout << "\n";
                removeNode(pdelNode, TEMP);
                pdelNode = pnextNode;
            }
            //now all but last exponent removed at which point letter switches
        }
        else
        {
            pdelNode = pnextNode = pbaseNode;
            isBase = true; //ensures base is not deleted
        }
        //pdel and pnext both point to the last exponent

        //loop over remaining exponents
        bool letterSwitch = false; //x <-> true, y <-> false; initialized to y
        for (int j = baseWord.size(); j < targetWord.size(); j++)
        {
            if (letterSwitch) //if letterSwitch says x
            {
                //then you're transitioning to a path of x-edges to delete
                //first, delete transition node (unless it's the base)
                pnextNode = pdelNode->getXout();
                if (!isBase)
                {
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                else  //move up without deletion if you're pointing to the base
                {
                    pdelNode = pnextNode;
                    isBase = false;
                }
                //now delete remaining nodes in this exponent except the last one
                for (int k = 1; k < targetWord[j]; k++)
                {
                    pnextNode = pdelNode->getXout();
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                //pdel and pnext now point to the next transition node
            }
            else //if letterSwitch says y
            {
                //then you're transitioning to a path of y-edges to delete
                //first, delete transition node (unless it's the base)
                pnextNode = pdelNode->getYout();
                if (!isBase)
                {
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                else  //move up without deletion if you're pointing to the base
                {
                    pdelNode = pnextNode;
                    isBase = false;
                }
                //now delete remaining nodes in this exponent except the last one
                for (int k = 1; k < targetWord[j]; k++)
                {
                    pnextNode = pdelNode->getYout();
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                //pdel and pnext now point to the next transition node
            }
            letterSwitch = !letterSwitch;  //flip letterSwitch for next exponent
        }

        //at this point all nodes have been deleted except targetNode, to which pdel points
        std::cout << "About to destroy: ";
        pdelNode->printWord();
        std::cout << "\n";
        removeNode(pdelNode, TEMP);
        return true;
    }
    else //if last exponent is on y
    {
        if (baseWord[i] < targetWord[i])
        {
            //handle first exponent
            pdelNode = pbaseNode->getYout();
            //j is the exponent being deleted during the loop
            for (int j = baseWord[i] + 1; j < targetWord[i]; j++)  //start 1 past base, stop at target
            {
                pnextNode = pdelNode->getYout();
                std::cout << "About to destroy: ";
                pdelNode->printWord();
                std::cout << "\n";
                removeNode(pdelNode, TEMP);
                pdelNode = pnextNode;
            }
            //now all but last exponent removed at which point letter switches
        }
        else
        {
            pdelNode = pnextNode = pbaseNode;
            isBase = true;
        }
        //pdel and pnext both point to the last exponent

        //loop over remaining exponents
        bool letterSwitch = true; //x <-> true, y <-> false; initialized to x
        for (int j = baseWord.size(); j < targetWord.size(); j++)
        {
            if (letterSwitch) //if letterSwitch says x
            {
                //then you're transitioning to a path of x-edges to delete
                //first, delete transition node
                pnextNode = pdelNode->getXout();
                int k = 1;
                if (!isBase)
                {
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                else  //move up without deletion if you're pointing to the base
                {
                    pdelNode = pnextNode;
                    k++;
                    isBase = false;
                }
                //now delete remaining nodes in this exponent except the last one
                for (k = k; k < targetWord[j]; k++)
                {
                    pnextNode = pdelNode->getXout();
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                //pdel and pnext now point to the next transition node
            }
            else //if letterSwitch says y
            {
                //then you're transitioning to a path of y-edges to delete
                //first, delete transition node
                pnextNode = pdelNode->getYout();
                int k = 1;
                if (!isBase)
                {
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                else  //move up without deletion if you're pointing to the base
                {
                    pdelNode = pnextNode;
                    k++;
                    isBase = false;
                }
                //now delete remaining nodes in this exponent except the last one
                for (k = k; k < targetWord[j]; k++)
                {
                    pnextNode = pdelNode->getYout();
                    std::cout << "About to destroy: ";
                    pdelNode->printWord();
                    std::cout << "\n";
                    removeNode(pdelNode, TEMP);
                    pdelNode = pnextNode;
                }
                //pdel and pnext now point to the next transition node
            }
            letterSwitch = !letterSwitch;  //flip letterSwitch for next exponent
        }

        //at this point all nodes have been deleted except targetNode, to which pdel points
        std::cout << "About to destroy: ";
        pdelNode->printWord();
        std::cout << "\n";
        removeNode(pdelNode, TEMP);
        return true;
    }
    */
    ///New (test; tentatively accepted) implementation that searches for nodes to delete

    std::list<Node>::iterator pdelNode = ptargetNode;
    while (targetWord.size() != baseWord.size()) //stop at last exponent of baseWord
    {
        int i = targetWord.size() - 1;  //always index of last exponent in targetWord
        int exp = targetWord[i];  //last exponent on targetWord
        while (exp > 0)
        {
            targetWord[i] = exp;
            pdelNode = searchNodes(&targetWord);
            if (pdelNode == std::list<Node>::iterator(NULL))
            {
                std::cout << "\nAttempt to delete node via null iterator in deletePath()\n";
                return false;
            }
            if (pdelNode->getNodeType() != Node::TEMP)
            {
                std::cout << "\nAttempt to delete preserved node in deletePath()\n";
                return false;
            }
            ///REMOVE
                //std::cout << "\nAbout to delete:\n";
                //pdelNode->printWord();
            removeNode(pdelNode);
            exp--;
        }
        targetWord.pop_back(); //remove exponent just cleared
    }
    int d = targetWord.size() - 1;
    if (targetWord[d] == baseWord[d])
    {
        /*//***Alternate definition here will also delete base node
        pdelNode = searchOpenNodes(&targetWord);
        std::cout << "\nAbout to delete:\n";
        pdelNode->printWord();
        removeNode(pdelNode, TEMP);
        ///***/
        return true;
    }
    int exp = targetWord[d];  //last exponent on targetWord
    while (exp > baseWord[d])
    {
        targetWord[d] = exp;
        pdelNode = searchNodes(&targetWord);
        if (pdelNode == std::list<Node>::iterator(NULL))
        {
            std::cout << "\nAttempt to delete node via null iterator\n";
            return false;
        }
        if (pdelNode->getNodeType() != Node::TEMP)
        {
            std::cout << "\nAttempt to delete preserved node in deletePath()\n";
            return false;
        }
        ///REMOVE
                //std::cout << "\nAbout to delete:\n";
                //pdelNode->printWord();
        removeNode(pdelNode);
        exp--;
    }

    return true;
}


//Erase duplicate identities, destroy associated temp node
void Graph::clearDuplicateIdentities()
{
    if (identityBank.empty())
        return;
    std::list<Identity>::iterator pdummy;
    std::list<Identity>::iterator pcomp = identityBank.begin();
    pcomp++;
    for (std::list<Identity>::iterator pid = identityBank.begin(); pid != identityBank.end(); pid++)
        for (pcomp = pcomp; pcomp != identityBank.end(); pcomp++)
            if (pid->getLeft() == pcomp->getLeft())
            {
                pdummy = pcomp;
                pdummy--;
                //std::cout << "\nIDENT ERASED: ";
                //pcomp->printIdent();
                //std::cout << "\n";
                removeNode(pcomp->getLeft()); //destroy node
                identityBank.erase(pcomp);   //erase identity
                pcomp = pdummy;  //move pcomp back to search the rest of the list
            }
}


//Clear all nodes of duplicate edges
void Graph::clearDuplicateEdges()
{
    for (std::list<Node>::iterator pn = nodeList.begin(); pn != nodeList.end(); pn++)
    {
        pn->clearDuplicatesXout();
        pn->clearDuplicatesXin();
        pn->clearDuplicatesYout();
        pn->clearDuplicatesYin();
    }
}


//Builds new nodes and puts them in temp
std::list<Node>::iterator Graph::buildXprodTemp(std::list<Node>::iterator pn)
{
    ///New test code will search for correct word
    /*
    //if an x product is already defined in the node then return immediately
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    if (!pn->isXoutEmpty())
        return *(pn->getXoutBegin());
    */

    std::vector<short> originalWord = pn->getElemName();
    std::vector<short> newWord = originalWord;

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
            if (letterExponent == exponent - 2) //can form a special edge
                specialEdge = true;

            //increment the last letter
            newWord[newWord.size() - 1] = newWord[newWord.size() - 1] + 1;
        }
    }
    else //in this case word ends in y
    {
        //just stick an x^1 on the end of the word
        newWord.push_back(1);
    }


    ///TEST - search for newWord first
    std::list<Node>::iterator psearchResult = searchNodes(&newWord);
    if (psearchResult != std::list<Node>::iterator(NULL))
    {
        setXedge(pn, psearchResult);
        return psearchResult;
    }
    ///TEST


    //make new node
    Node newNode(newWord);
    nodeList.push_back(newNode);
    std::list<Node>::iterator pnewNode = nodeList.end();
    pnewNode--;
    //now pnewNode points to the x-product, which has been placed in tempNodes
    setXedge(pn, pnewNode);

    if (specialEdge)
    {
        //form x-edge (bx^(n-1), b)
        std::vector<short> specialWord = originalWord;
        specialWord.pop_back();
        std::list<Node>::iterator pspecialNode = searchNodes(&specialWord);
        setXedge(pnewNode, pspecialNode);
    }

    return pnewNode;
}

std::list<Node>::iterator Graph::buildYprodTemp(std::list<Node>::iterator pn)
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

    Node newNode(newWord);
    nodeList.push_back(newNode);
    std::list<Node>::iterator pnewNode = nodeList.end();
    pnewNode--;
    setYedge(pn, pnewNode);

    if (specialEdge)
    {
        //form x-edge (bx^(n-1), b)
        std::vector<short> specialWord = originalWord;
        specialWord.pop_back();
        std::list<Node>::iterator pspecialNode = searchNodes(&specialWord);
        setYedge(pnewNode, pspecialNode);
    }

    return pnewNode;
}
