#ifndef BEZIER_SURFACE_H
#define BEZIER_SURFACE_H
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>
#include <cmath>
#include <glm/glm.hpp>


  

class BezierSurface
{
public:
	//verctices
	std::tuple<float*, int> Vertices;
	//indices
	std::tuple<unsigned int*, int> Indices;
	// constructor build vertices and indice of bezier curve
	BezierSurface(int resolutionU, int resolutionV);

	std::tuple<float*, int> getVertices();

	std::tuple<unsigned int*, int> getIndices();
private:
	//return the n's row of pascal triangle
	unsigned int* pascal(unsigned int row);
	
	void calcBezier(float* ptControle, unsigned int nbPtControle, unsigned int nbAttribut, unsigned int resolution, float* targetVertice);
	
	void calcNormal(float* vertice, unsigned int lengthVertice, unsigned int* indice, unsigned int lengthIndice, unsigned int nbAttribut, unsigned int offsetNormal);
};
  
#endif
