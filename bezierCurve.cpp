#include "bezierCurve.h"


unsigned int* BezierCurve::pascal(unsigned int row){
	unsigned int* pascal = (unsigned int*) malloc((row+1) * sizeof(unsigned int));
	pascal[0] = 1;
	for(int i = 1; i<=row; i++){
		pascal[i] = 1;
		for(int j = i-1; j>0; j--){
			pascal[j] = pascal[j-1] + pascal[j];
		}	
	}
	return pascal;
}

void BezierCurve::calcBezier(float* ptControle, unsigned int nbPtControle, unsigned int nbAttribut, unsigned int resolution, float* targetVertice){
	//init pascal triangle
	unsigned int* pascal = BezierCurve::pascal(nbPtControle-1);
	
	//float* cible = &pvertices[nbValControle];
	
	//calculate bezier curve values
	for(int i = 1; i<resolution; i++){
		float u = (float)i/(float)resolution;
		float x = 0;
		float y = 0;
		float z = 0;
		for(int j = 0; j<nbPtControle; j++){
			x += pascal[j] * std::pow(u,j) * std::pow(1-u,nbPtControle-j-1) * ptControle[nbAttribut*j];
			y += pascal[j] * std::pow(u,j) * std::pow(1-u,nbPtControle-j-1) * ptControle[nbAttribut*j+1];
			z += pascal[j] * std::pow(u,j) * std::pow(1-u,nbPtControle-j-1) * ptControle[nbAttribut*j+2];
		}
		targetVertice[nbAttribut * (i-1)] = x;
		targetVertice[nbAttribut * (i-1) + 1] = y;
		targetVertice[nbAttribut * (i-1) + 2] = z;
		targetVertice[nbAttribut * (i-1) + 3] = 0;
		targetVertice[nbAttribut * (i-1) + 4] = 0;
		targetVertice[nbAttribut * (i-1) + 5] = 0;
	}
}

BezierCurve::BezierCurve(unsigned int resolution){
	float vertices[] = {
	     // position        //color
	    -0.75,-0.75f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
	    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
	    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // top left
	     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top right
	     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom right
	     0.75,-0.75f, 0.0f, 0.0f, 1.0f, 0.0f   // bottom right
	    
	};
	unsigned int nbAttribut = 6;
	int nbValControle = sizeof(vertices)/sizeof(vertices[0]);
	int nbPtControle =  nbValControle/nbAttribut;
	int nbLineControle = ((sizeof(vertices)/sizeof(vertices[0])/nbAttribut) -1); 
	
	//copy de vertice
	float* pvertices = (float*)malloc(sizeof(vertices) + (resolution-1)*sizeof(float)*nbAttribut);
	for(int i = 0; i<sizeof(vertices)/sizeof(vertices[0]); i++){
		pvertices[i] = vertices[i];
	}

	//calculate bezier curve values
	BezierCurve::calcBezier(pvertices, nbPtControle, nbAttribut, resolution, &pvertices[nbValControle]);
	
	
	//indice for controle polynome
	unsigned int * pindices = (unsigned int*) malloc((nbLineControle+resolution) * 2 * sizeof(int));
	for(int i = 0; i<nbLineControle; i++){
		pindices[i*2] = i;
		pindices[i*2+1] = i+1;
	}
	
	//indice for bezier curve
	pindices[(nbLineControle)*2] = 0;
	pindices[(nbLineControle)*2+1] = nbLineControle + 1;
	for(int i = 1; i<resolution-1; i++){
		pindices[(i+nbLineControle)*2] = nbLineControle + i;
		pindices[(i+nbLineControle)*2+1] = nbLineControle + i + 1;
	}
	pindices[(resolution-1+nbLineControle)*2] = nbLineControle + resolution-1;
	pindices[(resolution-1+nbLineControle)*2+1] = nbLineControle;
	
	//std::cout << "nb vertices: " << (resolution-1+nbValControle) * nbAttribut << " = " << resolution-1 << "+" << nbValControle << " * " << nbAttribut << std::endl;
	
	Vertices = std::make_tuple(pvertices, ((resolution-1) * nbAttribut + nbValControle) * sizeof(float));
	Indices =  std::make_tuple(pindices, (resolution+nbLineControle) * 2 * sizeof(int));
	
}

std::tuple<float*, int> BezierCurve::getVertices(){
	return Vertices;
}

std::tuple<unsigned int*, int> BezierCurve::getIndices(){
	return Indices;
}
