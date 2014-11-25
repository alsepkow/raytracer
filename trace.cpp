#include <stdio.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include "global.h"
#include "sphere.h"
using namespace std;
//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector surf_norm, Spheres *sph) 
{
  RGB_float color;
  normalize(&surf_norm);

  Vector lightVector = get_vec(q,light1);
  float distance = vec_len(lightVector);
  normalize(&lightVector);

  normalize(&v);
  Vector viewerVector = vec_scale(v,-1);//get_vec(q,eye_pos);
  normalize(&viewerVector);

  Vector reflectedVector = (vec_scale(surf_norm,-2.0*vec_dot(lightVector,surf_norm)));
  reflectedVector = vec_plus(reflectedVector,lightVector);
  normalize(&reflectedVector);

  float dot1 = max(0.0f,vec_dot(lightVector,surf_norm));
  float dot2 = max(0.0f,float(vec_dot(reflectedVector,v)));

  //cout << distance << endl;
  float decay = decay_a + decay_b * distance + decay_c * distance * distance;

  float ambientComponent[3];
  float diffuseComponent[3];
  float specularComponent[3];
  float shininess = sph->mat_shineness;

  for(int i = 0; i < 3; i++)
  {
    ambientComponent[i] = (global_ambient[i] * sph->reflectance) +
                          (light1_ambient[i]*sph->mat_ambient[i]);
    diffuseComponent[i] = light1_diffuse[i]*sph->mat_diffuse[i]*dot1;
    specularComponent[i] = max(0.0f,float(pow(dot2,shininess)*light1_specular[i]*sph->mat_specular[i]));
  }

  color.r =  ambientComponent[0] + (1.0f/decay) * (diffuseComponent[0]  + specularComponent[0]);
  color.g =  ambientComponent[1] + (1.0f/decay) * (diffuseComponent[1]  + specularComponent[1]);
  color.b =  ambientComponent[2] + (1.0f/decay) * (diffuseComponent[2]  + specularComponent[2]);
	
  //cout << 1/decay << endl;
  //cout<<"RED:"<<color.r<<endl;
  //cout<<"GREEN:"<<color.g<<endl;
  //cout<<"BLUE"<<color.b<<endl;

  Point dummy;
  lightVector = vec_scale(lightVector,1);
  if(shadow_on && intersect_scene(q,lightVector,scene,&dummy,0))
  {
    color.r =  ambientComponent[0];
    color.g =  ambientComponent[1];
    color.b =  ambientComponent[2];
  }

  return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point o, Vector u,int recursiveSteps) 
{
  Spheres *sph = NULL;
  Point hit;
  RGB_float color;

  sph = intersect_scene(o,u,scene,&hit,0);
  if(sph == NULL) color = background_clr; 
  else color = phong(hit,u,sphere_normal(hit,sph),sph);

  if(color.r != 0.5 && color.g != 0.05 && color.b != 0.8)
  {
  //cout << "RED:" << color.r << " GREEN:"<< color.g << "  BLUE:"<< color.b << endl;
  }
  return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray,0);
      //ret_color = background_clr; // just background for now

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);
      
      /*
      // Checkboard for testing
      RGB_float clr = {float(i/32), 0, float(j/32)};
      ret_color = clr;*/

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}

