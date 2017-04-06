//
//  bvh.cpp
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//
//

#include "bvh.h"
#include <ctime>

bool bvhTree::rayTriangleIntersectMT(ray r, Tri3D tri, float &t, float &u, float &v)
{

    
    vec3d nv3 = (tri.m_b - tri.m_a).cross(tri.m_c - tri.m_a);

    vec3d v0 = tri.m_a;
    vec3d v1 = tri.m_c;
    vec3d v2 = tri.m_b;
    
    /*
    if(nv3.dot(r.origin) < 0)
    {
        vec3d temp = v0;
        v0 = v1;
        v1 = temp;
    }
    */
    
    
    
    vec3d v0v1 = v1 - v0;
    vec3d v0v2 = v2 - v0;
    
    vec3d pvec = r.dir.cross(v0v2);
    float det = v0v1.dot(pvec);
    
    //---CULLING---
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    //if (det < kEpsilon) return false;
    //---------
    
    //---NO CULLING---
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;
    //---------------

    float invDet = 1 / det;
    
    vec3d tvec = r.origin - v0;
    u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;
    
    vec3d qvec = tvec.cross(v0v1);
    v = r.dir.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    
    t = v0v2.dot(qvec) * invDet;
    
    return true; // this ray hits the triangle
     
    
}

void bvhTree::testIntersect(bvhNode &root, ray r, float t, rayList &list)
{ // reached a leaf
    if(root.leftChild == NULL && root.rightChild == NULL)
    {
        rayNode *node = new rayNode(root,t);
        list.insertRayNode(node);
        return;
    }
    
    float tL;
    float tR;
    bool intersectL = this->intersect(root.leftChild->box,r,tL);
    bool intersectR = this->intersect(root.rightChild->box,r,tR);
    
    if( intersectL )
    {
        testIntersect(*root.leftChild, r, tL, list);
    }
    if( intersectR )
    {
        testIntersect(*root.rightChild, r, tR, list);
    }
}

bool bvhTree::intersect(aabb box, ray r, float &tnear)
{
    
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    
    if (r.dir.m_x >= 0) {
        tmin = (box.m_x1 - r.origin.m_x) / r.dir.m_x;
        tmax = (box.m_x2 - r.origin.m_x) / r.dir.m_x;
    }
    else {
        tmin = (box.m_x2 - r.origin.m_x) / r.dir.m_x;
        tmax = (box.m_x1 - r.origin.m_x) / r.dir.m_x;
    }
    if (r.dir.m_y >= 0) {
        tymin = (box.m_y1 - r.origin.m_y) / r.dir.m_y;
        tymax = (box.m_y2 - r.origin.m_y) / r.dir.m_y;
    }
    else {
        tymin = (box.m_y2 - r.origin.m_y) / r.dir.m_y;
        tymax = (box.m_y1 - r.origin.m_y) / r.dir.m_y;
    }
    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    if (r.dir.m_z >= 0) {
        tzmin = (box.m_z1 - r.origin.m_z) / r.dir.m_z;
        tzmax = (box.m_z2 - r.origin.m_z) / r.dir.m_z;
    }
    else {
        tzmin = (box.m_z2 - r.origin.m_z) / r.dir.m_z;
        tzmax = (box.m_z1 - r.origin.m_z) / r.dir.m_z;
    }
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    
    //return ( (tmin < t1) && (tmax > t0) ); // if there is t bounds for scene
    tnear = tmin;
    return true;
    
}
void bvhTree::walkBB(bvhNode &root, ray r, Tri3D &trianlgeFound, float &tClose)
{ // reached a leaf

    if(root.leftChild == NULL && root.rightChild == NULL && root.m_tri_cnt > 0)
    {
        float u,v;
        float t1;
        // make sure trianlge is in front of ray, and that it's greater than bias so no false intersects are made
        if(rayTriangleIntersectMT(r, *root.tri_list->head->tri, t1,u,v) && t1 < tClose && t1 > intersectBias)
        {
            tClose = t1;
            //trianlgeFound = root.m_tris[0];
            trianlgeFound = *root.tri_list->head->tri;
        }
        return;
    }
    
    float tL;
    float tR;
    float t;
    bool intersectL = this->intersect(root.leftChild->box,r,t);
    bool intersectR = this->intersect(root.rightChild->box,r,t);
    
    if( intersectL )
    {
        walkBB(*root.leftChild, r, trianlgeFound, tClose);
    }
    if( intersectR )
    {
        walkBB(*root.rightChild, r, trianlgeFound, tClose);
    }
}

bool bvhTree::newTest(ray r, float &t, Tri3D &tri)
{
    Tri3D trianlgeHit;
    float tNear = 1000000;
    walkBB(*this->root, r, trianlgeHit, tNear);
    if(tNear < 1000000)
    {
        tri = trianlgeHit;
        t = tNear;
        return true;
    }
    return false;
}
    
                              
bool bvhTree::testRay(ray r, float &t, Tri3D &tri)
{
    rayList rays;
    float u;
    float v;
    bool failed = false;
    
    testIntersect(*this->root, r, t, rays);
    
    if(!rays.empty)
    {
        bool done = false;
        rayNode *current = rays.front;
        while(!done && current != NULL)
        {
            done = rayTriangleIntersectMT(r, *current->m_node->tri_list->head->tri, t, u, v);
            if(done)
            {
                tri = *current->m_node->tri_list->head->tri;
                return true;
            }
            current = current->m_next;
        }
    }
    else
    {
        return false;
    }
    
    return false;
}

bvhTree::bvhTree(Tri3D tris[], int tris_cnt)
{
    
    std::clock_t start;
    double duration;
    start = std::clock();
    cout << "tri count: " << tris_cnt << endl;
    
    
    vec3d tri_area_v;
    float tri_area = 0;
    int tag = 0;
    for(int i = 0; i < tris_cnt; i++)
    {
        tris[i].tag = i;
        tri_area_v = 0.5*(tris[i].m_b - tris[i].m_a).cross(tris[i].m_c - tris[i].m_a);
        float tri_area_comp = fabs(tri_area_v.m_x);
        if(tri_area_comp > tri_area)
        {
            tri_area = tri_area_comp;
            tag = i;
        }
    }
    cout << "largest triangle: " << tag << endl;

    
    m_tris = tris; // tree has pointer record of the array of triangles
    
    root = new bvhNode;
    root->m_tri_cnt = tris_cnt;
    
    root->tri_list = new triangleList; // create the new list for the triangles
    
    for(int i = 0; i < tris_cnt; i++)
    { // insert each triangle
        root->tri_list->insertTriangle(tris[i]);
    }
   
    splitTree(*root, 0);
    
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "done creating tree in: " << duration << " seconds" << endl;

}


point3D bvhTree::triCentroid(Tri3D tri)
{
    float x = 0;
    float y = 0;
    float z = 0;
    
    x = (tri.m_ax + tri.m_bx + tri.m_cx)/3;
    y = (tri.m_ay + tri.m_by + tri.m_cy)/3;
    z = (tri.m_az + tri.m_bz + tri.m_cz)/3;
    
    return point3D(x,y,z);
}
aabb bvhTree::getSplitBox(Tri3D tris_in[], int tris_cnt)
{
    // get box in right location
    float xMin = triCentroid(tris_in[0]).m_x;
    float yMin = triCentroid(tris_in[0]).m_y;
    float zMin = triCentroid(tris_in[0]).m_z;
    float xMax = xMin;
    float yMax = yMin;
    float zMax = zMin;
    
    for(int i = 0; i < tris_cnt; i++)
    { // loop through each tri and expand split box based on centroids
        if( triCentroid(tris_in[i]).m_x < xMin ) xMin = triCentroid(tris_in[i]).m_x;
        if( triCentroid(tris_in[i]).m_y < yMin ) yMin = triCentroid(tris_in[i]).m_y;
        if( triCentroid(tris_in[i]).m_z < zMin ) zMin = triCentroid(tris_in[i]).m_z;
        
        if( triCentroid(tris_in[i]).m_x > xMax ) xMax = triCentroid(tris_in[i]).m_x;
        if( triCentroid(tris_in[i]).m_y > yMax ) yMax = triCentroid(tris_in[i]).m_y;
        if( triCentroid(tris_in[i]).m_z > zMax ) zMax = triCentroid(tris_in[i]).m_z;
    }
    return aabb(xMin,yMin,zMin , xMax,yMax,zMax);
}

aabb bvhTree::getSplitBox(triangleList &list)
{
    // get box in right location
    float xMin = triCentroid(*list.head->tri).m_x;
    float yMin = triCentroid(*list.head->tri).m_y;
    float zMin = triCentroid(*list.head->tri).m_z;
    float xMax = xMin;
    float yMax = yMin;
    float zMax = zMin;
    
    triangleNode *current = list.head;
    while(current != NULL)
    { // loop through each tri and expand split box based on centroids
        if( triCentroid(*current->tri).m_x < xMin ) xMin = triCentroid(*current->tri).m_x;
        if( triCentroid(*current->tri).m_y < yMin ) yMin = triCentroid(*current->tri).m_y;
        if( triCentroid(*current->tri).m_z < zMin ) zMin = triCentroid(*current->tri).m_z;
        
        if( triCentroid(*current->tri).m_x > xMax ) xMax = triCentroid(*current->tri).m_x;
        if( triCentroid(*current->tri).m_y > yMax ) yMax = triCentroid(*current->tri).m_y;
        if( triCentroid(*current->tri).m_z > zMax ) zMax = triCentroid(*current->tri).m_z;
        
        current = current->next;
    }
    return aabb(xMin,yMin,zMin , xMax,yMax,zMax);
}

//x:0 , y:1 , z:2
int bvhTree::getLongestAxis(aabb aabb_in)
{
    int ret = 0;
    float n1 = fabs(aabb_in.m_x1 - aabb_in.m_x2);
    float n2 = fabs(aabb_in.m_y1 - aabb_in.m_y2);
    float n3 = fabs(aabb_in.m_z1 - aabb_in.m_z2);

    if(n1 >= n2 && n1 >= n3)
    {
        ret = 0;
    }
    
    if(n2 >= n1 && n2 >= n3)
    {
        ret = 1;
    }
    
    if(n3 >= n1 && n3 >= n2)
    {
        ret = 2;
    }
    
    return ret;
}

void bvhTree::splitTree(bvhNode &root, int dp)
{
    static int depth = 0;
    dp++;
    
    
    root.m_boundx1 = (*root.tri_list->head->tri).m_ax;
    root.m_boundy1 = (*root.tri_list->head->tri).m_ay;
    root.m_boundz1 = (*root.tri_list->head->tri).m_az;
    root.m_boundx2 = root.m_boundx1;
    root.m_boundy2 = root.m_boundy1;
    root.m_boundz2 = root.m_boundz1;

    triangleNode *current = root.tri_list->head;
    
    while(current != NULL)
    {
        Tri3D &currentTri = *current->tri;
        
        // tighten bounds
        if(currentTri.m_ax < root.m_boundx1) root.m_boundx1 = currentTri.m_ax;
        if(currentTri.m_bx < root.m_boundx1) root.m_boundx1 = currentTri.m_bx;
        if(currentTri.m_cx < root.m_boundx1) root.m_boundx1 = currentTri.m_cx;
        
        if(currentTri.m_ax > root.m_boundx2) root.m_boundx2 = currentTri.m_ax;
        if(currentTri.m_bx > root.m_boundx2) root.m_boundx2 = currentTri.m_bx;
        if(currentTri.m_cx > root.m_boundx2) root.m_boundx2 = currentTri.m_cx;
        
        if(currentTri.m_ay < root.m_boundy1) root.m_boundy1 = currentTri.m_ay;
        if(currentTri.m_by < root.m_boundy1) root.m_boundy1 = currentTri.m_by;
        if(currentTri.m_cy < root.m_boundy1) root.m_boundy1 = currentTri.m_cy;
        
        if(currentTri.m_ay > root.m_boundy2) root.m_boundy2 = currentTri.m_ay;
        if(currentTri.m_by > root.m_boundy2) root.m_boundy2 = currentTri.m_by;
        if(currentTri.m_cy > root.m_boundy2) root.m_boundy2 = currentTri.m_cy;
        
        if(currentTri.m_az < root.m_boundz1) root.m_boundz1 = currentTri.m_az;
        if(currentTri.m_bz < root.m_boundz1) root.m_boundz1 = currentTri.m_bz;
        if(currentTri.m_cz < root.m_boundz1) root.m_boundz1 = currentTri.m_cz;
        
        if(currentTri.m_az > root.m_boundz2) root.m_boundz2 = currentTri.m_az;
        if(currentTri.m_bz > root.m_boundz2) root.m_boundz2 = currentTri.m_bz;
        if(currentTri.m_cz > root.m_boundz2) root.m_boundz2 = currentTri.m_cz;
        
        current = current->next;
    }
    root.box.m_x1 = root.m_boundx1;
    root.box.m_y1 = root.m_boundy1;
    root.box.m_z1 = root.m_boundz1;
    root.box.m_x2 = root.m_boundx2;
    root.box.m_y2 = root.m_boundy2;
    root.box.m_z2 = root.m_boundz2;
    
    if(root.m_tri_cnt <= 1)
    {
        return; // less than or equal one triangle in node, we can stop now
    }
    

    int leftCnt  = 0;
    int rightCnt = 0;
    
    aabb splitBox = getSplitBox(*root.tri_list);
    float n1 = fabs(splitBox.m_x1 - splitBox.m_x2);
    float n2 = fabs(splitBox.m_y1 - splitBox.m_y2);
    float n3 = fabs(splitBox.m_z1 - splitBox.m_z2);
    if(n1 == 0 && n2 == 0 && n3 == 0)
    { // there's more than 2 triangles but the centroids are on top of eachother -- could be duplicate triangles
        return; // don't split any further because they're the exact same so the one in first position can be used
    }
    
    int xyzSplit = getLongestAxis(getSplitBox(*root.tri_list)); // which axis is the longest?
    
    int lr = 0; // 0:l  1:r
    
    static unsigned long nodeCnt = 0;
    
    bvhNode *leftSubtree = new bvhNode;
    bvhNode *rightSubtree = new bvhNode;
    nodeCnt = nodeCnt + 2;
    
    leftSubtree->tri_list = new triangleList;
    rightSubtree->tri_list = new triangleList;

    while(root.tri_list->count > 0)
    {
        Tri3D &removedTriangle = root.tri_list->removeHead(); // take from the head and set reference to the trianlge
        
        if(xyzSplit == 0)
        {
            if(triCentroid(removedTriangle).m_x < (splitBox.m_x2 - fabs(splitBox.m_x1 - splitBox.m_x2)/2)) lr = 0;
            else lr = 1;
        }
        if(xyzSplit == 1)
        {
            if(triCentroid(removedTriangle).m_y < (splitBox.m_y2 - fabs(splitBox.m_y1 - splitBox.m_y2)/2)) lr = 0;
            else lr = 1;
        }
        if(xyzSplit == 2)
        {
            if(triCentroid(removedTriangle).m_z < (splitBox.m_z2 - fabs(splitBox.m_z1 - splitBox.m_z2)/2)) lr = 0;
            else lr = 1;
        }
        if(lr == 0)
        { // left subtree
            leftCnt++;
            leftSubtree->tri_list->insertTriangle(removedTriangle);
            leftSubtree->m_tri_cnt++;
        }
        else
        { // right subtree
            rightCnt++;
            rightSubtree->tri_list->insertTriangle(removedTriangle);
            rightSubtree->m_tri_cnt++;
        }
        
    }
    
    delete root.tri_list; // all of the triangles have been moved out
    root.tri_list = NULL;
     
    
    depth++;
    

    if(leftCnt > 0) splitTree(*leftSubtree, dp);
    if(rightCnt > 0) splitTree(*rightSubtree, dp);
    
    root.leftChild = leftSubtree;
    root.rightChild = rightSubtree;
    
    //cout << "node count: " << nodeCnt << endl;
}




////////////////////
//////OLD CODE//////
////////////////////
//------------------------
// Pre triangle list implementation:
/*
 bool bvhTree::rayTriangleIntersectMT(ray r, Tri3D tri, float &t, float &u, float &v)
 {
 
 
 vec3d nv3 = (tri.m_b - tri.m_a).cross(tri.m_c - tri.m_a);
 
 vec3d v0 = tri.m_a;
 vec3d v1 = tri.m_c;
 vec3d v2 = tri.m_b;



vec3d v0v1 = v1 - v0;
vec3d v0v2 = v2 - v0;

vec3d pvec = r.dir.cross(v0v2);
float det = v0v1.dot(pvec);

//---CULLING---
// if the determinant is negative the triangle is backfacing
// if the determinant is close to 0, the ray misses the triangle
//if (det < kEpsilon) return false;
//---------

//---NO CULLING---
// ray and triangle are parallel if det is close to 0
if (fabs(det) < kEpsilon) return false;
//---------------

float invDet = 1 / det;

vec3d tvec = r.origin - v0;
u = tvec.dot(pvec) * invDet;
if (u < 0 || u > 1) return false;

vec3d qvec = tvec.cross(v0v1);
v = r.dir.dot(qvec) * invDet;
if (v < 0 || u + v > 1) return false;

t = v0v2.dot(qvec) * invDet;

return true; // this ray hits the triangle


}

void bvhTree::testIntersect(bvhNode &root, ray r, float t, rayList &list)
{ // reached a leaf
    if(root.leftChild == NULL && root.rightChild == NULL)
    {
        rayNode *node = new rayNode(root,t);
        list.insertRayNode(node);
        return;
    }
    
    float tL;
    float tR;
    bool intersectL = this->intersect(root.leftChild->box,r,tL);
    bool intersectR = this->intersect(root.rightChild->box,r,tR);
    
    if( intersectL )
    {
        testIntersect(*root.leftChild, r, tL, list);
    }
    if( intersectR )
    {
        testIntersect(*root.rightChild, r, tR, list);
    }
}

bool bvhTree::intersect(aabb box, ray r, float &tnear)
{
    
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    
    if (r.dir.m_x >= 0) {
        tmin = (box.m_x1 - r.origin.m_x) / r.dir.m_x;
        tmax = (box.m_x2 - r.origin.m_x) / r.dir.m_x;
    }
    else {
        tmin = (box.m_x2 - r.origin.m_x) / r.dir.m_x;
        tmax = (box.m_x1 - r.origin.m_x) / r.dir.m_x;
    }
    if (r.dir.m_y >= 0) {
        tymin = (box.m_y1 - r.origin.m_y) / r.dir.m_y;
        tymax = (box.m_y2 - r.origin.m_y) / r.dir.m_y;
    }
    else {
        tymin = (box.m_y2 - r.origin.m_y) / r.dir.m_y;
        tymax = (box.m_y1 - r.origin.m_y) / r.dir.m_y;
    }
    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    if (r.dir.m_z >= 0) {
        tzmin = (box.m_z1 - r.origin.m_z) / r.dir.m_z;
        tzmax = (box.m_z2 - r.origin.m_z) / r.dir.m_z;
    }
    else {
        tzmin = (box.m_z2 - r.origin.m_z) / r.dir.m_z;
        tzmax = (box.m_z1 - r.origin.m_z) / r.dir.m_z;
    }
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    
    //return ( (tmin < t1) && (tmax > t0) ); // if there is t bounds for scene
    tnear = tmin;
    return true;
    
}
void bvhTree::walkBB(bvhNode &root, ray r, Tri3D &trianlgeFound, float &tClose)
{ // reached a leaf
    
    if(root.leftChild == NULL && root.rightChild == NULL && root.m_tri_cnt > 0)
    {
        float u,v;
        float t1;
        // make sure trianlge is in front of ray, and that it's greater than bias so no false intersects are made
        if(rayTriangleIntersectMT(r, root.m_tris[0], t1,u,v) && t1 < tClose && t1 > intersectBias)
        {
            tClose = t1;
            trianlgeFound = root.m_tris[0];
        }
        return;
    }
    
    float tL;
    float tR;
    float t;
    bool intersectL = this->intersect(root.leftChild->box,r,t);
    bool intersectR = this->intersect(root.rightChild->box,r,t);
    
    if( intersectL )
    {
        walkBB(*root.leftChild, r, trianlgeFound, tClose);
    }
    if( intersectR )
    {
        walkBB(*root.rightChild, r, trianlgeFound, tClose);
    }
}

bool bvhTree::newTest(ray r, float &t, Tri3D &tri)
{
    Tri3D trianlgeHit;
    float tNear = 1000000;
    walkBB(*this->root, r, trianlgeHit, tNear);
    if(tNear < 1000000)
    {
        tri = trianlgeHit;
        t = tNear;
        return true;
    }
    return false;
}


bool bvhTree::testRay(ray r, float &t, Tri3D &tri)
{
    rayList rays;
    float u;
    float v;
    bool failed = false;
    
    testIntersect(*this->root, r, t, rays);
    
    if(!rays.empty)
    {
        bool done = false;
        rayNode *current = rays.front;
        while(!done && current != NULL)
        {
            done = rayTriangleIntersectMT(r, current->m_node->m_tris[0], t, u, v);
            if(done)
            {
                tri = current->m_node->m_tris[0];
                return true;
            }
            current = current->m_next;
        }
    }
    else
    {
        return false;
    }
    
    return false;
}

bvhTree::bvhTree(Tri3D tris[], int tris_cnt)
{
    std::clock_t start;
    double duration;
    start = std::clock();
    cout << "tri count: " << tris_cnt << endl;
    
    
    vec3d tri_area_v;
    float tri_area = 0;
    int tag = 0;
    for(int i = 0; i < tris_cnt; i++)
    {
        tris[i].tag = i;
        tri_area_v = 0.5*(tris[i].m_b - tris[i].m_a).cross(tris[i].m_c - tris[i].m_a);
        float tri_area_comp = fabs(tri_area_v.m_x);
        if(tri_area_comp > tri_area)
        {
            tri_area = tri_area_comp;
            tag = i;
        }
    }
    cout << "largest triangle: " << tag << endl;
    
    
    m_tris = tris; // tree has record of the array of triangles
    root = new bvhNode(tris_cnt, tris);
    
    splitTree(*root, 0);
    
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "done creating tree in: " << duration << " seconds" << endl;
    
}


point3D bvhTree::triCentroid(Tri3D tri)
{
    float x = 0;
    float y = 0;
    float z = 0;
    
    x = (tri.m_ax + tri.m_bx + tri.m_cx)/3;
    y = (tri.m_ay + tri.m_by + tri.m_cy)/3;
    z = (tri.m_az + tri.m_bz + tri.m_cz)/3;
    
    return point3D(x,y,z);
}
aabb bvhTree::getSplitBox(Tri3D tris_in[], int tris_cnt)
{
    // get box in right location
    float xMin = triCentroid(tris_in[0]).m_x;
    float yMin = triCentroid(tris_in[0]).m_y;
    float zMin = triCentroid(tris_in[0]).m_z;
    float xMax = xMin;
    float yMax = yMin;
    float zMax = zMin;
    
    for(int i = 0; i < tris_cnt; i++)
    { // loop through each tri and expand split box based on centroids
        if( triCentroid(tris_in[i]).m_x < xMin ) xMin = triCentroid(tris_in[i]).m_x;
        if( triCentroid(tris_in[i]).m_y < yMin ) yMin = triCentroid(tris_in[i]).m_y;
        if( triCentroid(tris_in[i]).m_z < zMin ) zMin = triCentroid(tris_in[i]).m_z;
        
        if( triCentroid(tris_in[i]).m_x > xMax ) xMax = triCentroid(tris_in[i]).m_x;
        if( triCentroid(tris_in[i]).m_y > yMax ) yMax = triCentroid(tris_in[i]).m_y;
        if( triCentroid(tris_in[i]).m_z > zMax ) zMax = triCentroid(tris_in[i]).m_z;
    }
    return aabb(xMin,yMin,zMin , xMax,yMax,zMax);
}

//x:0 , y:1 , z:2
int bvhTree::getLongestAxis(aabb aabb_in)
{
    int ret = 0;
    float n1 = fabs(aabb_in.m_x1 - aabb_in.m_x2);
    float n2 = fabs(aabb_in.m_y1 - aabb_in.m_y2);
    float n3 = fabs(aabb_in.m_z1 - aabb_in.m_z2);
    
    if(n1 >= n2 && n1 >= n3)
    {
        ret = 0;
    }
    
    if(n2 >= n1 && n2 >= n3)
    {
        ret = 1;
    }
    
    if(n3 >= n1 && n3 >= n2)
    {
        ret = 2;
    }
    
    return ret;
}

void bvhTree::splitTree(bvhNode &root, int dp)
{
    static int depth = 0;
    dp++;
    
    
    root.m_boundx1 = root.m_tris[0].m_ax;
    root.m_boundy1 = root.m_tris[0].m_ay;
    root.m_boundz1 = root.m_tris[0].m_az;
    root.m_boundx2 = root.m_boundx1;
    root.m_boundy2 = root.m_boundy1;
    root.m_boundz2 = root.m_boundz1;
    
    // todo: after triangle list modification, this will walk down
    //       the triangle list until null pointer is hit (the end)
    for(int i = 0; i < root.m_tri_cnt; i++)
    {
        // tighten bounds
        if(root.m_tris[i].m_ax < root.m_boundx1) root.m_boundx1 = root.m_tris[i].m_ax;
        if(root.m_tris[i].m_bx < root.m_boundx1) root.m_boundx1 = root.m_tris[i].m_bx;
        if(root.m_tris[i].m_cx < root.m_boundx1) root.m_boundx1 = root.m_tris[i].m_cx;
        
        if(root.m_tris[i].m_ax > root.m_boundx2) root.m_boundx2 = root.m_tris[i].m_ax;
        if(root.m_tris[i].m_bx > root.m_boundx2) root.m_boundx2 = root.m_tris[i].m_bx;
        if(root.m_tris[i].m_cx > root.m_boundx2) root.m_boundx2 = root.m_tris[i].m_cx;
        
        if(root.m_tris[i].m_ay < root.m_boundy1) root.m_boundy1 = root.m_tris[i].m_ay;
        if(root.m_tris[i].m_by < root.m_boundy1) root.m_boundy1 = root.m_tris[i].m_by;
        if(root.m_tris[i].m_cy < root.m_boundy1) root.m_boundy1 = root.m_tris[i].m_cy;
        
        if(root.m_tris[i].m_ay > root.m_boundy2) root.m_boundy2 = root.m_tris[i].m_ay;
        if(root.m_tris[i].m_by > root.m_boundy2) root.m_boundy2 = root.m_tris[i].m_by;
        if(root.m_tris[i].m_cy > root.m_boundy2) root.m_boundy2 = root.m_tris[i].m_cy;
        
        if(root.m_tris[i].m_az < root.m_boundz1) root.m_boundz1 = root.m_tris[i].m_az;
        if(root.m_tris[i].m_bz < root.m_boundz1) root.m_boundz1 = root.m_tris[i].m_bz;
        if(root.m_tris[i].m_cz < root.m_boundz1) root.m_boundz1 = root.m_tris[i].m_cz;
        
        if(root.m_tris[i].m_az > root.m_boundz2) root.m_boundz2 = root.m_tris[i].m_az;
        if(root.m_tris[i].m_bz > root.m_boundz2) root.m_boundz2 = root.m_tris[i].m_bz;
        if(root.m_tris[i].m_cz > root.m_boundz2) root.m_boundz2 = root.m_tris[i].m_cz;
    }
    root.box.m_x1 = root.m_boundx1;
    root.box.m_y1 = root.m_boundy1;
    root.box.m_z1 = root.m_boundz1;
    root.box.m_x2 = root.m_boundx2;
    root.box.m_y2 = root.m_boundy2;
    root.box.m_z2 = root.m_boundz2;
    
    if(root.m_tri_cnt <= 1)
    {
        return; // less than or equal one triangle in node, we can stop now
    }
    
    // this won't be needed anymore once the triangle list is implemeneted
    Tri3D *trisLeft = new Tri3D[root.m_tri_cnt];
    Tri3D *trisRight = new Tri3D[root.m_tri_cnt];
    
    
    int leftCnt  = 0;
    int rightCnt = 0;
    
    aabb splitBox = getSplitBox(root.m_tris,root.m_tri_cnt);
    float n1 = fabs(splitBox.m_x1 - splitBox.m_x2);
    float n2 = fabs(splitBox.m_y1 - splitBox.m_y2);
    float n3 = fabs(splitBox.m_z1 - splitBox.m_z2);
    if(n1 == 0 && n2 == 0 && n3 == 0)
    { // there's more than 2 triangles but the centroids are on top of eachother -- could be duplicate triangles
        return; // don't split any further because they're the exact same so the one in first position can be used
    }
    
    // todo: modify the getLongestAxis to take in a triangle _list_
    int xyzSplit = getLongestAxis(getSplitBox(root.m_tris,root.m_tri_cnt)); // which axis is the longest?
    
    int lr = 0; // 0:l  1:r


    
    // instead of a loop over amount, it will be
    // a walk down the triangle list until it hits
    // the null pointer (meaning the end)
    // root.m_tris[i] will be replaced with the current
    // triangle node, which can be dereferenced to the actual triangle
    for(int i = 0; i < root.m_tri_cnt; i++)
    {
        if(xyzSplit == 0)
        {
            if(triCentroid(root.m_tris[i]).m_x < (splitBox.m_x2 - fabs(splitBox.m_x1 - splitBox.m_x2)/2)) lr = 0;
            else lr = 1;
        }
        if(xyzSplit == 1)
        {
            if(triCentroid(root.m_tris[i]).m_y < (splitBox.m_y2 - fabs(splitBox.m_y1 - splitBox.m_y2)/2)) lr = 0;
            else lr = 1;
        }
        if(xyzSplit == 2)
        {
            if(triCentroid(root.m_tris[i]).m_z < (splitBox.m_z2 - fabs(splitBox.m_z1 - splitBox.m_z2)/2)) lr = 0;
            else lr = 1;
        }
        if(lr == 0)
        { // left subtree
            trisLeft[leftCnt] = root.m_tris[i];
            leftCnt++;
            // future implementation:
            // _remove_ from parent tree
            // _push_ into subtree
        }
        else
        { // right subtree
            trisRight[rightCnt] = root.m_tris[i];
            rightCnt++;
        }
        
    }
    
    // this won't be needed anymore since the
    // xyz split will take care of (shrinking) the
    // parent subtree's triangle linked list. By the end,
    // all of the triangles will have been moved into the left or right subtrees
    // add in a check to make sure that the parent tree triangle count is exactly 0.
    // If it's greater than 0, or less than 0, then that means something went wrong
    // because there's then the same triangle in two trees (it didn't split)
    
    if(depth > 0)
    {
        delete[] root.m_tris; // now you can delete the triangles that were sent down from subtree node
        root.m_tri_cnt = 0;
        root.m_tris = NULL;
    }
    
    depth++;
    
    // this will be moved to above the xyz split so
    // that the triangle list will be removing and pushing
    // into these new subtrees directly
    bvhNode *leftSubtree = new bvhNode(leftCnt, trisLeft);
    bvhNode *rightSubtree = new bvhNode(rightCnt, trisRight);
    
    
    if(leftCnt > 0) splitTree(*leftSubtree, dp);
    if(rightCnt > 0) splitTree(*rightSubtree, dp);
    
    root.leftChild = leftSubtree;
    root.rightChild = rightSubtree;
}
 */
//------------------------