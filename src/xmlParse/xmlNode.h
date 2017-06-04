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

using namespace std;

struct xmlNode
{
    string tag;
    unsigned long file_position = 0;
    xmlNode *parent = NULL;
    xmlNode *list_next = NULL;
    xmlNode *tree_next = NULL;
    
};

#endif /* xmlNode_h */
