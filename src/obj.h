//
//  obj.h
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//
//

#ifndef obj_h
#define obj_h

#include "3dTypes.h"
#include <fstream>

using namespace std;

struct face3D
{
    int m_a;
    int m_b;
    int m_c;
    face3D()
    {
        m_a = 0;
        m_b = 0;
        m_c = 0;
    }
    face3D(int a, int b, int c)
    {
        m_a = a;
        m_b = b;
        m_c = c;
    }
    face3D(face3D const &in)
    {
        m_a = in.m_a;
        m_b = in.m_b;
        m_c = in.m_c;
    }
};

class obj
{
public:
    int tris_cnt;
    Tri3D *tri;
    obj(string filename)
    {
        m_filename = filename;
        openOBJ();
    }
    ~obj()
    {
        delete[] tri;
    }
private:
    ifstream file;
    void getOBJPos(int &vstart, int &vend, int &fstart, int &fend);
    void openOBJ();
    void getVertices(point3D vertices[], int vstart, int vend);
    void getFaces(face3D faces[], int fstart, int fend);
    string m_filename;

};

#endif /* obj_h */
