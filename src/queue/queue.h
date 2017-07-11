//
//  queue.h
//  pathTrace
//
//  Created by HENRY BERGIN on 6/4/17.
//
//

#ifndef queue_h
#define queue_h

template <typename queueObject>
struct queueNode
{
    queueNode *m_next;
    queueObject *m_object;
};

template <typename queueObject>
class oQueue
{
public:
    void oPush(queueObject *object)
    {
        queueNode<queueObject> *newNode = new queueNode<queueObject>;
        newNode->m_object = object;
        
        if(count < 1)
        {
            head = newNode;
            tail = newNode;
            count++;
        }
        else
        {
            tail->m_next = newNode;
            tail = newNode;
            count++;
        }
        
    }
    queueObject *oPop()
    {
        queueNode<queueObject> *toPop = head;
        queueObject *ret = toPop->m_object;
        head = head->m_next;
        delete toPop;
        count--;
        return ret;
        
    }
    unsigned long oCount()
    {
        return count;
    }
private:
    
    queueNode<queueObject> *head;
    queueNode<queueObject> *tail;
    unsigned long count = 0;
};

#endif /* queue_h */
