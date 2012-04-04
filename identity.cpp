#include "identity.h"
#include "node.h"
#include <iostream>
#include <fstream>
#include <list>


//Constructors
Identity::Identity()
{
    std::list<Node>::iterator pnull = std::list<Node>::iterator(NULL);
    left = right = base =  pnull;
}

Identity::Identity(std::list<Node>::iterator lft, std::list<Node>::iterator rt, std::list<Node>::iterator bn)
{
    left = lft;
    right = rt;
    base = bn;
    left->setNodeType(Node::IDENT);
}


//Output formatted identity information - STANDARD
void Identity::printIdent()
{
    if (left == std::list<Node>::iterator(NULL) || right == std::list<Node>::iterator(NULL)
                                                || base == std::list<Node>::iterator(NULL)  )
    {
        std::cout << "Null identity";
        return;
    }

    left->printWord();
    std::cout << " = ";
    right->printWord();
    std::cout << " (base: ";
    base->printWord();
    std::cout << ")";
}


//Output - TO FILE
void Identity::printIdent(std::ofstream& fout)
{
    if (left == std::list<Node>::iterator(NULL) || right == std::list<Node>::iterator(NULL)
                                                || base == std::list<Node>::iterator(NULL)  )
    {
        fout << "\nNull identity\n";
        return;
    }

    fout << "\n";
    left->printWord(fout);
    fout << " = ";
    right->printWord(fout);
    fout << " (base: ";
    base->printWord(fout);
    fout << ")\n";
}


//Output to check file
void Identity::printIdentCheck(std::ofstream& fout)
{
    left->printWordCheck(fout);
    fout << "\n";
    right->printWordCheck(fout);
    fout << "\n";
    base->printWordCheck(fout);
    fout << "\n*\n";
}
