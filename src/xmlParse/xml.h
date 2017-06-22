//
//  xml.h
//  pathTrace
//
//  Created by HENRY BERGIN on 6/3/17.
//
//

#ifndef xml_h
#define xml_h

#include <iostream>
#include <fstream>
#include "xmlNode.h"
#include "queue.h"

class xmlParse
{
public:
    void parseFile(string file_path);
    void connectTree(oQueue<xmlNode> *treeQueue);
    void connectLevel(xmlNode *parent);
    void setTagName(xmlNode *node);
    string getTagName(unsigned long openStart);
    unsigned long getNextTag(xmlNode *node);
private:
    void buildXMLTree();
    
    string m_file_path;
    xmlNode *head;
    ifstream xmlFile;
    streampos currentPosition;
};

#endif /* xml_h */
