//
//  xmlNode.h
//  
//
//  Created by HENRY BERGIN on 6/3/17.
//
//

#ifndef xmlNode_h
#define xmlNode_h

#include <iostream>
#include "list/list.h"

using namespace std;

struct xmlNode
{
    string tag;
    unsigned long file_position;
    //xmlNode *child;
    glist<xmlNode> children;
    xmlNode *parent;
    xmlNode *list_next;
    xmlNode *tree_next;
    
};

#endif /* xmlNode_h */
