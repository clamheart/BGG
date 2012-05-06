#include "node.h"
#include <vector>
#include <iostream>

//---
//Node function definitions
//---

//Constructors
Node::Node()
{
    //default word has no length; NOT the same as the identity
}

Node::Node(std::vector<short>& elemnm, int nType)
{
    for(std::vector<short>::iterator pv = elemnm.begin(); pv != elemnm.end(); pv++)
        elemName.push_back(*pv);
    nodeType = nType;
}

//Print word (elemname) as a readable string
void Node::printWord(std::ostream& fout) const
{
    if (elemName[0] == 0)  //in this case the word is the identity, which is a special case
    {
        fout << "1";
        return;
    }
    bool letterSwitchIsX = 1;   //acts as a flippable bit which tells you to print an x or a y (x = 1, y = 0)
    for(std::vector<short>::const_iterator pv = elemName.begin(); pv != elemName.end(); pv++)
    {
        if (letterSwitchIsX)  //check bool to see which letter to print
            fout << "x^";
        else fout << "y^";

        fout << *pv;  //print exponent on the selected letter

        letterSwitchIsX = !letterSwitchIsX;  //switch the bit for the next cycle
    }
}


//Unformatted output to check file
void Node::printWordCheck(std::ofstream& fout) const
{
    for (int i = 0; i < elemName.size(); i++)
        fout << elemName[i];
}


//These functions clear duplicate nodes from their respective product lists.
void Node::clearDuplicatesXout()
{
    if (xout.empty())
        return;

    std::list< std::list<Node>::iterator >::iterator pln, plnComp;
    plnComp = pln = xout.begin();
    plnComp++;
    while (pln != xout.end())
    {
        while (plnComp != xout.end())
        {
            if (*pln == *plnComp)
            {
                xout.erase(plnComp);
                plnComp = pln;
            }
            plnComp++;
        }
        pln++;
    }
}
void Node::clearDuplicatesXin()
{
    if (xin.empty())
        return;

    std::list< std::list<Node>::iterator >::iterator pln, plnComp;
    plnComp = pln = xin.begin();
    plnComp++;
    while (pln != xin.end())
    {
        while (plnComp != xin.end())
        {
            if (*pln == *plnComp)
            {
                xin.erase(plnComp);
                plnComp = pln;
            }
            plnComp++;
        }
        pln++;
    }
}
void Node::clearDuplicatesYout()
{
    if (yout.empty())
        return;

    std::list< std::list<Node>::iterator >::iterator pln, plnComp;
    plnComp = pln = yout.begin();
    plnComp++;
    while (pln != yout.end())
    {
        while (plnComp != yout.end())
        {
            if (*pln == *plnComp)
            {
                yout.erase(plnComp);
                plnComp = pln;
            }
            plnComp++;
        }
        pln++;
    }
}
void Node::clearDuplicatesYin()
{
    if (yin.empty())
        return;

    std::list< std::list<Node>::iterator >::iterator pln, plnComp;
    plnComp = pln = yin.begin();
    plnComp++;
    while (pln != yin.end())
    {
        while (plnComp != yin.end())
        {
            if (*pln == *plnComp)
            {
                yin.erase(plnComp);
                plnComp = pln;
            }
            plnComp++;
        }
        pln++;
    }
}
