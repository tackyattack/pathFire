/*
 
 notes: how slabs work: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
                        http://people.csail.mit.edu/amy/papers/box-jgt.pdf

 // warning: make sure obj doesn't have blank line at end otherwise it will crash
 
 // render : does all the ray tracing
 // camera : puts everything together (bvh, ray, obj) into a frame buffer and does ray casting / anti-aliasing
 // obj    : gets the object
 // bvh    : builds the bvh tree
 
*/

#include "ofApp.h"
#include <ctime>
#include <fstream>
#include <iostream>

const string OBJ_FILE = "/Users/henrybergin/Documents/of_v0.9.8_osx_release/apps/myApps/pathTrace/objects/sphere_light.obj";
// NOTE: you have to make sure OBJ file doesn't have extra return at end

using namespace std;

void drawRay(ray r)
{
    ofSetColor(255,0,0);
    ofFill();
    float x2 = r.origin.m_x + r.dir.m_x*100000.0;
    float y2 = r.origin.m_y + r.dir.m_y*100000.0;
    float z2 = r.origin.m_z + r.dir.m_z*100000.0;
    ofDrawLine(r.origin.m_x, r.origin.m_y, r.origin.m_z, x2, y2, z2);
}

void boundingBox3D(float x1, float y1, float z1, float x2, float y2, float z2)
{ // first point is top front left corner, second point is bottom right in the back
    ofSetColor(0,0,255);
    ofFill();
    //front face
    ofDrawLine(x1,y1,z1,x2,y1,z1); // horizontal top
    ofDrawLine(x1,y2,z1,x2,y2,z1); // horizontal bottom
    ofDrawLine(x1,y1,z1,x1,y2,z1); // verticle left
    ofDrawLine(x2,y1,z1,x2,y2,z1); // verticle right
    //back face
    ofDrawLine(x1,y1,z2,x2,y1,z2); // horizontal top
    ofDrawLine(x1,y2,z2,x2,y2,z2); // horizontal bottom
    ofDrawLine(x1,y1,z2,x1,y2,z2); // verticle left
    ofDrawLine(x2,y1,z2,x2,y2,z2); // verticle right
    //right top edge
    ofDrawLine(x2,y1,z1,x2,y1,z2);
    //right bottom edge
    ofDrawLine(x2,y2,z1,x2,y2,z2);
    //left top edge
    ofDrawLine(x1,y1,z1,x1,y1,z2);
    //left bottom edge
    ofDrawLine(x1,y2,z1,x1,y2,z2);
}

void boundingBox3D(aabb box)
{
    boundingBox3D(box.m_x1, box.m_y1, box.m_z1, box.m_x2, box.m_y2, box.m_z2);
}

void drawTriangle3D(Tri3D tri_in)
{
    ofSetColor(0,255,0);
    ofFill();
    ofDrawTriangle(tri_in.m_ax,tri_in.m_ay,tri_in.m_az,
                   tri_in.m_bx,tri_in.m_by,tri_in.m_bz,
                   tri_in.m_cx,tri_in.m_cy,tri_in.m_cz);
    
}

//--------------------------------------------------------------
void drawInteractionArea();
bool bShowHelp;
ofEasyCam cam; // add mouse controls for camera movement

//bvhTree *tree;

renderer rend;

void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetVerticalSync(true);
    
    // this uses depth information for occlusion
    // rather than always drawing things on top of each other
    ofEnableDepthTest();
    
    // this sets the camera's distance from the object
    cam.setDistance(1000);
    
    ofSetCircleResolution(64);
    bShowHelp = true;
    
    obj myObject(OBJ_FILE); // open the obj
    

    rend.render(myObject.tri, myObject.tris_cnt);
}
/*
void printPreorder(bvhNode node)
{
    if (node.leftChild == NULL)
    {
        drawTriangle3D(node.m_tris[0]);
        return;
    }
    
 
    boundingBox3D(node.m_boundx1,node.m_boundy1,node.m_boundz1 , node.m_boundx2,node.m_boundy2,node.m_boundz2);
    

    printPreorder(*node.leftChild);
 
    printPreorder(*node.rightChild);
}
*/

void drawPix()
{
    ofFill();
    int x = 0;
    int y = 0;
    for(int i = 0; i < PIXLE_CNT; i++)
    {
        ofSetColor(rend.pixles[i].r,rend.pixles[i].g,rend.pixles[i].b);
        //if(x == 500 && y == 275) ofSetColor(0,255,0);
        ofDrawRectangle(x, y, 0, 1, 1);
        x++;
        if(x==WIDTH)
        {
            x=0;
            y++;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------

float i = 0;

void ofApp::draw(){
    /*
    cam.begin();
     
    ofRotateY(ofRadToDeg(sin(i)));
    i = i + 0.01;
    if(i > 200) i = 0;
    
    printPreorder(*tree->root);
    
    drawRay(r);
    
    cam.end();
     */
    drawPix();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
