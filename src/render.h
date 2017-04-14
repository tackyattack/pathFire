//
//  render.h
//  pathTrace
//
//  Created by HENRY BERGIN on 12/19/16.
//
//

#ifndef render_h
#define render_h

#include "3dTypes.h"

const int WIDTH = 800;
const int HEIGHT = 400;

const int PIXLE_CNT = WIDTH*HEIGHT;
struct pixle
{
    int r;
    int g;
    int b;
};
class renderer
{
public:
    pixle pixles[PIXLE_CNT];
    void render(Tri3D tris[], int tris_cnt);
private:
    
};

#endif /* render_h */
