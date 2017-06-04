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

void xmlParse::connectParent(xmlNode *parent, bool isHead)
{
    
    xmlNode *newNode;
    
    unsigned int refCount = 0;
    bool headFilled = false;
    
    if(isHead)
    {
      head = new xmlNode;
    }
    
    xmlNode *currentNode = head;
    xmlNode *parentNode = head;
    
    
    if(!isHead)
    {
        currentNode = parent;
        parentNode  = parent;
        headFilled = true;
    }
    
    xmlFile.seekg(parent->file_position);
    
    refCount = 0; // reset reference count
    char input;
    do
    {
        // go through first level
        
        xmlFile >> input;
        //cout << input;
        
        if(input == '<')
        { // tag detected
            // ignore other tags on next level (i.e. not 0 ref count)
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
            /*
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
             */
            
        }
        
        
    }while(refCount > 0 || (!xmlFile.eof()) );
}

void xmlParse::setTagName(xmlNode *node)
{
    
    unsigned long startingPos = xmlFile.tellg();
    node->tag = getTagName(startingPos);
}

string xmlParse::getTagName(unsigned long openStart)
{
    
    unsigned long startingPos = xmlFile.tellg();
    
    xmlFile.seekg(openStart);
    
    char tagName[256] = {0};
    char input = 0;
    char i = 0;
    
    xmlFile >> input; // read first character
    while(input != '>')
    {
        tagName[i] = input;
        xmlFile >> input;
        i++;
    }
    
    tagName[i] = '\0';
    
    string tagNameStr(tagName,i);
    
    xmlFile.seekg(startingPos);
    
    return tagNameStr;
}

unsigned long xmlParse::getNextTag(xmlNode *node)
{
    unsigned long startPos = xmlFile.tellg();
    
    xmlFile.seekg(node->file_position+1); // skip my tag symbol
    unsigned long nextTag = 0;
    
    char input = 0;
    char next = 0;
    bool done = false;
    while((input != '<') && (!done))
    {
        xmlFile >> input;
        xmlFile >> next;
        if(input == '<' && next != '/') done = true;
    }
    nextTag = xmlFile.tellg();
    nextTag = nextTag - 1;
    xmlFile.seekg(startPos); // return the file position to starting state
    return nextTag;
}

void xmlParse::connectLevel(xmlNode *parent)
{
    xmlFile.seekg(parent->file_position+1); // jump to right after the parent's open tag pos
    
    bool tagAdded = false;
    unsigned int refCount = 1;
    unsigned int ts = 0;
    char input;
    
    xmlNode *currentNode = parent;
    xmlNode *newNode = NULL;
    
    while(refCount > 0)
    {
        xmlFile >> input;
        if(input == '<')
        {
            xmlFile >> input;
            if(input == '/')
            {
                refCount--;
                //cout << "close tag" << endl;
            }
            else
            { // open tag
                refCount++;
                cout << "open tag: ";
                ts = xmlFile.tellg();
                ts = ts - 1;
                cout << getTagName(ts) << ": " << ts << endl;
                
                if (refCount == 2)
                {
                    // tag is on this level
                    tagAdded = true;
                    newNode = new xmlNode;
                    newNode->file_position = ts;
                    if(currentNode != parent)
                    {
                        currentNode->list_next = newNode; // only the children themselves are connect to each other
                                                          // the tree pointer is what connects them
                    }
                    else
                    {
                        parent->tree_next = newNode; // links to the first node in the children blob
                    }
                    currentNode = newNode; // walk forward
                    cout << "tag added" << endl;
                }
                
            }
            //cout << "ref count: " << refCount << endl;
        }
        
    }
    
    if(!tagAdded)
    {
        cout << "hit end: tag had no children" << endl; // this indicates you would add null to its children
        parent->tree_next = NULL; // the first node in the list is the next blob
    }

    
    cout << "--------------------" << endl;
    
}


void xmlParse::recursiveConnect(xmlNode *currentNode)
{
    connectParent(currentNode, false);
}

void xmlParse::connectTree(xmlNode *currentNode)
{
    char input;
    
    xmlFile.seekg(currentNode->file_position);
    
    while( input != '<')
    { // find immediate next open tag
        xmlFile >> input;
        if(xmlFile.peek() == '/') xmlFile >> input;
    }
    
        
    xmlNode *newNode = new xmlNode;
    newNode->file_position = xmlFile.tellg();
    currentNode->tree_next = newNode;
        
    
}

void xmlParse::buildXMLTree()
{
    
    head = new xmlNode;
    
    head->file_position = 1;
    cout << getNextTag(head) << endl;
    
    //setTagName(head);
    //cout << head->tag << endl;
    
    // structure:
    // connect head, then recurse down tree lists to connect
    
    connectLevel(head);
    connectLevel(head->tree_next);
    connectLevel(head->tree_next->list_next); // this is what recursion should do -- go down lists for each tree
    
    
    //connectLevel(head->list_next->list_next);
    while(1);
    
    
    //connectParent(head, false);
    
    //connectTree(head->list_next->list_next->list_next);
    
    
    
    /*
    
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
     */
    
}