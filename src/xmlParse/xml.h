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

class xmlParse
{
public:
    void parseFile(string file_path);
private:
    string m_file_path;
    xmlNode *head;
    ifstream xmlFile;
};

#endif /* xml_h */
