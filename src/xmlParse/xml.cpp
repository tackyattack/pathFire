//
//  xml.cpp
//  pathTrace
//
// Created by HENRY BERGIN on 6/3/17.
//
// How to use a queue to connect tree:
//
// node 1 has children of node 2 and node 3
// node 2 has children of node 4, node 5, node 6
// node 3 has children of node 7, node 8, node 9
//
// (1) ++> (2)-->(4)-->(5)-->(6)
//
// 1 ++> 2-->3
// 2 ++> 4-->5-->6
//
// node pointers:
// --> : list for children
// ++> : tree pointer
//
//
// pop (1) pushes children nodes (2), (3) into queue
// ----------------
// 1
// ----------------
//
// 1 ++> 2
// 1 ++> 3
// ----------------
// (2) (3)
// ----------------
//
// pop (2) pushes children nodes (4), (5), (6) into queue (found from 2's list (i.e. the children list))
// ----------------
// (3) (4)-->(5)-->(6)
// ----------------
//
// pop (3) pushes children nodes (7), (8), (9)
// ----------------
// (4)-->(5)-->(6) (7)-->(8)-->(9)
// ----------------

// As you can see, it's a level exploration. It's easy to first go through the file
// and find the open tags. These will connect to each other via the tree ++> pointer.
// <a>
//   <b1></b1>
//   <b2></b2>
// </a>
//
// so (a)'s tree next (++>) would be (b1) because that's on the next level and it's the first we found of that level
// then (b1)'s list next (-->) would be (b2) because that's the next tag on this current level
//
// think of tree next (++>) as the entry pointer to the next level
// think of list next (-->) as a pointer used to list all the tags of that level
//
// this is useful because you can look through all tag's of a level, then find the one you're looking for,
// go into it, and explore its tags, etc, etc. 
//

#include "xml.h"

using namespace std;


void xmlParse::parseFile(string file_path)
{
    m_file_path = file_path;
    xmlFile.open(file_path);
    
    buildXMLTree();
}

void xmlParse::setTagName(xmlNode *node)
{
    
    unsigned long startingPos = xmlFile.tellg();
    node->tag = getTagName(node->file_position);
    xmlFile.seekg(startingPos);
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
                    setTagName(newNode);
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

void xmlParse::connectTree(oQueue<xmlNode> *treeQueue)
{
    xmlNode *poppedNode;
    xmlNode *currentNode;
    
    while(treeQueue->oCount() > 0)
    {
        poppedNode = treeQueue->oPop();
        cout << "popping: " << getTagName(poppedNode->file_position) << endl;
        connectLevel(poppedNode);
        if(poppedNode->tree_next != NULL)
        {
            currentNode = poppedNode->tree_next;
            while(currentNode != NULL)
            { // push all of the children of this root node
                treeQueue->oPush(currentNode);
                cout << "pushing: " << getTagName(currentNode->file_position) << endl;
                currentNode = currentNode->list_next;
            }
        }
    }
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
    //connectLevel(head->tree_next);
    //connectLevel(head->tree_next->list_next); // this is what recursion should do -- go down lists for each tree
    
    
    //connectLevel(head->list_next->list_next);
    
    oQueue<xmlNode> buildQueue;
    buildQueue.oPush(head->tree_next);
    buildQueue.oPush(head->tree_next->list_next);
    buildQueue.oPush(head->tree_next->list_next->list_next);
    
    connectTree(&buildQueue);
    
    while(1);
    
    //------
    // queue testing
    oQueue<xmlNode> tq;
    xmlNode *testNode1;
    xmlNode *testNode2;
    xmlNode *poppedNode;
    
    testNode1->file_position = 10;
    testNode2->file_position = 11;
    
    tq.oPush(testNode1);
    tq.oPush(testNode2);
    
    poppedNode = tq.oPop();
    poppedNode = tq.oPop();
    //------
    
    
    
    
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