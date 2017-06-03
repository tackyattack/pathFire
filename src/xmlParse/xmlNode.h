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
    unsigned int file_position;
    xmlNode *child;
};

#endif /* xmlNode_h */
