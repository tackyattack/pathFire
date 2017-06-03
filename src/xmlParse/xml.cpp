//
//  xml.cpp
//  pathTrace
//
//  Created by HENRY BERGIN on 6/3/17.
//
//

#include "xml.h"

using namespace std;


void xmlParse::parseFile(string file_path)
{
    m_file_path = file_path;
    xmlFile.open(file_path);
    
    buildXMLTree();
}

void xmlParse::buildXMLTree()
{
    
    xmlNode *currentNode = head; // points to the tree position
    xmlNode *listNode = head; // points to the list position
    xmlNode *newNode;
    
    bool headFilled = false;
    bool currentLevelFilled = false;
    
    unsigned int refCount = 0;
    
    while(!xmlFile.eof())
    {
        // go through first level
        
        char input;
        xmlFile >> input;
        cout << input;
        
        
        if(input == '<')
        { // tag detected
            
            xmlFile >> input;
            if(input == '/')
            {
                refCount--;
            }
            else
            {
                refCount++;
            }
            
            
        }
        
        
    }
    
}