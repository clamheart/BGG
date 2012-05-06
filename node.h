#ifndef NODE_H_
#define NODE_H_
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

//Nodes are the vertices of the graph. Each node represents an element of the group. This element is contained
//in the node object. A node may be linked by an edge to other nodes. If there is an x-edge from the node A to the node B
//then A will be in B's xin list, and B will be in A's xout list; the yin and yout lists represent the same thing, but for
//y-edges rather than x-edges.

//"Multiplication" of an element g by x always, unless stated otherwise, refers to right multiplication by x; that is, gx.

class Node
{
private:
    std::vector<short> elemName;  //Stores the element in reduced form as a sequence of numbers representing the
                                  //exponent on each letter. Since the first letter is always x, it is unnecessary
                                  //to store the sequence of letters. [0] represents the identity.

    int nodeType;   //Type of node, i.e. temporary, open, or closed

    std::list< std::list<Node>::iterator > xin;   //Contains those nodes which, when multiplied by x, gives you this node;
                                                  //that is, xin holds the nodes which are the tail of an
                                                  //x-edge that ends at this node (comes IN to this node).

    std::list< std::list<Node>::iterator > yin;   //Contains those nodes which, when multiplied by y, gives you this node.

    std::list< std::list<Node>::iterator > xout;  //Contains those nodes which are the product of this node with x;
                                                  //that is, xout contains nodes which are the head of an x-edge
                                                  //that begins at this node (goes OUT from this node).

    std::list< std::list<Node>::iterator > yout;  //Contains those nodes which are the product of this node with y.

public:
    //Used to identify node type
    enum{OPEN, CLOSED, TEMP, IDENT};

    //Constructors, destructor
    Node();
    Node(std::vector<short>& elemnm, int nType = TEMP);

    //Return true if last letter is an x
    bool lastLetterIsX()
        { return (elemName.size() % 2); }

    //Set node type
    void setNodeType(int t)
    {
        if (t == OPEN || t == CLOSED || t == IDENT)
            nodeType = t;
        else nodeType = TEMP;
    }

    //Get node type
    int getNodeType()
        { return nodeType; }

    //Print word (elemname) formatted as a string
    void printWord(std::ostream& fout = std::cout) const;
    //Print unformatted word for check file
    void printWordCheck(std::ofstream& fout) const;

    //Check if node is open
    bool isOpen() const
    {
        if (xout.empty() || yout.empty())  //node is open if either its x or y product is not a preserved node
            return true;
        return false;
    }
    //Determine whether an individual list is empty
    bool isXoutEmpty() const
    {
        if (xout.empty())
            return true;
        return false;
    }
    bool isYoutEmpty() const
    {
        if (yout.empty())
            return true;
        return false;
    }
    bool isXinEmpty() const
    {
        if (xin.empty())
            return true;
        return false;
    }
    bool isYinEmpty() const
    {
        if (yin.empty())
            return true;
        return false;
    }


    //Get data
    const std::vector<short>& getElemName() const
        { return elemName; }
    std::list< std::list<Node>::iterator >::iterator getXinBegin()
        { return xin.begin(); }
    std::list< std::list<Node>::iterator >::iterator getXinEnd()
        { return xin.end(); }
    std::list< std::list<Node>::iterator >::iterator getYinBegin()
        { return yin.begin(); }
    std::list< std::list<Node>::iterator >::iterator getYinEnd()
        { return yin.end(); }
    std::list< std::list<Node>::iterator >::iterator getXoutBegin()
        { return xout.begin(); }
    std::list< std::list<Node>::iterator >::iterator getXoutEnd()
        { return xout.end(); }
    std::list< std::list<Node>::iterator >::iterator getYoutBegin()
        { return yout.begin(); }
    std::list< std::list<Node>::iterator >::iterator getYoutEnd()
        { return yout.end(); }


    //Set pointers - adds an iterator to the appropriate list
    bool setXin(std::list<Node>::iterator pn)
    {
        if (pn != std::list<Node>::iterator(NULL))
        {
            xin.push_back(pn);
            return true;
        }
        return false;
    }
    bool setYin(std::list<Node>::iterator pn)
    {
        if (pn != std::list<Node>::iterator(NULL))
        {
            yin.push_back(pn);
            return true;
        }
        return false;
    }
    bool setXout(std::list<Node>::iterator pn)
    {
        if (pn != std::list<Node>::iterator(NULL))
        {
            xout.push_back(pn);
            return true;
        }
        return false;
    }
    bool setYout(std::list<Node>::iterator pn)
    {
        if (pn != std::list<Node>::iterator(NULL))
        {
            yout.push_back(pn);
            return true;
        }
        return false;
    }


    //Remove a node from xin, xout, etc. by its value
    void removeFromXout(std::list<Node>::iterator pn)
        { xout.remove(pn); }
    void removeFromXin(std::list<Node>::iterator pn)
        { xin.remove(pn); }
    void removeFromYout(std::list<Node>::iterator pn)
        { yout.remove(pn); }
    void removeFromYin(std::list<Node>::iterator pn)
        { yin.remove(pn); }


    //Erase a node from xin, etc. by its position in the list
    void eraseFromXout(std::list< std::list<Node>::iterator >::iterator pln)
        { xout.erase(pln); }
    void eraseFromXin(std::list< std::list<Node>::iterator >::iterator pln)
        { xin.erase(pln); }
    void eraseFromYout(std::list< std::list<Node>::iterator >::iterator pln)
        { yout.erase(pln); }
    void eraseFromYin(std::list< std::list<Node>::iterator >::iterator pln)
        { yin.erase(pln); }


    //Use these get functions if you just need any iterator from the list; returns NULL if list is empty
    std::list<Node>::iterator getXin()
    {
        if (isXinEmpty())
            return std::list<Node>::iterator(NULL);
        return *(getXinBegin());
    }
    std::list<Node>::iterator getYin()
    {
        if (isYinEmpty())
            return std::list<Node>::iterator(NULL);
        return *(getYinBegin());
    }
    std::list<Node>::iterator getXout()
    {
        if (isXoutEmpty())
            return std::list<Node>::iterator(NULL);
        return *(getXoutBegin());
    }
    std::list<Node>::iterator getYout()
    {
        if (isYoutEmpty())
            return std::list<Node>::iterator(NULL);
        return *(getYoutBegin());
    }


    //Remove duplicate edges
    void clearDuplicatesXout();
    void clearDuplicatesXin();
    void clearDuplicatesYout();
    void clearDuplicatesYin();


    //Get size of xin, etc.
    int getSizeXin()
        { return xin.size(); }
    int getSizeYin()
        { return yin.size(); }
    int getSizeXout()
        { return xout.size(); }
    int getSizeYout()
        { return yout.size(); }


    //Return true if the given node is found in xout, etc;
    bool isInXout(std::list<Node>::iterator pn)
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = xout.begin(); pln != xout.end(); pln++)
            if (*pln == pn)
                return true;
        return false;
    }
    bool isInYout(std::list<Node>::iterator pn)
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = yout.begin(); pln != yout.end(); pln++)
            if (*pln == pn)
                return true;
        return false;
    }
    bool isInXin(std::list<Node>::iterator pn)
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = xin.begin(); pln != xin.end(); pln++)
            if (*pln == pn)
                return true;
        return false;
    }
    bool isInYin(std::list<Node>::iterator pn)
    {
        for (std::list< std::list<Node>::iterator >::iterator pln = yin.begin(); pln != yin.end(); pln++)
            if (*pln == pn)
                return true;
        return false;
    }
};

#endif
