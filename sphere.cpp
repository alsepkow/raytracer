#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 *
 * We utilize the line-sphere intersection algorithm from analytic geometry
 * demonstrated on the wikipedia article at the following url.
 * http://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
 *
 **********************************************************************/
float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit) {
  Vector nU = u;
  normalize(&nU);
  Vector originAndCenteDifference = get_vec(sph->center,o);
  float distance;
  float a = vec_dot(nU,nU);
  float b = 2*vec_dot(nU,originAndCenteDifference);
  float c = vec_dot(originAndCenteDifference,originAndCenteDifference);
  c      -= pow(sph->radius, 2);
  
  float descriminant = b * b - 4 *a *c;
  if(descriminant < 0) return -1.0;  //No intersections
  else 
  {
    //Solve for the smallest root
    distance = -b - sqrt(descriminant);
    distance = distance/(2*a);
    if(distance < 0.0) return -1.0; //We are behind the camera
      hit->x = o.x + distance * nU.x;
      hit->y = o.y + distance * nU.y;
      hit->z = o.z + distance * nU.z;
      return distance;
  }
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point o, Vector u, Spheres *sph, Point *hit, int i) {
  if(sph == NULL) return NULL;
  Point tempHit = {0.0,0.0,0.0};
  Spheres *closestSphere = sph;

  float closestDistance = intersect_sphere(o,u,sph,&tempHit);
  hit->x = tempHit.x;
  hit->y = tempHit.y;
  hit->z = tempHit.z;
  
  float distance2;
  while(sph->next != NULL)
  {
    //cout << "REACHED" <<endl;
    distance2 = intersect_sphere(o,u,sph->next,&tempHit);
    if(distance2 >= 0.0) 
    { 
      //cout <<"d2: " <<distance2 << endl;
      //cout <<"d1: " <<distance1 << endl;
      if(distance2 < closestDistance) 
      {
        closestSphere = sph->next;
        hit->x = tempHit.x;
        hit->y = tempHit.y;
        hit->z = tempHit.z;
        closestDistance = distance2;
        //cout << "REACHED" << endl;
      }
    }
    if(closestDistance == -1.0)
    {
      closestDistance = distance2;
      closestSphere = sph->next;
      if(closestDistance > 0.0)
      {
        hit->x = tempHit.x;
        hit->y = tempHit.y;
        hit->z = tempHit.z; 
      }
    }

    sph = sph->next;
  }

  if(closestDistance == -1.0) return NULL;
	return closestSphere;
}



/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient)[0] = amb[0];
  (new_sphere->mat_ambient)[1] = amb[1];
  (new_sphere->mat_ambient)[2] = amb[2];
  (new_sphere->mat_diffuse)[0] = dif[0];
  (new_sphere->mat_diffuse)[1] = dif[1];
  (new_sphere->mat_diffuse)[2] = dif[2];
  (new_sphere->mat_specular)[0] = spe[0];
  (new_sphere->mat_specular)[1] = spe[1];
  (new_sphere->mat_specular)[2] = spe[2];
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}
