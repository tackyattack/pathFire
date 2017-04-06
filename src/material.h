//
//  material.h
//  pathTrace
//
//  Created by HENRY BERGIN on 1/9/17.
//
//

#ifndef material_h
#define material_h

#include "3dTypes.h"

float chi_plus(float a);
float GGX_F(vec3d v, vec3d m, float n1, float n2);
float GGX_G(vec3d v, vec3d n, vec3d m, float alpha);
float GGX_D(vec3d n, vec3d m, float alpha);
float GGX_PDF(vec3d i, vec3d o, vec3d n, vec3d m, float alpha);
vec3d generate_microfacet(vec3d n, vec3d v, float alpha, unsigned short *Xi);
vec3d scatterDirection(vec3d i, vec3d m);
vec3d transmissionDirection(vec3d i, vec3d m, float ni, float no);
float GGX_BRDF(vec3d i, vec3d o, vec3d n, float alpha);

#endif /* material_h */
