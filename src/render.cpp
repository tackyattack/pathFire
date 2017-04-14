//
//  render.cpp
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//  Takes a ray and gives back a pixel
//

#include "render.h"
#include "bvh.h"
#include "material.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

bvhTree *tree;

/*
struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
    double x, y, z;                  // position, also color (r,g,b)
    Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
    Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); }
    Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); }
    Vec operator*(double b) const { return Vec(x*b,y*b,z*b); }
    Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); }
    Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); }
    double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross:
    Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);}
};
*/

enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()

inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }
inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }

const bool indirectLighting = false;

const int LIGHT_TAG = 0;

vec3d radiance(const ray &r, int depth, unsigned short *Xi, bool E=true){
    
    double t;                               // distance to intersection
    int id=0;                               // id of intersected object

    //if(!intersect(r, t, id)) return Vec(0,0,0);
    
    /*
    //------NORMAL VIZ------
    float t3;
    Tri3D triangleNorm;
    if(tree->testRay(ray(vec3d(r.o.x,r.o.y,r.o.z),vec3d(r.d.x,r.d.y,r.d.z)), t3, triangleNorm))
    {
        Vec ret;
        vec3d nv4 = (triangleNorm.m_b - triangleNorm.m_a).cross(triangleNorm.m_c - triangleNorm.m_a);
        ret.x = nv4.m_x;
        ret.y = nv4.m_y;
        ret.z = nv4.m_z;
        ret.norm();
        return ret;
    }
    else
    {
        return Vec(0,0,0);
    }
    //----------------------
    */
    
    //----triangle version----
    float t2;
    
/*
    if(!tree->testRay(ray(vec3d(r.o.x,r.o.y,r.o.z),vec3d(r.d.x,r.d.y,r.d.z)), t2, triangle))
    {
        
        return Vec(0,0,0);
    }
 */
    Tri3D *ptr_to_triangle; // pointer to triangle that was intersected
    
    if(!tree->newTest(r, t2, ptr_to_triangle)) // test the ray
    {
        /*
        Vec w=r.o; // w is straight up and oriented with the surface normal
        Vec u=( ( fabs(w.x)>.1 ? Vec(0,1,0) : Vec(1,0,0) )%w ).norm();
        Vec v=w%u; // w cross u
        return(u,v,w);
         */
        // environment map can go here, but only works if indirect lighting is used
        // within the recursion, the ray will be bounced off object. If it hits no
        // triangle, then it escapes to the sky and goes infinitely away to the spherical map
        // to reduce noise, direct lighting routine is used to send more rays towards the bright spots
        return vec3d(0,0,0);
    }
    
    //Tri3D &triangle = *ptr_to_triangle; // create a reference to the triangle that was intersected
    
    float light_em = 0.75;
    if(ptr_to_triangle->tag == LIGHT_TAG)
    { // hit a light directly so make it visible
        if(E)
        {
            return vec3d(light_em,light_em,light_em);
        }
        else
        {
            return vec3d(0,0,0);
        }
    }
    
    //return Vec(t2/600,0,0);
    //return Vec(1,0,0);
    t = t2;
    id = 8;
    //------------------------
    
 
    vec3d x=r.origin+r.dir*t; // point of intersection


    //----triangle version----
    vec3d n;
    vec3d nv3 = (ptr_to_triangle->m_b - ptr_to_triangle->m_a).cross(ptr_to_triangle->m_c - ptr_to_triangle->m_a);
    n = nv3;
    n.norm();

    //------------------------
    
    vec3d nl = n.dot(r.dir) < 0 ? n : n*(-1); // dot product gives us information on angle between two vectors
                                          // dot is how parallel they are
                                          // compare normal and ray direction to determine if going into / out of object
                                          // flips the normal if ray and surface normal don't agree with right into / out of direction
                                          // I think nl is always pointed up out of surface,
                                          // and n depends on how ray struck it (in/out of object)
                                          // n is what tells you where ray is going in
    


    
    //----triangle version----
    vec3d emission = vec3d(0,0,0);
    vec3d color    = vec3d(0.75,0,0.3);
    Refl_t material_type = DIFF;
    //------------------------
    
    double p = color.m_x>color.m_y && color.m_x>color.m_z ? color.m_x : color.m_y>color.m_z ? color.m_y : color.m_z; // max reflection
    /*
    if (++depth>5)
    {
        if (erand48(Xi)<p) color=color*(1/p); else return emission; //R.R.
    }
     */
    if(++depth > 5)
    { // max bounces
        return vec3d(0,0,0);
    }
    //EXPLICIT LIGHTING:
    //if (++depth>5||!p) if (erand48(Xi)<p) f=f*(1/p); else return obj.e*E;
    
    // Eventually this stuff will be pulled from the triangle pointer
    if (material_type == DIFF)
    {
        
        // Ideal DIFFUSE reflection
        
        //erand48 : random number between 0.0 and 1.0
        
        double r1  = 2*M_PI*erand48(Xi);  // random angle
        double r2  = erand48(Xi);         // random number
        double r2s = sqrt(r2);            // random distance from center of hemisphere
        
        // use normal to find orthonormal coordinate frame (makes)
        vec3d w=nl; // w is straight up and oriented with the surface normal
        vec3d u=( ( fabs(w.m_x)>.1 ? vec3d(0,1,0) : vec3d(1,0,0) ).cross(w) ).norm();
        vec3d v=w.cross(u); // w cross u

        vec3d d = ( u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2) ).norm(); // make random reflected
                                                                           // ray (formula based on realistic ray tracing)
 
        //----explicit----
        
        // For picking the lights, you might be able to just get away with uniform
        // probability at first. 1/(number of lights) is what the PDF would return
        // by selecting any light. For the actual selection, it would split a 0 to 1
        // interval up into number of light segments, and cast a random number to find
        // where in that region a light is picked.
        // Might be good to make another file for lights that contains the PDF function
        // and whatever else is needed. The function would take in the scene graph to get
        // all its information for number of lights and whatnot to calculate the PDF at
        // whatever light.
        // The scene graph could also have a "render data" node that contains things
        // useful to the render engine. Like, number of lights, precalculated things, etc.
        // Might be good to house the BVH tree in there too. That way the render function
        // takes in a scene and outputs the render.

        Tri3D triL = tree->m_tris[LIGHT_TAG];

        vec3d e;
        float light_emitted = light_em;
        float BRDF = 1/M_1_PI; // perfect lambert
        
        float a = (triL.m_a - triL.m_b).length(); // side a
        float b = (triL.m_b - triL.m_c).length(); // side b
        float c = (triL.m_c - triL.m_a).length(); // side c
        float p = a + b + c; // sum of sides
        float s = p/2;       // half of the sum of the sides

        float tri_area = sqrt(s*(s-a)*(s-b)*(s-c)); // heron's formula

        //----make a random point on the light triangle----
        float tri_r1 = erand48(Xi);
        float tri_r2 = erand48(Xi);

        if(tri_r1 + tri_r2 >= 1)
        {
            tri_r1 = 1 - tri_r1;
            tri_r2 = 1 - tri_r2;
        }
        
        vec3d x_light_random = triL.m_a + tri_r1*(triL.m_b - triL.m_a) + tri_r2*(triL.m_c - triL.m_a);
        //--------------------------------------------------
        
        vec3d object_normal;
        object_normal.m_x = nl.m_x;
        object_normal.m_y = nl.m_y;
        object_normal.m_z = nl.m_z;

        vec3d light_normal =  (triL.m_b - triL.m_a).cross(triL.m_c - triL.m_a);
        
        float t1 = 0.0;
        
        float light_out;
        vec3d dd = x_light_random - x;
        
        if(light_normal.dot(dd) > 0)
        { // make the light emit from whatever direction light ray hits it (unidirectional light)
          // disable this to emit from just one side
            light_normal = -1*light_normal; // flip light normal (depends on winding of light)
        }
        
        object_normal = object_normal.norm(); // normalize the object's normal
        light_normal = light_normal.norm();   // normalize the light's normal

        //---GGX REFLECTION---
        float alpha = 0.9;
        //vec3d m = generate_microfacet(nl,alpha,Xi);
        
        // incident is where photon came from
        // outgoing is where photon bounced to
        vec3d o = r.origin - x;       // vector from hit point to where ray is coming from (camera receives photon)
        vec3d i = x_light_random - x; // vector from hit point to random light (photon came from light)
        float BRDFspec = GGX_BRDF(i, o, nl, alpha); // how much light transferred from this particular i/o configuration?
        BRDF = BRDFspec;
        //-------------------

        Tri3D *ptr_to_tri_hit; // points to the triangle that was hit
        bool hit = tree->newTest(ray(x, dd), t1, ptr_to_tri_hit); // test the ray
        //Tri3D &triHit = *ptr_to_tri_hit; // create a reference to the triangle that was hit

        if(ptr_to_tri_hit->tag == triL.tag && hit)
        {
            light_out = (BRDF*light_emitted*(object_normal.dot(dd))*(-1*light_normal.dot(dd))*tri_area)/
            (dd.length()*dd.length()*dd.length()*dd.length());

            if(object_normal.dot(dd)/dd.length() < 0)
            {
                light_out = 0;
            }
            if(-1*light_normal.dot(dd)/dd.length() < 0)
            {
                light_out = 0;
            }
            
        }
        else
        {
            light_out = 0;
        }

        e.m_x = e.m_x + color.m_x*light_out;
        e.m_y = e.m_y + color.m_y*light_out;
        e.m_z = e.m_z + color.m_z*light_out;

        vec3d dl = e;
        vec3d gl(0,0,0);
        if(indirectLighting)
        {
            gl = color.mult(radiance(ray(x,d),depth,Xi,0)); // don't include emission hits
            
        }
        // There is no difference in brightness, dl is just less noisy
        // so it LOOKS less bright. Confirmed by making no randomness in dl to light ray
        
        /*
        vec3d micro = generate_microfacet(nl, i, alpha, Xi);
        vec3d is = (r.origin - x).norm();       // incident
        vec3d os = scatterDirection(is, micro).norm(); // outgoing based on microfacet and incident
        BRDF = GGX_BRDF(is, os, micro, alpha);
        float lo = BRDF*nl.dot(os);
        float PROB = GGX_PDF(is, os, micro, nl, alpha); // get the probability of this selection using the PDF function
        gl = lo*color.mult(radiance(ray(x,os), depth, Xi))/PROB;
        dl = dl*0; // disable dl
        */

        //if(tree->newTest(ray(x,os), t2, triangle) && triangle.tag == LIGHT_TAG) gl = vec3d(0,0,0);
        
         
        /*
        // use normal to find orthonormal coordinate frame based on w (straight up axis)
        vec3d w2 = r.dir-n*2*n.dot(r.dir); // w oriented to IDEAL (i.e. perfect, no roughness) reflection
        vec3d specColor = vec3d(0.75,0.75,0.75); // dielectric cannot have colored reflection, only metal
        vec3d spec = specColor.mult(radiance(ray(x, w2),depth,Xi));
        
        float P = 0.5;
        if(erand48(Xi) < P)
        {
            return dl*(1/P);
        }
        else
        {
            return spec*(1/(1-P));
        }
        */
            
        return (dl + gl);
        
        //------------------------
        
        //------------
        
        //return emission + color.mult(radiance(Ray(x,d),depth,Xi,0)); // cast the random ray and factor it in
                                                                     // (starts at surface of object, points to d)
        
    } else if (material_type == SPEC)
    {

        //return emission + color.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi)); // simple ray reflection based on physics law
    }
    else if (material_type == REFR)
    {
        
        // Ideal dielectric REFRACTION
        ray reflRay( x, r.dir-n*2*n.dot(r.dir) ); // same as specular reflection since glass can have both reflected and refracted
        
        bool into = n.dot(nl) > 0;   // Ray from outside going in? Compare ray surface normal and regular surface normal
        
        //--physics parameters for refraction--
        double nc=1;
        double nt=1.5;
        double nnt=into?nc/nt:nt/nc;
        double ddn=(r.dir).dot(nl);
        double cos2t;
        //-------------------------------------
        
        if ( (cos2t=1-nnt*nnt*(1-ddn*ddn))<0 )
        {
            // Total internal reflection
            // ray never made it out, just bounced around inside
            return emission + color.mult(radiance(reflRay,depth,Xi));
        }
        
        
        vec3d tdir = ( r.dir*nnt - n*( (into ? 1:-1)*(ddn*nnt+sqrt(cos2t))) ).norm();
        
        double a=nt-nc;
        double b=nt+nc;
        double R0=a*a/(b*b);
        double c = 1-(into?-ddn:tdir.dot(n));
        double Re=R0+(1-R0)*c*c*c*c*c;
        double Tr=1-Re;
        double P=.25+.5*Re;
        double RP=Re/P;
        double TP=Tr/(1-P);
        
        // Russian roulette
        return emission + color.mult(depth > 2 ? (erand48(Xi)<P ?
                                         radiance(reflRay,depth,Xi)*RP:radiance(ray(x,tdir),depth,Xi)*TP):
                                         radiance(reflRay,depth,Xi)*Re+radiance(ray(x,tdir),depth,Xi)*Tr);
    }
}

void renderer::render(Tri3D tris[], int tris_cnt)
{
    
    for (int i = 0; i < tris_cnt; i++) {
        tris[i].m_a = tris[i].m_a + vec3d(75.0,-36.0,-200.0);
        tris[i].m_b = tris[i].m_b + vec3d(75.0,-36.0,-200.0);
        tris[i].m_c = tris[i].m_c + vec3d(75.0,-36.0,-200.0);
        
        tris[i].m_ax = tris[i].m_a.m_x;
        tris[i].m_ay = tris[i].m_a.m_y;
        tris[i].m_az = tris[i].m_a.m_z;
        tris[i].m_bx = tris[i].m_b.m_x;
        tris[i].m_by = tris[i].m_b.m_y;
        tris[i].m_bz = tris[i].m_b.m_z;
        tris[i].m_cx = tris[i].m_c.m_x;
        tris[i].m_cy = tris[i].m_c.m_y;
        tris[i].m_cz = tris[i].m_c.m_z;
    }
 
    tree = new bvhTree(tris,tris_cnt); // create new BVH tree with triangles

    int w=WIDTH, h=HEIGHT;
    int samps = 2; // samples per subpixel
    //float FOV = 0.5135;
    float FOV = 0.5;
    ray cam(vec3d(50,52,295.6), vec3d(0,-0.042612,-1).norm()); // cam pos, dir
    vec3d cx=vec3d(w*FOV/h,0,0), cy=(cx%cam.dir).norm()*FOV, r, *c=new vec3d[w*h];
//#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP
    for (int y=0; y<h; y++){                       // Loop over image rows
        fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*y/(h-1));
        for (unsigned short x=0, Xi[3]={0,0,static_cast<unsigned short>(y*y*y)}; x<w; x++)   // Loop cols
            for (int sy=0, i=(h-y-1)*w+x; sy<2; sy++)     // 2x2 subpixel rows
                for (int sx=0; sx<2; sx++, r=vec3d()){        // 2x2 subpixel cols
                    for (int s=0; s<samps; s++){
                        double r1=2*erand48(Xi), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
                        double r2=2*erand48(Xi), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
                        vec3d d = cx*( ( (sx+.5 + dx)/2 + x)/w - .5) +
                        cy*( ( (sy+.5 + dy)/2 + y)/h - .5) + cam.dir;

                        r = r + radiance(ray(cam.origin+d*140,d.norm()),0,Xi)*(1./samps); // Camera rays are pushed ^^^^^
                                                                                            // forward to start in interior
                    }
                    c[i] = c[i] + vec3d(clamp(r.m_x),clamp(r.m_y),clamp(r.m_z))*.25;
                }
    }
 
    for (int i = 0; i<w*h; i++) {
        pixles[i].r = toInt(c[i].m_x);
        pixles[i].g = toInt(c[i].m_y);
        pixles[i].b = toInt(c[i].m_z);
    }
}
