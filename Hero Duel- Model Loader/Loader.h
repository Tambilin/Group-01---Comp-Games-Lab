#pragma once
#include <string>
#include <vector>
#include <fstream>

#include "MTL.h"
//#include "GLEW/glew.h"
//#include "glut.h"

#include <iostream> // this is just for testing, you can remove this with all the cout's

typedef struct
{
	float x;
	float y;
	float z;
}
points;

typedef struct
{
	float vn[3]; 
}
normal;

typedef struct
{
	float vt[3]; 
}
coordinate;

typedef struct
{
	std::string name;
	points p[3]; 
	coordinate coord[3];
	normal norm[3];
}
face;

class Loader
{
public:
    /**
     *  \brief string location of the obj file
     *  
     *  In Order for you to Load the 3D object you need to specify its location 
     *  in a string. 
     *  
     *  
     */
    Loader( std::string input);

    ~Loader(void);


    void draw(); // this function takes the obj file and draws it

    //this can be ignored, it does not make any changes to the problem
    void move(float x, float y, float z);
    void getPos();
    void setPos(float x, float y, float z); // set position
    void find_box(float &maxX,float &minX,float &maxY, float &minY, float &maxZ,float &minZ);

private:




    std::ifstream m_inFile;

    // the list of vectors that i will be using
    std::vector<points> m_points;
    std::vector<normal> m_normals;
    std::vector<coordinate> m_coords;

    std::vector<MTL> m_mtl;

    std::vector<face> m_faces;

    void process(std::string input);
    void processMTL(std::string input);

    //void inputPoints(points temp);
    //void inputNormals(normal temp);
    //void inputCoordinates(coordinate temp);
    //void createFaces(face temp);


};