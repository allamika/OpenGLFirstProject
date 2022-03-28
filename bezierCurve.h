#ifndef BEZIER_CURVE_H
#define BEZIER_CURVE_H
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>
#include <cmath>
  

class BezierCurve
{
public:
	//verctices
	std::tuple<float*, int> Vertices;
	//indices
	std::tuple<unsigned int*, int> Indices;
	// constructor build vertices and indice of bezier curve
	BezierCurve(unsigned int resolution);

	std::tuple<float*, int> getVertices();

	std::tuple<unsigned int*, int> getIndices();
private:
	//return the n's row of pascal triangle
	unsigned int* pascal(unsigned int row);
	
	void calcBezier(float* ptControle, unsigned int nbPtControle, unsigned int nbAttribut, unsigned int resolution, float* targetVertice);
};
  
#endif
