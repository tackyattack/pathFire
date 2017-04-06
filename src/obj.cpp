//
//  obj.cpp
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//  Takes an obj file and makes a triangle list
//

#include "obj.h"
#include <fstream>
#include <iostream>

using namespace std;

const float scale = 50.0;

void obj::getOBJPos(int &vstart, int &vend, int &fstart, int &fend)
{
    
    char line[500] = {0};
    while(line[0] != 'v')
    {
        file.getline(line,500,'\n');
        vstart++;
    }
    vend=vstart-1;
    while(line[0] != 'v' || line[1] != 'n')
    {
        file.getline(line,500,'\n');
        vend++;
    }
    fstart=vend+1;
    while(line[0] != 'f')
    {
        file.getline(line,500,'\n');
        fstart++;
    }
    fend = fstart;
    while(!file.eof())
    {
        file.getline(line,500,'\n');
        fend++;
    }
    vstart-=1;
    vend-=1;
    fstart-=1;
    fend-=1;
    cout << "vstart: " << vstart << " vend " << vend << " fstart " << fstart << " fend " << fend << endl;
}

void obj::getVertices(point3D vertices[], int vstart, int vend)
{
    char line[500] = {0};
    file.clear();
    file.seekg (0, file.beg);
    for (int i = 0; i < vstart; i++)
    {
        file.getline(line, 500, '\n');
    }
    for(int i = 0; i <= (vend-vstart); i++)
    {
        file.getline(line, 500, ' '); // skip v
        file.getline(line, 500, ' '); // get x
        vertices[i].m_x = atof(line);
        file.getline(line, 500, ' '); // get y
        vertices[i].m_y = atof(line);
        file.getline(line, 500, '\n'); // get z
        vertices[i].m_z = atof(line);
        //cout << "x: " << vertices[i].m_x << " y: " << vertices[i].m_y << " z: " << vertices[i].m_z << endl;
    }
}

void obj::getFaces(face3D faces[], int fstart, int fend)
{
    char line[500] = {0};
    file.clear();
    file.seekg (0, file.beg);
    for (int i = 0; i < fstart; i++)
    {
        file.getline(line, 500, '\n');
    }
    for(int i = 0; i <= (fend-fstart); i++)
    {
        file.getline(line, 500, ' '); // skip v
        file.getline(line, 500, ' '); // get x
        faces[i].m_a = atoi(line);
        file.getline(line, 500, ' '); // get y
        faces[i].m_b = atoi(line);
        file.getline(line, 500, '\n'); // get z
        faces[i].m_c = atoi(line);
        //cout << "a: " << faces[i].m_a << " b: " << faces[i].m_b << " c: " << faces[i].m_c << endl;
    }
}

void obj::openOBJ()
{
    
    do                                               // loop to insure stream connection
    {
        file.clear();                                  // reset connect fail bit
        file.open(m_filename.c_str());
    } while (!file);                                   // checks the connection
    cout << "file opened" << endl;
    file.seekg (0, file.beg);
    
    int vstart = 0;
    int vend   = 0;
    int fstart = 0;
    int fend   = 0;
    getOBJPos(vstart,vend,fstart,fend);
    
    point3D *vertices;
    vertices = new point3D[vend-vstart];
    
    getVertices(vertices,vstart,vend);
    face3D *faces;
    faces = new face3D[fend-fstart];
    getFaces(faces,fstart,fend);
    
    tri = new Tri3D[fend-fstart];
    tris_cnt = fend - fstart + 1;
    for(int i = 0; i < tris_cnt; i++)
    {
        
        tri[i].m_ax = scale*vertices[faces[i].m_a-1].m_x;
        tri[i].m_ay = scale*vertices[faces[i].m_a-1].m_y;
        tri[i].m_az = scale*vertices[faces[i].m_a-1].m_z;
        
        tri[i].m_bx = scale*vertices[faces[i].m_b-1].m_x;
        tri[i].m_by = scale*vertices[faces[i].m_b-1].m_y;
        tri[i].m_bz = scale*vertices[faces[i].m_b-1].m_z;
        
        tri[i].m_cx = scale*vertices[faces[i].m_c-1].m_x;
        tri[i].m_cy = scale*vertices[faces[i].m_c-1].m_y;
        tri[i].m_cz = scale*vertices[faces[i].m_c-1].m_z;
        
        tri[i].m_a.m_x = tri[i].m_ax;
        tri[i].m_a.m_y = tri[i].m_ay;
        tri[i].m_a.m_z = tri[i].m_az;
        tri[i].m_b.m_x = tri[i].m_bx;
        tri[i].m_b.m_y = tri[i].m_by;
        tri[i].m_b.m_z = tri[i].m_bz;
        tri[i].m_c.m_x = tri[i].m_cx;
        tri[i].m_c.m_y = tri[i].m_cy;
        tri[i].m_c.m_z = tri[i].m_cz;
    }
    
    delete[] faces;
    delete [] vertices;
}