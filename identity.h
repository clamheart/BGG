#ifndef IDENTITY_H_
#define IDENTITY_H_
#include <list>     //contains two std::list<Node>::iterators
#include <iostream>
#include <fstream>
#include "node.h"

class Identity
{
private:

    std::list<Node>::iterator left;   //Corresponding to the left-hand side of the identity, which
                                     //has some special properties (e.g. begins with the base open node)

    std::list<Node>::iterator right;  //Corresponding to the right-hand side of the identity; the two sides
                                     //must meet different conditions to determine when reduced

    std::list<Node>::iterator base;  //Indicates the open node used as a base in forming the identity

public:

    //Constructors
    Identity();
    Identity(std::list<Node>::iterator lft, std::list<Node>::iterator rt, std::list<Node>::iterator bn);

    //Get data
    std::list<Node>::iterator getLeft()
        { return left; }
    std::list<Node>::iterator getRight()
        { return right; }
    std::list<Node>::iterator getBase()
        { return base; }

    //Set data
    void setLeft(std::list<Node>::iterator lft)
        { left = lft; }
    void setRight(std::list<Node>::iterator rt)
        { right = rt; }
    void setBase(std::list<Node>::iterator bn)
        { right = bn; }

    //Output formatted identity - STANDARD output
    void printIdent();

    //Output formatted dentity - TO FILE
    void printIdent(std::ofstream& fout);

    //Output formatted for verification - TO FILE
    void printIdentCheck(std::ofstream& fout);
};

#endif
