//
//  bvh.h
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//  Creates a BVH tree from triagle list
//

#ifndef bvh_h
#define bvh_h

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include "3dTypes.h"

#define intersectBias 0.0001

using namespace std;

struct triangleNode
{
    Tri3D *tri;
    triangleNode *next;
};

struct triangleList
{
    void insertTriangle(Tri3D &tri)
    {
        //-----create the new node-----
        triangleNode *tri_node = new triangleNode;
        tri_node->next = NULL;
        tri_node->tri = &tri;
        //-----------------------------
        
        if(count == 0)
        { // list is empty
            head = tri_node;
            tail = tri_node;
        }
        else
        { // link in the new node
            tail->next = tri_node;
            tail = tri_node;
        }
        
        count++; // triangle was added
    }
    
    Tri3D &removeTriangle(Tri3D &tri)
    {
        Tri3D *return_tri;
        triangleNode *current;
        triangleNode *next;
        triangleNode *prev;
        bool found = false;
        if(count == 0)
        { // list is empty
            cout << "error: trying to remove from empty list" << endl;
        }
        else
        { // take out the triangle with the address that was passed in
            next = head;
            while((!found) && (next != NULL))
            {
                prev = current;
                current = next;
                next = current->next;
                
                if(current->tri == &tri) found = true;
            }
        }
        
        if(!found) cout << "error: triangle not found" << endl;
        if(found)
        {
            return_tri = current->tri;
            count--;
            
            prev->next = current->next;
            
            delete current;
        }
        
        return *return_tri; // return the triangle that was pulled off the list
        
    }
    
    Tri3D &removeHead()
    {
        Tri3D *return_tri;
        triangleNode *current;
        bool found = false;
        if(count == 0)
        { // list is empty
            cout << "error: trying to remove from empty list" << endl;
        }
        else
        { // take out the triangle with the address that was passed in
            current = head;
            return_tri = current->tri;
            head = head->next;
            delete current;
            count--;
        }
        
        return *return_tri; // return the triangle that was pulled off the list
        
    }
    
    triangleNode *head;
    triangleNode *tail;
    unsigned long count = 0;
};

struct bvhNode
{
    float m_boundx1;
    float m_boundy1;
    float m_boundz1;
    float m_boundx2;
    float m_boundy2;
    float m_boundz2;
    aabb box;
    triangleList *tri_list;
    int m_tri_cnt;
    bvhNode *leftChild = NULL;
    bvhNode *rightChild = NULL;
    bvhNode()
    {
        m_boundx1 = 0;
        m_boundy1 = 0;
        m_boundz1 = 0;
        m_boundx2 = 0;
        m_boundy2 = 0;
        m_boundz2 = 0;
        
        m_tri_cnt = 0;
    }
};


struct rayNode
{
    bvhNode *m_node;
    float m_t;
    rayNode *m_next;
    rayNode(bvhNode &node, float t)
    {
        m_t = t;
        m_node = &node;
    }
};
struct rayList
{
    rayNode *front = NULL;
    bool empty = true;
    void insertRayNode(rayNode *newNode)
    {
        
        if(front == NULL)
        { // empty at first
            front = newNode;
            newNode->m_next = NULL;
            empty = false;
            return;
        }
        
        rayNode *previous = front;
        rayNode *current = front;
        while(current->m_next != NULL && newNode->m_t > current->m_t)
        {
            previous = current;
            current = current->m_next;
        }
        if(current == NULL)
        { // we're at the end of the ray list
            
            current->m_next = newNode;
            newNode->m_next = NULL;
        }
        else if(current == front)
        { // we're at the front so push it in
            
            if(newNode->m_t < current->m_t)
            {
                newNode->m_next = current;
                front = newNode;
            }
            else
            {
                newNode->m_next = current->m_next;
                current->m_next = newNode;
            }
        }
        else
        { // somewhere inside the list and t less than current
            newNode->m_next = current->m_next;
            current->m_next = newNode;
        }
    }
    ~rayList()
    {
        //delete nodes
        rayNode *current = front;
        rayNode *next = front;
        while(next != NULL)
        {
            current = next;
            next = next->m_next;
            delete current;
        }
    }
};


class bvhTree
{
public:
    bvhNode *root;
    Tri3D *m_tris; 
    bvhTree(Tri3D tris[], int tris_cnt);
    bool intersect(aabb box, ray r, float &tnear);
    void testIntersect(bvhNode &root, ray r, float t, rayList &list);
    bool rayTriangleIntersectMT(ray r, Tri3D tri, float &t, float &u, float &v);
    bool testRay(ray r, float &t, Tri3D &tri);
    void walkBB(bvhNode &root, ray r, Tri3D *&trianlgeFound, float &tClose);
    bool newTest(ray r, float &t, Tri3D *&tri);
private:
    void splitTree(bvhNode &root, int dp);
    int getLongestAxis(aabb aabb_in);
    aabb getSplitBox(Tri3D tris_in[], int tris_cnt);
    aabb getSplitBox(triangleList &list);
    point3D triCentroid(Tri3D tri);
};

#endif /* bvh_h */
