//
//  scene.h
//  pathTrace
//
//  Created by HENRY BERGIN on 3/2/17.
//  Holds all the data for a scene
//

#ifndef scene_h
#define scene_h

#include <iostream>
#include "3dTypes.h"

using namespace std;


struct materialNode
{

};

struct meshNode
{
    materialNode *material;
    Tri3D **triangles;
    int triangleCount = 0;
    int triangleFill  = 0;
    
    meshNode(int triCnt)
    {  // constructor
        triangleCount = triCnt;
        triangles = new Tri3D*[triangleCount]; // allocate memory for the triangle pointers
                                               // for triangles that belong to this mesh
    }
    
    void addTriangle(Tri3D *triangle)
    { // adds a triangle to the mesh node
        if(triangleFill > (triangleCount-1))
        {
            cout << "error: trying to fill past limit of mesh" << endl;
            return;
        }
        triangles[triangleFill] = triangle;
        triangleFill++; // increment fill line
    }
    
};

class scene
{
    
};

#endif /* scene_h */
