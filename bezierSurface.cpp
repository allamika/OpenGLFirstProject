#include "bezierSurface.h"

float verticesS[] = {
     // position        //color
    -0.75,-0.75f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom left
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // top left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // top right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // bottom right
     0.75,-0.75f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // bottom right
    
    -0.75,-1.75f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
    -0.5f, -1.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
    -0.5f,  1.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // top left
     0.5f,  1.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // top right
     0.5f, -1.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // bottom right
     0.75,-1.75f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // bottom right
     
    -0.75,-0.75f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
    -0.5f, -0.5f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // top left
     0.5f,  0.5f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // top right
     0.5f, -0.5f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom right
     0.75,-0.75f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom right
};

unsigned int* BezierSurface::pascal(unsigned int row){
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


void BezierSurface::calcBezier(float* ptControle, unsigned int nbPtControle, unsigned int nbAttribut, unsigned int resolution, float* targetVertice){
	//init pascal triangle
	unsigned int* pascal = BezierSurface::pascal(nbPtControle-1);
	
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
	free(pascal);
}

void BezierSurface::calcNormal(float* vertice, unsigned int lengthVertice, unsigned int* indice, unsigned int lengthIndice, unsigned int nbAttribut, unsigned int offsetNormal){
	
	//std::cout << "start normal cumputation : for " << lengthIndice/3 << " triangle on " << lengthVertice << " point" << std::endl;	
	
	unsigned int* countNormal = (unsigned int*)malloc(lengthVertice*sizeof(unsigned int));
	for(int i = 0; i < lengthVertice;i++){
		countNormal[i] = 0;
	}
	
	for(int i = 0; i < lengthIndice/3; i++){
		int ip1 = indice[i*3] * nbAttribut;
		int ip2 = indice[i*3+1] * nbAttribut;
		int ip3 = indice[i*3+2] * nbAttribut;
		glm::vec3 U(vertice[ip2] - vertice[ip1], vertice[ip2+1] - vertice[ip1+1], vertice[ip2+2] - vertice[ip1+2]);
		glm::vec3 V(vertice[ip3] - vertice[ip1], vertice[ip3+1] - vertice[ip1+1], vertice[ip3+2] - vertice[ip1+2]);
		float Nx = U.y*V.z + U.z*V.y;
		float Ny = U.z*V.x + U.x*V.z;
		float Nz = U.x*V.y + U.y*V.x;
		
		float norm = sqrt( Nx*Nx + Ny*Ny + Nz*Nz );
		
		if(norm != 0){
			Nx /= norm;
			Ny /= norm;
			Nz /= norm;
		}
		
		vertice[ip1 + offsetNormal] -= Nx;
		vertice[ip2 + offsetNormal] -= Nx;
		vertice[ip3 + offsetNormal] -= Nx;
		vertice[ip1 + offsetNormal+1] += Ny;
		vertice[ip2 + offsetNormal+1] += Ny;
		vertice[ip3 + offsetNormal+1] += Ny;
		vertice[ip1 + offsetNormal+2] += Nz;
		vertice[ip2 + offsetNormal+2] += Nz;
		vertice[ip3 + offsetNormal+2] += Nz;
		
		countNormal[ip1/nbAttribut]++;
		countNormal[ip2/nbAttribut]++;
		countNormal[ip3/nbAttribut]++;
		
	//	std::cout << ip1 + offsetNormal+2 << " [" << ip1/nbAttribut << ", " << ip2/nbAttribut << ", " << ip3/nbAttribut << "]";
	//	std::cout << " [" << Nx << ", " << Ny << ", " << Nz << "]" << std::endl;
	}
	
	for(int i = 0; i < lengthVertice; i++){
		vertice[i*nbAttribut + offsetNormal] /= countNormal[i];
		vertice[i*nbAttribut + offsetNormal+1] /= countNormal[i];
		vertice[i*nbAttribut + offsetNormal+2] /= countNormal[i];
	//	std::cout << "mean: " << " [" << i*nbAttribut + offsetNormal << ", " << i*nbAttribut + offsetNormal+1 << ", " << i*nbAttribut + offsetNormal+2 << "]" << std::endl;
	}
	free(countNormal);
	//std::cout << "normal cumputation done" << std::endl;
}


void BezierSurface::calcParaCoord(float* vertice, int resolutionU, int resolutionV, unsigned int nbAttribut, unsigned int offsetParaCoord){
	std::cout << "calcul para sur: " << resolutionU << ", " << resolutionV << std::endl;
	for(int i = 0; i< resolutionU+1; i++){
		for(int j = 0; j< resolutionV+1; j++){
			std::cout << "cible: " << nbAttribut*(resolutionV+1)*i + nbAttribut*j + offsetParaCoord << ", value: " << (float)i/(float)resolutionU << std::endl;
			vertice[nbAttribut*(resolutionV+1)*i + nbAttribut*j + offsetParaCoord] = (float)i/(float)resolutionU;
			vertice[nbAttribut*(resolutionV+1)*i + nbAttribut*j + offsetParaCoord+1] = (float)j/(float)resolutionV;
		}
	}
}

BezierSurface::BezierSurface(int resolutionU, int resolutionV){
	unsigned int nbAttribut = 8;
	unsigned int offsetNormal = 3;
	unsigned int offsetParaCoord = 6;
	unsigned int nbControleLine = 3;
	unsigned int nbPtC = sizeof(verticesS)/sizeof(float)/nbAttribut;
	//std::cout << "sizeof(verticesS): " << sizeof(verticesS) << std::endl;
	//std::cout << "nbPtC: " << nbPtC << std::endl;
	unsigned int nbPtCInLine = nbPtC / nbControleLine;
	//std::cout << "nbPtCInLine: " << nbPtCInLine << std::endl;
	float* firstAxe = (float*)malloc((resolutionU+1) * nbAttribut * nbControleLine * sizeof(float));
	
	float* cvertices = (float*)malloc(sizeof(verticesS));
	for(int i = 0; i<sizeof(verticesS)/sizeof(verticesS[0]); i++){
		cvertices[i] = verticesS[i];
	}
	
	
	for(int i = 0; i<nbControleLine; i++){
		for(int j = 0; j<nbAttribut; j++){
			firstAxe[i*nbAttribut*(resolutionU+1)+j] = cvertices[i*nbAttribut*nbPtCInLine+j];
		}
		BezierSurface::calcBezier(&cvertices[i*nbAttribut*nbPtCInLine], nbPtCInLine, nbAttribut, resolutionU, &firstAxe[i*nbAttribut*(resolutionU+1)+nbAttribut]);
		for(int j = 0; j<nbAttribut; j++){		
			firstAxe[(i+1)*nbAttribut*(resolutionU+1)-nbAttribut+j] = cvertices[(i+1)*nbAttribut*nbPtCInLine-nbAttribut+j];
		}
	}
	
	//std::cout << "firstAxe: " << std::endl;
	for(int i = 0; i<(resolutionU+1) * nbAttribut * nbControleLine; i++){
	//	std::cout << firstAxe[i] << ", ";
		if(i%8 == 7){
	//		std::cout << std::endl;
		}
	}
	
	float* firstAxec = (float*)malloc((resolutionU+1) * nbAttribut * nbControleLine * sizeof(float));
	for(int i = 0; i<(resolutionU+1); i++){
		for(int j = 0; j<nbControleLine; j++){
			for(int k = 0; k<nbAttribut; k++){
				firstAxec[i*nbControleLine*nbAttribut+j*nbAttribut+k] = firstAxe[j*(resolutionU+1)*nbAttribut+i*nbAttribut+k];
			}
		}
	}
	
	for(int i = 0; i<(resolutionU+1) * nbAttribut * nbControleLine; i++){
		if(i%6 == 0){
			//std::cout << "[" << firstAxec[i] << ", ";
		}
		else if(i%6 == 5){
			//std::cout << firstAxec[i] << "]";
			if(i%18 == 17){
				//std::cout << std::endl;
			}
		}
		else{
			//std::cout << firstAxec[i] <<  ", ";
		}
		
	}
	
	float* secondAxe = (float*)malloc((resolutionV+1) * nbAttribut * (resolutionU+1) * sizeof(float));
	for(int i = 0; i<(resolutionU+1); i++){
		for(int j = 0; j<nbAttribut; j++){
			secondAxe[i*nbAttribut*(resolutionV+1)+j] = firstAxec[i*nbAttribut*nbControleLine+j];
			//std::cout << "case " << i*nbAttribut*(resolutionV+1)+j << ": " << firstAxec[i*nbAttribut*nbControleLine+j] << " de la case: " << i*nbAttribut*nbControleLine+j << std::endl;
		}
		BezierSurface::calcBezier(&firstAxec[i*nbAttribut*nbControleLine], nbControleLine, nbAttribut, resolutionV, &secondAxe[i*nbAttribut*(resolutionV+1)+nbAttribut]);
		for(int j = 0; j<nbAttribut; j++){		
			secondAxe[(i+1)*nbAttribut*(resolutionV+1)-nbAttribut+j] = firstAxec[(i+1)*nbAttribut*nbControleLine-nbAttribut+j];
		}
	}
	
	//std::cout << "second axes: " << std::endl;
	for(int i = 0; i<(resolutionV+1) * nbAttribut * (resolutionU+1); i++){
	//	std::cout << secondAxe[i] << ", ";
		if(i%8 == 7){
	//		std::cout << std::endl;
		}
	}
	
	
	//std::cout << "secondAxis done" << std::endl;
	int nbTriangle = (resolutionV)*(resolutionU)*6;
	unsigned int* pindices = (unsigned int*)malloc(nbTriangle*sizeof(unsigned int));
	//std::cout << "nb alloc: " << (resolutionV)*(resolutionU-1)*3 << std::endl;
	for(int i = 0; i<(resolutionU); i++){
		pindices[i*resolutionV*3] = i*(resolutionV+1);
		pindices[i*resolutionV*3+1] = i*(resolutionV+1)+1;
		pindices[i*resolutionV*3+2] = (i+1)*(resolutionV+1);
		for(int j = 1; j<resolutionV; j++){
			pindices[(i*resolutionV+j)*3] = i*(resolutionV+1)+j;
			pindices[(i*resolutionV+j)*3+1] = i*(resolutionV+1)+j+1;
			pindices[(i*resolutionV+j)*3+2] = (i+1)*(resolutionV+1)+j;
			
		}
	}
	
	
	for(int i = 0; i<(resolutionU); i++){
		//pindices[nbTriangle/2 + i*resolutionV*3] = i*(resolutionV+1)+3;
		//pindices[nbTriangle/2 + i*resolutionV*3+1] = i*(resolutionV+1)-1+3;
		//pindices[i*resolutionV*3+2] = (i-1)*(resolutionV+1)+1+3;
		for(int j = 0; j<resolutionV; j++){
			pindices[nbTriangle/2 + (i*resolutionV+j)*3] = (i+1)*(resolutionV+1)+j+1;
			//std::cout << "truc: " << (i+1)*(resolutionV+1)+j+1 << " at " << nbTriangle/2 + (i*resolutionV+j)*3 << std::endl;
			pindices[nbTriangle/2 + (i*resolutionV+j)*3+1] = (i+1)*(resolutionV+1)+j;
			//std::cout << "truc: " << (i+1)*(resolutionV+1)+j << " at " << nbTriangle/2 + (i*resolutionV+j)*3+1 << std::endl;
			pindices[nbTriangle/2 + (i*resolutionV+j)*3+2] = (i)*(resolutionV+1)+j+1;
			//std::cout << "truc: " << (i)*(resolutionV+1)+j+1 << " at " << nbTriangle/2 + (i*resolutionV+j)*3+2 << std::endl;
			
		}
	}
	
	calcNormal(secondAxe, (resolutionV+1) * (resolutionU+1), pindices, nbTriangle, nbAttribut, offsetNormal);
	
	calcParaCoord(secondAxe, resolutionU, resolutionV, nbAttribut, offsetParaCoord);
	
	
	
	
	//std::cout << "ok" << std::endl;
	
	free(cvertices);
	free(firstAxe);
	free(firstAxec);
	
	Vertices = std::make_tuple(secondAxe, ((resolutionV+1) * nbAttribut * (resolutionU+1) * sizeof(float)));
	Indices =  std::make_tuple(pindices, (nbTriangle*sizeof(unsigned int)));
}

std::tuple<float*, int> BezierSurface::getVertices(){
	return Vertices;
}

std::tuple<unsigned int*, int> BezierSurface::getIndices(){
	return Indices;
}
