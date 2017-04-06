class vec
{
  float x;
  float y;
  float z;
  vec(float in_x, float in_y, float in_z)
  {
    x = in_x;
    y = in_y;
    z = in_z;
  }
  vec(vec b)
  {
    x = b.x;
    y = b.y;
    z = b.z;
  }
};
vec ADD(vec a, float b)
{
  vec ret = new vec(0,0,0);
  ret.x = a.x + b;
  ret.y = a.y + b;
  ret.z = a.z + b;
  return ret;
}
vec ADD(vec a, vec b)
{
  vec ret = new vec(0,0,0);
  ret.x = a.x + b.x;
  ret.y = a.y + b.y;
  ret.z = a.z + b.z;
  return ret;
}
vec SUB(vec a, vec b)
{
  vec ret = new vec(0,0,0);
  ret.x = a.x - b.x;
  ret.y = a.y - b.y;
  ret.z = a.z - b.z;
  return ret;
}
vec MULT(vec a, float b)
{
  vec ret = new vec(0,0,0);
  ret.x = a.x * b;
  ret.y = a.y * b;
  ret.z = a.z * b;
  return ret;
}
vec MULT(vec a, vec b)
{
  vec ret = new vec(0,0,0);
  ret.x = a.x * b.x;
  ret.y = a.y * b.y;
  ret.z = a.z * b.z;
  return ret;
}
vec CROSS(vec a, vec b)
{
  vec ret = new vec(0,0,0);
  ret.x = a.y * b.z - a.z*b.x;
  ret.y = a.z * b.x - a.x * b.z;
  ret.z = a.x * b.y - a.y * b.x;
  return ret;
}
vec NORM(vec a)
{
  vec unitVecP = new vec(a);
  float len = sqrt(unitVecP.x*unitVecP.x + unitVecP.y*unitVecP.y + unitVecP.z*unitVecP.z);
  unitVecP.x = unitVecP.x/len;
  unitVecP.y = unitVecP.y/len;
  unitVecP.z = unitVecP.z/len;
  return unitVecP;
}
float DOT(vec a, vec b)
{
  return a.x*b.x+a.y*b.y+a.z*b.z;
}
class ray
{
  vec origin;
  vec dir;
  ray(vec o, vec d)
  {
    origin = o;
    dir = d;
  }
};

void drawPixel(float x, float y, float z)
{
  //----pixel----
  pushMatrix();
  float size = 0.01;
  translate(x,y,z);
  noStroke();
  fill(255,0,0);
  sphere(size);
  popMatrix();
  //------------
}

void drawPixel(vec p)
{
  drawPixel(p.x,p.y,p.z);
}

void drawUnitSphere()
{
  pushMatrix();
  translate(0,0,0);
  noStroke();
  fill(0,0,255);
  sphere(1);
  popMatrix();
}
void drawLine(vec p1, vec p2)
{
  
  drawPixel(p1);

  vec unitVecP = new vec(p2);
  float len = sqrt(unitVecP.x*unitVecP.x + unitVecP.y*unitVecP.y + unitVecP.z*unitVecP.z);
  unitVecP.x = unitVecP.x/len;
  unitVecP.y = unitVecP.y/len;
  unitVecP.z = unitVecP.z/len;
  
  int done = 0;
  float interval = 0;
  //println(unitVecP.x + " " + unitVecP.y + " " + unitVecP.z);
  vec pos = new vec(0,0,0);
  vec increment = new vec(p1);
  for(int i = 0; i < 500 && (done == 0); i++ )
  {
    interval = interval + 0.01;
    pos = ADD(p1,MULT(unitVecP,interval));
    increment.x = p1.x + pos.x;
    increment.y = p1.y + pos.y;
    increment.z = p1.z + pos.z;
    drawPixel(increment);
    if(pos.x > p2.x) done = 1;
  }

}

void drawRay(ray r)
{

  drawLine(r.origin, ADD(r.dir,MULT(r.dir,100000)) );
}

float erand48()
{
  return random(0,1000)/1000;
}

vec cosine_hemisphere(vec normal)
{
  float x1 = erand48();
  float x2 = erand48();
  
  float theta = acos(sqrt(1-x1));
  float phi = 2*PI*x2;
  
  float xs = sin(theta)*cos(phi);
  float ys = cos(theta);
  float zs = sin(theta)*sin(phi);
  
  vec h = new vec(normal);
  if( abs(h.x) <= abs(h.y) && abs(h.x) <= abs(h.z) )
  {
    h.x = 1.0;
  }
  else if( abs(h.y) <= abs(h.x) && abs(h.y) <= abs(h.z) )
  {
    h.y = 1.0;
  }
  else
  {
    h.z = 1.0;
  }
  
  vec x = NORM(CROSS(h,normal));
  vec z = NORM(CROSS(x,normal));
  vec dir = ADD(ADD(MULT(x,xs),MULT(normal,ys)),MULT(z,zs));
  return dir;
}

class sc
{ // spherical coordinate
  float theta;
  float phi;
  sc(float phi_in, float theta_in)
  {
    theta = theta_in;
    phi = phi_in;
  }
};
float importance_sample_ggx(float alpha, float [] pdf) // pass pdf by reference, return altitude angle
{
  float x2 = erand48();
  
  float theta_num = 1 - x2;
  float theta_den = (alpha*alpha - 1)*x2 + 1;
  float theta = acos(theta_num/theta_den);
  
  float pdf_num = alpha*alpha*cos(theta)*sin(theta);
  float pdf_den = PI*(cos(theta)*cos(theta)*(alpha-1)+1)*(cos(theta)*cos(theta)*(alpha-1)+1);
  pdf[0] = pdf_num / pdf_den;
  return theta;
}

float D_GGX(vec m, vec n, float alpha)
{
  float nm = DOT(n,m);
  float num = alpha*alpha;
  nm = nm * nm;
  float den = (nm*(alpha*alpha - 1) + 1);
  den = den*den*PI;
  return num/den;
}
float G_IMPLICIT(vec n, vec l, vec v)
{
  return DOT(n,l)*DOT(n,v);
}
float GGX_BRDF(vec m, vec n, vec l, vec v, float alpha)
{ // excludes fresnal
  float num = D_GGX(m,n,alpha)*G_IMPLICIT(n,l,v);
  return num;
}
void setup()
{
  size(640, 360, P3D);
  background(255);
  translate(width/2, height/2, 0);
  rotateZ(PI);
  scale(100);
  
  drawUnitSphere();

  //drawPixel(0,0,1);
  
  vec v = new vec(1,0,0);
  vec n = new vec(0,1,0);
  vec a = n;
  vec b = CROSS(a,v);
  

  drawRay(new ray(new vec(0,0,0), n));
  drawRay(new ray(new vec(0,0,0), b));

}