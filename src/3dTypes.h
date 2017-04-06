//
//  3dTypes.h
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//
//

#ifndef _dTypes_h
#define _dTypes_h

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const float kEpsilon = 1e-8;

//----forward declarations----
struct meshNode; // from scene.h | triangle needs to know its parent mesh
//----------------------------

struct point3D
{
    float m_x;
    float m_y;
    float m_z;
    point3D()
    {
        m_x = 0;
        m_y = 0;
        m_z = 0;
    }
    point3D(float x, float y, float z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }
    point3D(point3D const &in)
    {
        m_x = in.m_x;
        m_y = in.m_y;
        m_z = in.m_z;
    }
};

struct vec3d
{
    float m_x;
    float m_y;
    float m_z;
    
    vec3d(float x, float y, float z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }
    vec3d()
    {
        m_x = 0;
        m_y = 0;
        m_z = 0;
    }
    vec3d(point3D pt)
    {
        m_x = pt.m_x;
        m_y = pt.m_y;
        m_z = pt.m_z;
    }
    
    float dot(const vec3d &b) const
    {
        return m_x*b.m_x + m_y*b.m_y + m_z*b.m_z;
    }
    vec3d cross(vec3d b)
    {
        vec3d c;
        c.m_x = m_y*b.m_z - m_z*b.m_y;
        c.m_y = m_z*b.m_x - m_x*b.m_z;
        c.m_z = m_x*b.m_y - m_y*b.m_x;
        return c;
    }
    float length()
    {
        return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
    }
    
    vec3d operator/(const float b) const { return vec3d(m_x/b,m_y/b,m_z/b); }
    vec3d operator+(const vec3d &b) const { return vec3d(m_x+b.m_x,m_y+b.m_y,m_z+b.m_z); }
    vec3d operator-(const vec3d &b) const { return vec3d(m_x-b.m_x,m_y-b.m_y,m_z-b.m_z); }
    vec3d operator*(float b) const { return vec3d(m_x*b,m_y*b,m_z*b); }
    vec3d operator%(vec3d b) { return cross(b); } // cross product
    vec3d mult(const vec3d &b) const { return vec3d(m_x*b.m_x,m_y*b.m_y,m_z*b.m_z); }
    vec3d& norm(){ return *this = *this * (1/sqrt(m_x*m_x+m_y*m_y+m_z*m_z)); }
    
    
};

inline vec3d unit_vector(vec3d b)
{
    return b/b.length();
}

inline vec3d operator *(float t, const vec3d v)
{
    return vec3d(t*v.m_x,t*v.m_y,t*v.m_z);
}

struct ray
{
    vec3d origin;
    vec3d dir;
    ray()
    {
        origin = vec3d(0,0,0);
        dir    = vec3d(0,0,0);
    }
    ray(vec3d o, vec3d d)
    {
        origin = o;
        dir = d;
    }
    vec3d point_at_parameter(float t) {return origin + t*dir;}
};


struct aabb
{
    float m_x1 = 0;
    float m_y1 = 0;
    float m_z1 = 0;
    
    float m_x2 = 0;
    float m_y2 = 0;
    float m_z2 = 0;
    
    aabb(float x1, float y1, float z1  ,  float x2, float y2, float z2)
    {
        m_x1 = x1;
        m_y1 = y1;
        m_z1 = z1;
        
        m_x2 = x2;
        m_y2 = y2;
        m_z2 = z2;
    }
    aabb()
    {
        m_x1 = 0;
        m_y1 = 0;
        m_z1 = 0;
        
        m_x2 = 0;
        m_y2 = 0;
        m_z2 = 0;
    }
};


struct Tri3D
{
    /*
     b
     
     
     a     c
     
     */
    
    unsigned int tag = 0;
    meshNode *parentMesh;
    
    float m_ax;
    float m_ay;
    float m_az;
    
    float m_bx;
    float m_by;
    float m_bz;
    
    float m_cx;
    float m_cy;
    float m_cz;
    
    vec3d m_a;
    vec3d m_b;
    vec3d m_c;
    
    Tri3D(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
    //constructor
    {
        m_ax = ax;
        m_ay = ay;
        m_az = az;
        
        m_bx = bx;
        m_by = by;
        m_bz = bz;
        
        m_cx = cx;
        m_cy = cy;
        m_cz = cz;

        m_a.m_x = m_ax;
        m_a.m_y = m_ay;
        m_a.m_z = m_az;
        m_b.m_x = m_bx;
        m_b.m_y = m_by;
        m_b.m_z = m_bz;
        m_c.m_x = m_cx;
        m_c.m_y = m_cy;
        m_c.m_z = m_cz;
    }
    Tri3D(vec3d a, vec3d b, vec3d c)
    //constructor
    {
        m_a = a;
        m_b = b;
        m_c = c;
        
        m_ax = a.m_x;
        m_ay = a.m_y;
        m_az = a.m_z;
        
        m_bx = b.m_x;
        m_by = b.m_y;
        m_bz = b.m_z;
        
        m_cx = c.m_x;
        m_cy = c.m_y;
        m_cz = c.m_z;
    }
    Tri3D()
    //constructor
    {
        m_ax = 0;
        m_ay = 0;
        m_az = 0;
        
        m_bx = 0;
        m_by = 0;
        m_bz = 0;
        
        m_cx = 0;
        m_cy = 0;
        m_cz = 0;
        
        m_a.m_x = m_ax;
        m_a.m_y = m_ay;
        m_a.m_z = m_az;
        m_b.m_x = m_bx;
        m_b.m_y = m_by;
        m_b.m_z = m_bz;
        m_c.m_x = m_cx;
        m_c.m_y = m_cy;
        m_c.m_z = m_cz;
    }
    
};


#endif /* _dTypes_h */
