//
//  list.h
//  pathTrace
//
//  Created by HENRY BERGIN on 6/3/17.
//
//

#ifndef list_h
#define list_h

template <typename listObject>
struct node
{
    listObject *object;
    node *next;
};

template <typename listObject>
class glist
{
    
public:
    void insertObject(listObject *obj)
    {
        //-----create the new node-----
        node<listObject> *list_node = new node<listObject>;
        list_node->next = NULL;
        list_node->object = obj;
        //-----------------------------
        
        if(count == 0)
        { // list is empty
            head = list_node;
            tail = list_node;
        }
        else
        { // link in the new node
            tail->next = list_node;
            tail = list_node;
        }
        
        count++; // object was added
    }
    listObject &removeTriangle(listObject &obj);
private:
    node<listObject> *head;
    node<listObject> *tail;
    unsigned long count = 0;
     
};


#endif /* list_h */
