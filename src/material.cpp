//
//  material.cpp
//  pathTrace
//
//  Created by HENRY BERGIN on 1/9/17.
//
//

#include <stdio.h>
#include "material.h"
#include <iostream>
using namespace std;

/*
 NOTES:
 
 make sure i,o,m,n are all unit vectors when passed in
 
*/

float sign(float a)
{ // clamps a to [-1,1]
    if(a >= 0)
        return 1; // a > 0
    else
        return -1; // a <= 0
}

float chi_plus(float a)
{ // binary of is a positive or not?
    if(a > 0)
        return 1; // a > 0
    else
        return 0; // a <= 0
}

float GGX_F(vec3d v, vec3d m, float n1, float n2)
{ // GGX fresnel term
    
    v = v.norm(); // make sure viewing angle and microfacet are normalized
    m = m.norm();
    
    //return powf(1- v.dot(m), 5);

    float c = fabs(v.dot(m));
    float g = sqrt((n2*n2)/(n1*n1) - 1 + c*c);
    
    float cnum = c*(g+c) - 1;
    float cden = c*(g-c) + 1;
    return 0.5*((g - c)*(g - c))/((g + c)*(g + c))*(1 + (cnum*cnum)/(cden*cden));
}
float GGX_G(vec3d v, vec3d n, vec3d m, float alpha)
{ // GGX geometry term
    
    float tan_theta_v_sqr = ( 1 - v.dot(n)*v.dot(n) ) / ( v.dot(n)*v.dot(n) );
    float num = chi_plus(v.dot(m) / v.dot(n)) * 2;
    float den = 1 + sqrt(1 + alpha*alpha*tan_theta_v_sqr);
    
    return  num/den;
}
float GGX_D(vec3d n, vec3d m, float alpha)
{ // GGX distribution term
    
    m = m.norm();
    n = n.norm();
    
    float cos_theta_m = n.dot(m);
    float tan_theta_m_sqr = ( 1 - n.dot(m)*n.dot(m) ) / ( n.dot(m)*n.dot(m) );
    
    float num = alpha*alpha*chi_plus(m.dot(n));
    float den = M_PI*cos_theta_m*cos_theta_m*cos_theta_m*cos_theta_m*(alpha*alpha + tan_theta_m_sqr)*(alpha*alpha + tan_theta_m_sqr);
    return num/den;
}

float GGX_PDF(vec3d i, vec3d o, vec3d n, vec3d m, float alpha)
{ // GGX PDF for scattered direction o
    
    o = o.norm();
    i = i.norm();
    m = m.norm();
    n = n.norm();

    //vec3d hr = sign(i.dot(n))*(i+o);
    vec3d hr = i+o;
    vec3d h  = hr.norm();
    
    float jacobian = 1 / (4*fabs(o.dot(h)));
    
    
    float PDFm = GGX_D(n, m, alpha)*fabs(m.dot(n));
    
    return PDFm*jacobian; // transform from m PDF to include scatter direction versus just m direction
}

float GGX_PROB_T(vec3d i, vec3d o, vec3d n, vec3d m, float alpha, float ni, float no)
{ // GGX PDF for transmitted direction o
    
    o = o.norm();
    i = i.norm();
    m = m.norm();
    n = n.norm();
    
    vec3d ht = (-1)*(ni)*i + (-1)*(no)*o; // half vector
    vec3d h = ht.norm(); // unit half vector
    
    float jD = ni*(i.dot(h)) + no*(o.dot(h));
    jD = jD*jD;
    float jacobian = (no*no)*fabs(o.dot(h))/jD;
    
    
    float PDFm = GGX_D(n, m, alpha)*fabs(m.dot(n));
    
    return PDFm*jacobian; // transform from m PDF to include transmitted direction versus just m direction
}

vec3d scatterDirection(vec3d i, vec3d m)
{ // given i and m, returns where the scattered reflection goes
    return 2*fabs(i.dot(m))*m - i;
}

vec3d transmissionDirection(vec3d i, vec3d m, float ni, float no)
{ // returns where the scattered refraction goes
    
    float n = ni/no;
    float c = i.dot(m);
    
    return ( n*c - sign(i.dot(m))*sqrt(1 + n*(c*c - 1) ) )*m - n*i;
}


vec3d generate_microfacet(vec3d n, vec3d v, float alpha, unsigned short *Xi)
{ // creates the normal of the microsurface
    n = n.norm();
    v = v.norm();
    
    float x1 = erand48(Xi);
    float x2 = erand48(Xi);
    
    float theta_m = atan(alpha*sqrt(x1)/sqrt(1 - x1)); // up / down altitude based on n
    float phi_m   = 2*M_PI*x2; // perpendicular to surface normal, rotates outgoing away from v
    
/*
    n.m_x = 0;
    n.m_y = 0; // on z axis
    n.m_z = 1;
    
    v.m_x = 0;
    v.m_y = 1; // on y axis
    v.m_z = 0;
   */ 
    vec3d a = n; // parallel to normal
    vec3d b = v.cross(a); // out of n and v plane
    vec3d c = a.cross(b); // parallel to v and in n and v plane
    
    //theta_m = M_PI/2;
    //phi_m   = M_PI/2;
    
    // spherical coordinates to frame coordinates
    float m_c = sin(theta_m)*cos(phi_m);
    float m_b = sin(theta_m)*sin(phi_m);
    float m_a = cos(theta_m);
    //----------------------------------------
    /*
    //cout << endl;
    vec3d p;
    
    //cout << "spherical: <" << m_b << "," << m_c << "," << m_a << ">" << endl;
    
    p = n;
    //cout << "normanl: <" << p.m_x << "," << p.m_y << "," << p.m_z << ">" << endl;
    
    p = a;
    //cout << "coord a: <" << p.m_x << "," << p.m_y << "," << p.m_z << ">" << endl;
    
    p = b;
    //cout << "coord b: <" << p.m_x << "," << p.m_y << "," << p.m_z << ">" << endl;
    
    p = c;
    //cout << "coord c: <" << p.m_x << "," << p.m_y << "," << p.m_z << ">" << endl;
    
    p = (m_a*a + m_b*b + m_c*c);
     */
    //p = a*cos(M_PI/2) + c*cos(0) + b*sin(0);
    //cout << "micronormal: <" << p.m_x << "," << p.m_y << "," << p.m_z << ">" << endl;
    //while(1);
    
    return m_a*a + m_b*b + m_c*c; // convert coordinate frame back into x,y,z space
}

float GGX_BRDF(vec3d i, vec3d o, vec3d n, float alpha)
{ // microfacet BRDF for reflective material
    
    i = i.norm();
    o = o.norm();
    n = n.norm();
    
    //vec3d hr = sign(i.dot(n))*(i+o); // make it work for both sides (just for reflection, although may not be
                                       // necessary since nl is auto orienting)
    vec3d hr = i+o;
    vec3d h  = hr.norm();

    float G1 = GGX_G(i,n,h,alpha);
    float G2 = GGX_G(o,n,h,alpha);
    float G  = G1*G2;

    float D = GGX_D(n,h,alpha);
    
    //float F = GGX_F(o,h,1.0,1.46);
    //cout << F << endl;
    float F = 1; // looking through Blender's implementation, they just skip the F term and leave it up to the node editor
                 // to determine if fresnal is to be added
    //G = 1;

    float num = F*D*G;
    float den = 4*fabs(i.dot(n))*fabs(o.dot(n));
    return num/den;
}