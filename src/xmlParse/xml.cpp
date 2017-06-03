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
    
    head = new xmlNode;
    
    xmlNode *newNode;

    
    unsigned int refCount = 0;
    
    bool headFilled = false;
    xmlNode *currentNode = head;
    xmlNode *parentNode = head;
    refCount = 0; // reset reference count
    do
    {
        // go through first level
        
        char input;
        xmlFile >> input;
        //cout << input;
        
        if(input == '<')
        { // tag detected
            // ignore other tags on next level (i.e. not 0 ref count)
            if(refCount == 1)
            {
                cout << "open on current level" << endl;
                cout << xmlFile.tellg() << endl;
                if(!headFilled)
                {
                    head->file_position = xmlFile.tellg();
                    currentNode->parent = parentNode;
                    currentNode = head;
                    headFilled = true;
                }
                else
                {
                    newNode = new xmlNode;
                    newNode->file_position = xmlFile.tellg();
                    newNode->parent = parentNode;
                    currentNode->list_next = newNode;
                    currentNode = currentNode->list_next;
                }
            }
            
            xmlFile >> input;
            if(input == '/')
            {
                refCount--;
                cout << "close tag" << endl;
            }
            else
            {
                refCount++;
                cout << "open tag" << endl;
            }
            cout << "ref count: " << refCount << endl;
            
        }
        
        
    }while(refCount > 0);
    
}