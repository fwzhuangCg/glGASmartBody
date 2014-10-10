//
//  Assignment6
//  Loads a simple rigged, skinned animated character
//
//  Created by George Papagiannakis
//  Copyright (c) 2012 University Of Crete & FORTH. All rights reserved.
//


// basic STL streams
#include <iostream>
#include <sstream>
#include <iosfwd>

#ifdef _WIN32
#define snprintf _snprintf
#endif

// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

// Here we decide which of the two versions we want to use
// If your systems supports both, choose to uncomment USE_OPENGL32
// otherwise choose to uncomment USE_OPENGL21
// GLView cna also help you decide before running this program:
//
#define USE_OPENGL32
//#define USE_OPENGL21

// GLFW lib
// http://www.glfw.org/documentation.html
#ifdef USE_OPENGL32
#ifndef _WIN32
#define GLFW_INCLUDE_GL3
#define USE_GL3
#define GLFW_NO_GLU
#define GL3_PROTOTYPES 1
#endif
#endif
#define GLFW_DLL //use GLFW as a dynamically linked library
#include <GL/glfw.h>

// GLM lib
// http://glm.g-truc.net/api/modules.html
#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

//local
// #define USE_MAGICK
#define USE_SOIL2
// PLEASE uncomment //#define USE_MAGICK or USE_SOIL2 in glGAHelper.h
// SINCE ONLY A SIMPLE #define USE_MAGICK or USE_SOIL2 here does not work
#include <glGA/glGAHelper.h>

#include <AntTweakBar/AntTweakBar.h>


// Various objects and effects
#define USE_MESH_DYNAMIC
#ifdef USE_MESH_DYNAMIC
#include <glGA/glGARigMesh.h>
#endif


#include "glgasmartbody.h"
#include "glgasmartbodylistener.h"

#include <sb/SBPython.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>

#include <sbm/sbm_deformable_mesh.h>
#include <sb/SBAttribute.h>
#include <sb/SBObject.h>


#define USE_MESH_DYNAMIC
#include <glm/gtc/random.hpp>


#include "BmlEventHandler.h"

#include <glGA/glGAHelper.h>
#include <glGA/PlatformWrapper.h>

GLenum          m_textureTarget;
GLuint          m_textureObj;


#define GLFW_DLL // use GLFW as a dynamically linked library
// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))



// API function prototypes
void GLFWCALL   WindowSizeCB(int, int);
void GLFWCALL   OnMouseButton(int glfwButton, int glfwAction);
void GLFWCALL   OnMousePos(int mouseX, int mouseY);
void GLFWCALL   OnMouseWheel(int pos);
void GLFWCALL   OnKey(int glfwKey, int glfwAction);
void GLFWCALL   OnChar(int glfwChar, int glfwAction);
void TW_CALL    ResetViewCB(void * /*clientData*/);
// Our function prototypes
void            setUniformMVP(GLuint, glm::vec3 const &, glm::vec3 const &);
void            quad(int a, int b, int c, int d);
void            quadTextured(int a, int b, int c, int d);
void            colorcube();
void            colorcubeTextured();
void            groundPlane();
void            init();
void            initWithPhong();
void            initWithPhongTextured();
void            initPlane();
void            initAxes();
void            initMeshStatic();
//void            initMeshDynamic();
void            displayMeshStatic();
void            displayMeshDynamic();
void            display();
void            displayPlane();
void            displayAxes();
void            displayWithPhong();
void            displayWithPhongTextured();
void            printVectorContents(std::vector<glm::vec3>& array, std::string name);
double          calcFPS(double theTimeInterval, std::string theWindowTitle);
double          calcSimpleFPS();
float           calcRunningTime();

// Our global vars
typedef glm::vec4   color4;
typedef glm::vec4   point4;
int                 Index = 0;
int                 IndexGround = 0;
const   int         NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
typedef enum        { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 } RotationAxis;
GLuint              program, programPlane, programAxes, ModelView, Projection, ModelViewPlane, View;
GLuint              ProjectionPlane, LightPosition, ModelViewAxes, ProjectionAxes;
GLuint              vao, vaoPlane, vaoAxes, m_VAO, mRig_VAO, m_Buffers[4], programRigMesh;
GLuint              buffer, bufferPlane, bufferAxes, bufferAxesColors;
GLuint              textureSampler;
GLuint              vPosition, vTexCoord, vColor, vNormal, vPos;
TwBar*              MainBar = NULL;
double              speed = 0.0;
int                 wire = 0;
int                 cameraView = 0;
// to change the camera view change the g_Zomm variable
float               g_Zoom = 600.0f, g_ZoomX = 0.0, g_ZoomY = -5.0; // Viewer position in each direction
float               g_quat[4] = { 0, 0, 0, 1 }; // null rotation
double              g_dir[4] = { 0, 0, -1, 0 };
glm::vec4           bgColor(0.0f, 0.0f, 0.4f, 1.0f);
glm::vec3           viewer_pos(0.0, 0.0, -5.0);
//glm::vec3			viewer_pos(-20.0, -20.0, -20.0);
glm::vec3           viewer_dir(0.0, 0.0, 0.0);
glm::vec3           viewer_up(0.0, 1.0, 0.0);
glm::quat           quatRot;
glm::mat4           quatToMat;
glm::mat4           model_view, model_viewPlane, model_viewAxes, view_mat;
glm::mat4           nav_mat;
glm::mat4           projection;
point4              light_position;
RotationAxis        Axis = Xaxis;
GLfloat             Theta[NumAxes] = { 0.0, 0.0, 0.0 };
bool                showMat = true;
bool                g_resetView = false;
bool                g_useGAinterp = false;
float               g_GAfactorDenominator = 2.0;
// Modelling arrays
point4              points[NumVertices];
color4              colors[NumVertices];
glm::vec3           normals[NumVertices];
glm::vec2           tex_coords[NumVertices];
Texture*            pTexture = NULL;
//TextureManager*   pTexureManager = NULL;
//GLuint              m_textureObj;
// Initial horizontal angle : toward -Z
float               phiAngle = 3.14f;
// Initial vertical angle : none
float               thetaAngle = 0.0f;
// Initial Field of View
float               initialFoV = 45.0f;
float               travelSpeed = 10.0f; // 3 units / second
float               mouseSpeed = 0.00005f;
int                 xpos, ypos, xpos2, ypos2;
static bool         bNewMousePos = true;
float               deltaTime;
int                 windowWidth = 1024, windowHeight = 768;
glm::vec3           direction, rightCamVec, up;
//double              lastTime;
double              currentTime;
bool                initViewerPosition = true;
const int           N = 64;
point4              planePoints[6 * N*N];
//Assimp related global vars

#if defined USE_MESH_STATIC 
Mesh*                   pMesh = NULL;
#endif

#if defined  USE_MESH_DYNAMIC
RigMesh*                pRigMesh = NULL;
GLuint                  m_RigBuffers[RigMesh::NUM_VBOs];
GLuint              m_boneLocation[MAX_BONES];
aiScene*            m_pScene2;
#endif

std::vector<glm::mat4>  Transforms;

GLuint              programMesh, ModelViewMesh, ProjectionMesh;
GLuint              vPositionMesh, vTexCoordMesh, vColorMesh, vNormalMesh, vBoneWeightMesh, vBoneIDMesh;
GLuint              LightPositionMesh;
GLuint              vaoMesh;
glm::mat4           model_viewMesh;
timeval             startTime;



//-------------------------------------------------------------------------------------
glGASmartBody::glGASmartBody(void)
{
	/*Initialize values of glManager
	m_NumBones|
	m_pScene = NULL|
	m_Textures.clear() |
	m_Textures.resize(0) |
	m_useGAforInterpolation|
	m_GAfactorDenominator |
	*/
	//#if defined USE_MESH_DYNAMIC
	//glGASmartBody::glManager = new RigMesh();
	//#endif
	//glManager = new RigMesh();
}
//-------------------------------------------------------------------------------------
glGASmartBody::~glGASmartBody(void)
{

}

/**/




/*
FramesPerSecond = 1/SecondsPerFrame
If you intend to make a 60fps game, your target will be 16.6666ms ; If you intend to make a 30fps game, your target will be 33.3333ms.
*/
double calcSimpleFPS()
{
	static double lastTime = glfwGetTime();
	static int nbFrames = 0;

	// Measure speed
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1 sec ago
		// printf and reset timer
		printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		nbFrames = 0;
		lastTime += 1.0;
	}

	return 0;
}

float calcRunningTime()
{
	timeval currTime;
	gettimeofday(&currTime, NULL);
	float RunningTime = (float)(currTime.tv_sec - startTime.tv_sec);
	RunningTime += ((float)(currTime.tv_usec - startTime.tv_usec)) / 1000000.0f;
	return RunningTime;
}

double calcFPS(double theTimeInterval = 1.0, std::string theWindowTitle = "NONE")
{
	// Static values which only get initialised the first time the function runs
	static double t0Value = glfwGetTime(); // Set the initial time to now
	static int    fpsFrameCount = 0;             // Set the initial FPS frame count to 0
	static double fps = 0.0;           // Set the initial FPS value to 0.0

	// Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();

	// Ensure the time interval between FPS checks is sane (low cap = 0.1s, high-cap = 10.0s)
	// Negative numbers are invalid, 10 fps checks per second at most, 1 every 10 secs at least.
	if (theTimeInterval < 0.1)
	{
		theTimeInterval = 0.1;
	}
	if (theTimeInterval > 10.0)
	{
		theTimeInterval = 10.0;
	}

	// Calculate and display the FPS every specified time interval
	if ((currentTime - t0Value) > theTimeInterval)
	{
		// Calculate the FPS as the number of frames divided by the interval in seconds
		fps = (double)fpsFrameCount / (currentTime - t0Value);

		// If the user specified a window title to append the FPS value to...
		if (theWindowTitle != "NONE")
		{
			// Convert the fps value into a string using an output stringstream
			std::ostringstream stream;
			stream << fps;
			std::string fpsString = stream.str();

			// Append the FPS value to the window title details
			theWindowTitle += " | FPS: " + fpsString;

			// Convert the new window title to a c_str and set it
			const char* pszConstString = theWindowTitle.c_str();
			glfwSetWindowTitle(pszConstString);
		}
		else // If the user didn't specify a window to append the FPS to then output the FPS to the console
		{
			std::cout << "FPS: " << fps << std::endl;
		}

		// Reset the FPS frame counter and set the initial time to be now
		fpsFrameCount = 0;
		t0Value = glfwGetTime();
	}
	else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		fpsFrameCount++;
	}

	// Return the current FPS - doesn't have to be used if you don't want it!
	return fps;
}




void setUniformMVP(GLuint Location, glm::vec3 const & Translate, glm::vec3 const & Rotate)
{
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), Translate);
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 View = glm::rotate(ViewRotateX, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 MVP = Projection * View * Model;
	//glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(MVP));
}


// Callback function called by GLFW when window size changes
void GLFWCALL WindowSizeCB(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	GLfloat aspect = GLfloat(width / height);
	// glm::mat4 perspective(float fovy, float aspect, float zNear, float zFar);
	projection = glm::perspective(45.0f, aspect, 0.01f, 500.0f);


#if defined USE_MESH_DYNAMIC
	projection = glm::perspective(45.0f, aspect, 0.1f, 900.0f);
	glUniformMatrix4fv(ProjectionMesh, 1, GL_FALSE, glm::value_ptr(projection));
#endif

	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}


// Callback function called by GLFW when a mouse button is clicked
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction)
{
	if (!TwEventMouseButtonGLFW(glfwButton, glfwAction))   // Send event to AntTweakBar
	{
		// Event has not been handled by AntTweakBar
		// Do something if needed.
		if ((glfwButton == GLFW_MOUSE_BUTTON_LEFT) && glfwAction == GLFW_PRESS)
		{
			Axis = Xaxis;
		}

		// Button released?
		else if (glfwButton == GLFW_MOUSE_BUTTON_MIDDLE && glfwAction == GLFW_PRESS)
		{
			Axis = Yaxis;
		}
		else if (glfwButton == GLFW_MOUSE_BUTTON_RIGHT && glfwAction == GLFW_PRESS)
		{
			Axis = Zaxis;
		}

	}
}


// Callback function called by GLFW when mouse has moved
void GLFWCALL OnMousePos(int mouseX, int mouseY)
{
	xpos = mouseX;
	ypos = mouseY;

	if (!TwEventMousePosGLFW(mouseX, mouseY) && cameraView)  // Send event to AntTweakBar
	{
		if (bNewMousePos) {
			xpos2 = xpos;
			ypos2 = ypos;
			bNewMousePos = false;

			// Compute new orientation
			phiAngle += mouseSpeed * float(windowWidth / 2 - xpos);
			thetaAngle += mouseSpeed * float(windowHeight / 2 - ypos);
			//std::cout<<"phiAngle: "<<phiAngle<<"  thetaAngle:"<<thetaAngle<<std::endl;
			//std::cout<<"xpos: "<<xpos<<"  ypos:"<<ypos<<std::endl;
		}

		if ((xpos != xpos2) || (ypos != ypos2)) {
			bNewMousePos = true;
		}

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		direction = glm::vec3(
			cos(thetaAngle) * sin(phiAngle),
			sin(thetaAngle),
			cos(thetaAngle) * cos(phiAngle)
			);

		// Right vector
		rightCamVec = glm::vec3(
			sin(phiAngle - 3.14f / 2.0f),
			0,
			cos(phiAngle - 3.14f / 2.0f)
			);
		//std::cout<<"rightCamVec: "<<rightCamVec.x<<", "<<rightCamVec.y<<", "<<rightCamVec.z<<std::endl;
		// Up vector
		up = glm::cross(rightCamVec, direction);
	}
}


// Callback function called by GLFW on mouse wheel event
void GLFWCALL OnMouseWheel(int pos)
{
	if (!TwEventMouseWheelGLFW(pos))   // Send event to AntTweakBar
	{
		// Event has not been handled by AntTweakBar
		// Do something if needed.
	}
}


// Callback function called by GLFW on key event
void GLFWCALL OnKey(int glfwKey, int glfwAction)
{
	if (!TwEventKeyGLFW(glfwKey, glfwAction) && cameraView)  // Send event to AntTweakBar
	{
		if (glfwKey == GLFW_KEY_ESC && glfwAction == GLFW_PRESS) // Want to quit?
			glfwCloseWindow();
		else if (glfwKey == GLFW_KEY_UP && glfwAction == GLFW_PRESS) // MOVE FORWARDS
		{
			viewer_pos += direction * deltaTime * travelSpeed;
			//std::cout<<"viewer_pos: "<<glm::to_string(viewer_pos)<<std::endl<<std::endl;
		}
		else if (glfwKey == GLFW_KEY_DOWN && glfwAction == GLFW_PRESS) // MOVE BACKWARDS
		{
			viewer_pos -= direction * deltaTime * travelSpeed;
			//std::cout<<"viewer_pos: "<<glm::to_string(viewer_pos)<<std::endl<<std::endl;
		}
		else if (glfwKey == GLFW_KEY_RIGHT && glfwAction == GLFW_PRESS) // MOVE RIGHT
		{
			viewer_pos += rightCamVec * deltaTime * travelSpeed;
			//std::cout<<"viewer_pos: "<<glm::to_string(viewer_pos)<<std::endl<<std::endl;
		}
		else if (glfwKey == GLFW_KEY_LEFT && glfwAction == GLFW_PRESS) // MOVE LEFT
		{
			viewer_pos -= rightCamVec * deltaTime * travelSpeed;
			//std::cout<<"viewer_pos: "<<glm::to_string(viewer_pos)<<std::endl<<std::endl;
		}
		else
		{
			// Event has not been handled
			// Do something if needed.
		}
	}
}


// Callback function called by GLFW on char event
void GLFWCALL OnChar(int glfwChar, int glfwAction)
{
	if (!TwEventCharGLFW(glfwChar, glfwAction))    // Send event to AntTweakBar
	{
		// Event has not been handled by AntTweakBar
		// Do something if needed.
	}
}

void TW_CALL ResetViewCB(void * /*clientData*/)
{
	g_quat[0] = 0;
	g_quat[1] = 0;
	g_quat[2] = 0;
	g_quat[3] = 1;
	g_Zoom = 40.0;

#ifdef PHONG_SHADER
	g_Zoom = -50.0;
#endif

	if (!g_resetView) {
		g_resetView = true;
	}
	else //restore manual rotation from GUI
		g_resetView = false;
}

void bindTexture(GLenum textureUnit)
{
	//std::cout << "inside the bindTexure me to textureUnit na einai " << textureUnit << std::endl;
	//if (glActiveTexture == NULL)
		//std::cout << "glActiveTexture is NULL" << std::endl;
	//else
		//std::cout << "glActiveTexture is not NULL" << std::endl;
	glActiveTexture(textureUnit);
	//std::cout << "inside the bindTexure me to textureUnit na einai " << textureUnit << std::endl;

	glBindTexture(m_textureTarget, m_textureObj);
}

void render()
{
	for (unsigned int i = 0; i < pRigMesh->m_Entries.size(); ++i)
	{
		const unsigned int materialIndex = pRigMesh->m_Entries[i].materialIndex;

		assert(materialIndex < m_Textures.size());

		if (pRigMesh->m_Textures[materialIndex]) {
			//		std::cout << "The material Index is " << materialIndex << " and the size of m_Textures is " << m_Textures.size() << std::endl;
			pRigMesh->m_Textures[materialIndex]->bindTexture(GL_TEXTURE0);

		}

		// render primitives from array data with a per-element offset
		/*
		glDrawElementsBaseVertex behaves identically to glDrawElements except that the ith element transferred by the corresponding draw call will be taken from element indices[i] + basevertex of each enabled array:

		void glDrawElementsBaseVertex(	GLenum  	mode,
		GLsizei  	count,
		GLenum  	type,
		GLvoid * 	indices,
		GLint  	basevertex);
		*/
		glDrawElementsBaseVertex(GL_TRIANGLES,
			pRigMesh->m_Entries[i].numIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int)* pRigMesh->m_Entries[i].baseIndex),
			pRigMesh->m_Entries[i].baseVertex);

	}//end for

}//end render()


#ifdef USE_MESH_DYNAMIC 
void glGASmartBody::initMeshDynamic(const std::string & objectClass)
{
	GLExitIfError();
	gettimeofday(&startTime, NULL);

	// create a client VAO (retrieve it from Mesh)
	//create a VAO
	glGenVertexArrays(1, &mRig_VAO);
	glBindVertexArray(mRig_VAO);

	// Collada-based linear-blend skinned mesh
	//pMesh->loadRigMesh("GAPlane.dae");
	bool meshLoaded = true;
	//meshLoaded = pRigMesh->loadRigMesh("../../_glGA-data/data/models/boblampclean.md5mesh");
	//meshLoaded = pRigMesh->loadRigMesh("../../_glGA-data/data/models/Andy2012.dae");

	//meshLoaded = pRigMesh->loadRigMesh("../../_glGA-data/data/models/alyson_rig_idle_metaio.dae");

	meshLoaded = pRigMesh->loadRigMesh(objectClass);

	//meshLoaded = pRigMesh->loadRigMesh("../../_glGA-data/data/models/gangnam_style_2.dae");
	//meshLoaded = pRigMesh->loadRigMesh("../../_glGA-data/data/models/GACylinder.dae");
	//meshLoaded = pRigMesh->loadRigMesh("C:/Users/DamianosMt/Downloads/Woman@idle.dae");

	if (!meshLoaded) {
		std::cout << "\t ASSIMP COULD NOT LOAD MESH!" << std::endl;
		int in;
		std::cin >> in;
		exit(EXIT_FAILURE);
	}


	//create the VBO
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_RigBuffers), m_RigBuffers);

	// Init the OpenGL 3.1 Textured Blinn-Phong shaders meshStatic.vert and meshStatic.frag
	// Load shader for Phong vertex-based lighting
	//programRigMesh = LoadShaders("C:/Users/DamianosMt/Documents/GitHub/glGA/assignments/Assignment6/meshDynamic.vert", "C:/Users/DamianosMt/Documents/GitHub/glGA/assignments/Assignment6/meshDynamic.frag");
	
	string str_vertex_filepath = getGLSLResourcePath("C:/Users/DamianosMt/Documents/GitHub/glGA/assignments/Assignment6/meshDynamic.vert");
	//cout << str_vertex_filepath << endl;
	string str_fragment_filepath = getGLSLResourcePath("C:/Users/DamianosMt/Documents/GitHub/glGA/assignments/Assignment6/meshDynamic.frag");
	//cout << str_fragment_filepath << endl;


	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(str_vertex_filepath.c_str(), std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		std::cout << "shader: " << str_vertex_filepath << " not found!" << std::endl;
		//return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(str_fragment_filepath.c_str(), std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else
	{
		std::cout << "shader: " << str_fragment_filepath << " not found!" << std::endl;
		//return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength = 10;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n\n", str_vertex_filepath.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	if (!Result)
	{
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}
	// Compile Fragment Shader
	printf("Compiling shader : %s\n\n", str_fragment_filepath.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	if (!Result)
	{
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);


	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	
	programRigMesh = ProgramID;
		
	//    programMesh = LoadShaders("vshaderSimple.vert", "fshaderSimple.frag");
	glUseProgram(programRigMesh);


	// here we get from Mesh class all Vertices, TexCoords, Normals so that we can build
	// here the VBO for all these
	// connect vertex Arrays to the shader
	// Return the location of an attribute variable
	//
	// Vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_RigBuffers[RigMesh::POS_VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pRigMesh->Positions[0]) * pRigMesh->Positions.size(), &pRigMesh->Positions[0], GL_STATIC_DRAW);
	// Connect vertex arrays to the the shader attributes: vPosition, vNormal, vTexCoord
	vPositionMesh = glGetAttribLocation(programRigMesh, "vPosition");
	glEnableVertexAttribArray(vPositionMesh);
	glVertexAttribPointer(vPositionMesh, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//stop using previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//printVectorContents(pMesh->Positions,"Positions");

	// TEXCOORD VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_RigBuffers[RigMesh::TEXCOORD_VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pRigMesh->TexCoords[0]) * pRigMesh->TexCoords.size(), &pRigMesh->TexCoords[0], GL_STATIC_DRAW);
	vTexCoordMesh = glGetAttribLocation(programRigMesh, "vTexCoord");
	glEnableVertexAttribArray(vTexCoordMesh);
	glVertexAttribPointer(vTexCoordMesh, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//stop using previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Normal VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_RigBuffers[RigMesh::NORMAL_VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pRigMesh->Normals[0]) * pRigMesh->Normals.size(), &pRigMesh->Normals[0], GL_STATIC_DRAW);
	vNormalMesh = glGetAttribLocation(programRigMesh, "vNormal");
	glEnableVertexAttribArray(vNormalMesh);
	glVertexAttribPointer(vNormalMesh, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//glVertexAttribPointer(2,3, GL_FLOAT, GL_FALSE, 0, 0);
	//stop using previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//printVectorContents(pMesh->Normals,"Normals");

	// Bone VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_RigBuffers[RigMesh::BONE_VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pRigMesh->Bones[0])*pRigMesh->Bones.size(), &pRigMesh->Bones[0], GL_STATIC_DRAW);
	vBoneIDMesh = glGetAttribLocation(programRigMesh, "BoneIDs");
	glEnableVertexAttribArray(vBoneIDMesh);
	glVertexAttribIPointer(vBoneIDMesh, 4, GL_INT, sizeof(RigMesh::VertexBoneData), BUFFER_OFFSET(0));
	vBoneWeightMesh = glGetAttribLocation(programRigMesh, "Weights");
	glEnableVertexAttribArray(vBoneWeightMesh);
	glVertexAttribPointer(vBoneWeightMesh, 4, GL_FLOAT, GL_FALSE, sizeof(RigMesh::VertexBoneData), BUFFER_OFFSET(16)); //16=4 elements in UIds[] * 4 bytes per each integer, stride is sizeof(RigMesh::VertexBoneData)
	//stop using previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RigBuffers[RigMesh::INDEX_VBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pRigMesh->Indices[0]) * pRigMesh->Indices.size(), &pRigMesh->Indices[0], GL_STATIC_DRAW);
	//stop using previous VBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Initialize shader lighting parameters
	light_position = glm::vec4(0.0, 0.0, -1.0, 0.0);
	glm::vec4 light_ambient(0.2, 0.2, 0.2, 1.0);
	glm::vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	glm::vec4 light_specular(1.0, 1.0, 1.0, 1.0);

	glm::vec4 material_ambient(0.5, 0.5, 0.5, 1.0);
	glm::vec4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	glm::vec4 material_specular(1.0, 1.0, 1.0, 1.0);
	float  material_shininess = 100.0;

	glm::vec4 ambient_product = light_ambient * material_ambient;
	glm::vec4 diffuse_product = light_diffuse * material_diffuse;
	glm::vec4 specular_product = light_specular * material_specular;

	//a flag if a texture is present
	float material_texture = 1;
	std::cout << "number of textures present: " << pRigMesh->m_Textures.size() << std::endl;
	if (pRigMesh->m_Textures.size()<2) {
		material_texture = 0;
		std::cout << "number of textures present: " << material_texture << std::endl;
	}

	// Specify the value of a uniform variable for the current program object
	// note: the use of glm::value_ptr() to return a pointer to the memory location
	// note:  glGetUniformLocation() Returns the location of a uniform variable 
	glUniform4fv(glGetUniformLocation(programRigMesh, "AmbientProduct"), 1, glm::value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(programRigMesh, "DiffuseProduct"), 1, glm::value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(programRigMesh, "SpecularProduct"), 1, glm::value_ptr(specular_product));
	glUniform1f(glGetUniformLocation(programRigMesh, "Shininess"), material_shininess);
	glUniform1f(glGetUniformLocation(programRigMesh, "TexturesPresent"), material_texture);

	// retrieve vertex shader uniform variables 
	ModelViewMesh = glGetUniformLocation(programRigMesh, "ModelView");
	ProjectionMesh = glGetUniformLocation(programRigMesh, "Projection");
	LightPositionMesh = glGetUniformLocation(programRigMesh, "LightPosition");

	// retrieve Bone IDs

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
		char Name[128];
		memset(Name, 0, sizeof(Name));
		snprintf(Name, sizeof(Name), "Bones[%d]", i);
		m_boneLocation[i] = glGetUniformLocation(programRigMesh, Name);
	}

	//std::cout<<"\t@@@initMeshDynamic(): ModelViewMesh:"<<ModelViewMesh<<" ,\nProjectionMesh: "<<ProjectionMesh<<std::endl;
	// retrieve texture Sampler2D from Mesh class
	// retrieve texture Sampler2D
	textureSampler = glGetUniformLocation(programRigMesh, "TextureSampler2D");
	// init sampler with Texture Unit 0
	glUniform1i(textureSampler, 0);
	// init sampler with Texture Unit from Mesh class

	//stop using previous VBO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindVertexArray(0);


	//GLExitIfError();
}

void displayMeshDynamic()
{
	glUseProgram(programRigMesh);
	glBindVertexArray(mRig_VAO);

	//GLExitIfError();


	// here we update all the bone transforms in the rigMesh
	//float runningTime = glfwGetTime();
	float runningTime = calcRunningTime();
	//std::cout<<" Running time: "<<runningTime<<std::endl;

	// pass the Bones transformation matrix as uniform
	// need to call pRigMesh->boneTransform() before following
	//GPTEMP: do we use same Transforms or create new in every frame?
	// GPTEMP: what if the time management of runningTime is wrong????
	//runningTime = currentTime;
	//std::cout<<" running time:"<<runnin gTime<<std::endl;
	pRigMesh->boneTransform(runningTime, Transforms);

	for (uint i = 0; i < Transforms.size(); ++i)
	{
		//SetBoneTransform(i, Transforms[i])
		//assert(i < MAX_BONES);
		// pass the bone matrix to vertex shader!
		//GLuint m_boneLocation[MAX_BONES];
		glUniformMatrix4fv(m_boneLocation[i], 1, GL_FALSE, glm::value_ptr(Transforms[i]));
	}

	static double lastTime = glfwGetTime();
	// Compute time difference between current and last frame
	currentTime = glfwGetTime();
	deltaTime = float(currentTime - lastTime);

	//Theta[Axis] += 0.01;
	Theta[Axis] += speed;
	if (Theta[Axis] >360.0)
		Theta[Axis] -= 360.0;

	// GL states
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	if (g_resetView) {
		viewer_pos = glm::vec3(0.0, 0.0, -5.0);
		model_viewMesh = (glm::translate(glm::mat4(1.0), viewer_pos));
		initViewerPosition = true;
	}
	else if (cameraView)
	{

		if (initViewerPosition)
		{
			viewer_pos += glm::vec3(0.0, 0.0, 10.0);
			initViewerPosition = false;
		}
		nav_mat = glm::lookAt(viewer_pos,           // Camera is here
			viewer_pos + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
		//glm::mat4 temp = glm::scale(glm::mat4(1.0), glm::vec3(0.3));

		model_viewMesh = nav_mat * glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.0, 0.0));// * temp; //* (glm::translate(glm::mat4(1.0), viewer_pos) *quatToMat);
		std::cout << "\nviewer_pos: " << glm::to_string(viewer_pos) << std::endl;
		std::cout << "direction: " << glm::to_string(direction) << std::endl;
		std::cout << "up: " << glm::to_string(up) << std::endl << std::endl;
	}
	else
	{
		//update light position
		for (int i = 0; i<3; i++) {
			light_position[i] = g_dir[i];
		}
		//std::cout<<"light_position: "<<glm::to_string(light_position)<<std::endl;
		//printVec4GLM("light_position: ", light_position);

		//setup MV matrix including rotation from GUI
		quatRot = glm::quat(g_quat[3], g_quat[0], g_quat[1], g_quat[2]);
		quatToMat = glm::mat4_cast(quatRot);


		// generate new modelView matrix for each frame
		viewer_pos = glm::vec3(g_ZoomX, g_ZoomY, -g_Zoom);


		model_viewMesh = (glm::translate(glm::mat4(1.0), viewer_pos) *quatToMat);
	}

	// simple test
	//glm::mat4 model_view = (glm::translate(glm::mat4(1.0), viewer_pos));
	//printMat4GML("model_view", model_view);
	//std::cout<<glm::to_string(model_view)<<std::endl;

	// pass the uniform matrix to the shader
	// void glUniformMatrix4fv(	GLint location, GLsizei count, GLboolean transpose,const GLfloat *value);
	//model_viewMesh = glm::translate(glm::mat4(1.0), viewer_pos);
	//model_viewMesh = glm::mat4(1.0) 
	//projection = glm::perspective(45.0f,aspect,1.0f,500.0f);
	glUniformMatrix4fv(ModelViewMesh, 1, GL_FALSE, glm::value_ptr(model_viewMesh));
	glUniformMatrix4fv(ProjectionMesh, 1, GL_FALSE, glm::value_ptr(projection));
	//printMat4GML("model_viewMesh",model_viewMesh);
	//printMat4GML("projection",projection);

	// pass the light position and direction
	glUniform4f(LightPositionMesh, light_position.x, light_position.y, light_position.z, light_position.w);

	//GLExitIfError();

	//update before rendering the GA rotor denominator
	pRigMesh->m_GAfactorDenominator = g_GAfactorDenominator;

	// render primitives from array data
	// finally render the Mesh with glDrawElementsBaseVertex
	// bind textures
	//pRigMesh->render();
	render();
	//glDrawArrays( GL_TRIANGLES, 0, 24 );

	//glPopAttrib();

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
	glBindVertexArray(0);

	//GLExitIfError();
	//GLCheckError();
}
#endif


int main(int argc, const char * argv[])
{

	// initialise GLFW
	int running = GL_TRUE;

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	//only for OpenGL 2.1
#ifdef USE_OPENGL21
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
#endif

	//Only for OpenGL 3.2
#ifdef USE_OPENGL32
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif

	GLFWvidmode mode;
	glfwGetDesktopMode(&mode);
	if (!glfwOpenWindow(windowWidth, windowHeight, mode.RedBits, mode.GreenBits, mode.BlueBits, 0, 32, 0, GLFW_WINDOW /* or GLFW_FULLSCREEN */))
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwEnable(GLFW_MOUSE_CURSOR);
	glfwEnable(GLFW_KEY_REPEAT);
	// Ensure we can capture the escape key being pressed below
	glfwEnable(GLFW_STICKY_KEYS);
	glfwSetMousePos(windowWidth / 2, windowHeight / 2);
	glfwSetWindowTitle("glGASmartBody Renderer");

	//calculate FPS
	std::string windowTitle = "glGASmartBody Renderer ";
	GLExitIfError();

	//init GLEW and basic OpenGL information
	// VERY IMPORTANT OTHERWISE GLEW CANNOT HANDLE GL3
#ifdef USE_OPENGL32
	glewExperimental = true;
#endif
	glewInit();
	//GLExitIfError();// according GLEW, to You might still get GL_INVALID_ENUM (depending on the version of GLEW you use), but at least it works ok from this point forward

	std::cout << "\nUsing GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	if (GLEW_VERSION_2_1)
	{
		std::cout << "\nYay! OpenGL 2.1 is supported!\n" << std::endl;
	}
	if (GLEW_VERSION_3_0)
	{
		std::cout << "Yay! OpenGL 3.0 is supported!\n" << std::endl;
	}
	if (GLEW_VERSION_3_2)
	{
		std::cout << "Yay! OpenGL 3.2 is supported!\n" << std::endl;
	}

	/*
	This extension defines an interface that allows various types of data
	(especially vertex array data) to be cached in high-performance
	graphics memory on the server, thereby increasing the rate of data
	transfers.
	Chunks of data are encapsulated within "buffer objects", which
	conceptually are nothing more than arrays of bytes, just like any
	chunk of memory.  An API is provided whereby applications can read
	from or write to buffers, either via the GL itself (glBufferData,
	glBufferSubData, glGetBufferSubData) or via a pointer to the memory.
	*/
	if (glewIsSupported("GL_ARB_vertex_buffer_object"))
		std::cout << "ARB VBO's are supported" << std::endl;
	else if (glewIsSupported("GL_APPLE_vertex_buffer_object"))
		std::cout << "APPLE VBO's are supported" << std::endl;
	else
		std::cout << "VBO's are not supported,program will not run!!!" << std::endl;

	/*
	This extension introduces named vertex array objects which encapsulate
	vertex array state on the client side. The main purpose of these
	objects is to keep pointers to static vertex data and provide a name
	for different sets of static vertex data.
	By extending vertex array range functionality this extension allows multiple
	vertex array ranges to exist at one time, including their complete sets of
	state, in manner analogous to texture objects.
	GenVertexArraysAPPLE creates a list of n number of vertex array object
	names.  After creating a name, BindVertexArrayAPPLE associates the name with
	a vertex array object and selects this vertex array and its associated
	state as current.  To get back to the default vertex array and its
	associated state the client should bind to vertex array named 0.
	*/

	if (glewIsSupported("GL_ARB_vertex_array_object"))
		std::cout << "ARB VAO's are supported\n" << std::endl;
	else if (glewIsSupported("GL_APPLE_vertex_array_object"))//this is the name of the extension for GL2.1 in MacOSX
		std::cout << "APPLE VAO's are supported\n" << std::endl;
	else
		std::cout << "VAO's are not supported, program will not run!!!\n" << std::endl;

	GLExitIfError();

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	//std::cout<<"Extensions: "<<glGetString (GL_EXTENSIONS)<<std::endl;

	//init AntTweakBar
#ifdef USE_OPENGL21
	TwInit(TW_OPENGL, NULL);      //OpenGL 2.1
#endif
#ifdef USE_OPENGL32
	TwInit(TW_OPENGL_CORE, NULL);   //OpenGL 3.2
#endif
	//fprintf(stderr, "AntTweakBar initialization failed: %s\n", TwGetLastError());

	GLExitIfError();

	//Create a glGAsb object
	glGASmartBody* glGAsb = new glGASmartBody();

#if defined USE_MESH_DYNAMIC
	pRigMesh = new RigMesh();
	
	//glGAsb->initMeshDynamic();
	//#endif

	/* Start using SmartBody */
	// the root path to SmartBody: change this to your own path
	// set the following to the location of the Python libraries. 



	//We have to set the media path
	//std::string mediaPath = "C:/Users/Solid/smartbody/data";
	std::string mediaPath = "C:/Users/DamianosMt/Documents/GitHub/glGA/_glGA-data/data/models";

	// add a message logger to stdout
	vhcl::Log::StdoutListener* stdoutLog = new vhcl::Log::StdoutListener();
	vhcl::Log::g_log.AddListener(stdoutLog);

	LOG("Loading Python...");

	// initialize the Python libraries
	initPython("C:/Users/DamianosMt/smartbody/core/smartbody/Python27/libs");



	glGAsb->m_pScene = SmartBody::SBScene::getScene();

	glGAsb->m_pScene->startFileLogging("smartbody.log");
	glGASmartBodyListener* listener = new glGASmartBodyListener(glGAsb);
	glGAsb->m_pScene->addSceneListener(listener);
	glGAsb->m_pScene->start();


	// sets the media path, or root of all the data to be used
	glGAsb->m_pScene->setMediaPath(mediaPath);


	/*From down here USE C++ instructions to load the character (Ogre uses python file instead)
	,we load the .dae file as an asset*/

	//it it not necessary to laod the assets here
	//m_pScene->loadAssetsFromPath("C:/Users/Solid/Documents/GitHub/glGA/_glGA-data/data/models/Andy2012.dae");

	// load the assets from the indicated locations
	LOG("Loading Assets...");
	glGAsb->m_pScene->loadAssets();
	// create a character
	LOG("Creating the character...");
	//create the character
	SmartBody::SBCharacter* character = glGAsb->m_pScene->createCharacter("astroBoy_walk", mediaPath + "/astroBoy_walk.dae");
	LOG("Character is created");
	// load the skeleton 
	SmartBody::SBSkeleton* skeleton = glGAsb->m_pScene->createSkeleton("C:/Users/DamianosMt/Documents/GitHub/glGA/_glGA-data/data/models/astroBoy_walk.dae");
	LOG("Skeleton is loaded");
	// attach the skeleton to the character
	character->setSkeleton(skeleton);
	LOG("Skeleton is attached");
	//attach the mesh to the character (THIS SHOULD WORK SO I CAN TAKE THE VALUES OF DeformableMesh)
	//No need to attach the mesh as 

	// create the standard set of controllers (idling, gesture, nodding, etc.)
	//character->createStandardControllers();


	/* Check when and if the over-writte functions are being used*/

	LOG("Number of Pawns are : %d ", glGAsb->m_pScene->getNumPawns());
	LOG("Number of Characters are : %d ", glGAsb->m_pScene->getNumCharacters());
	SmartBody::SBPawn *obj = glGAsb->m_pScene->createPawn("somePawn"); // onCharecterUpdate is called two times and onCreatePawn is also called
	//obj->setStringAttribute("collisionShape", "box");
	//SmartBody::SBPawn *obj2 = (SmartBody::SBPawn *) m_pScene->getObjectFromString("somePawn");//it does not get the pawn

	/*None of the below are causing the calls*/
	LOG("Now setDoubleAttribute");
	//character->setDoubleAttribute("deformableMeshScale", .01);
	LOG("After the deformableMesh Scale");

	LOG("Now setStringAttribute");
	//character->setStringAttribute("deformableMesh", "");
	LOG("After the setStringAttribute");

	LOG("Now the setStingAttribute");
	//character->setStringAttribute("displayType", "GPUMesh");
	LOG("After the setStringAttribute");

	/* Calls of the interface for the FaceAnimation*/
	LOG("Now send the BML Command");
	SmartBody::SBBmlProcessor *pros = glGAsb->m_pScene->getBmlProcessor();
	pros->execBML("astroBoy_walk", "<body posture=\"ChrBrad@Idle01\"/>");//none of the funtions are being called
	//m_pScene->getBmlProcessor->execBML("astroBoy_walk", "<body posture=\"ChrBrad@Idle01\"/>");
	LOG("After the BML command is used");

	LOG("Now for Face animation");
	SmartBody::SBFaceDefinition *f = glGAsb->m_pScene->createFaceDefinition("astroBoy_walk"); // onObjectCreate of glGASmartBody is called
	LOG("FaceAnimation created");
	LOG("Face set AU");
	f->setAU(1, "left", "");// none of the functions are being called 
	LOG("AU is setted");
	LOG("Face setViseme");
	f->setViseme("open", "");// onCharacterUpdate is being called twice
	f->setViseme("right", "");
	LOG("Viseme animation is being created");

	//some face definitons
	//character->setFaceDefinition(f); //onCharacterUpdate is called here as may times as we have set the above setAu and setViseme
	//When simulation starts the onViseme is called two times,as many times as we called the setViseme above
	LOG("After the FaceAnimation");


	/*Now check when an Event handler is being called if the onEvent is being called*/
	/*None of the below are calling the onEvent function*/
	BmlEventHandler *myHandler = new BmlEventHandler();
	//SmartBody::SBEventHandler *h = new SmartBody::SBEventHandler();
	SmartBody::SBEventManager *MEvent = glGAsb->m_pScene->getEventManager();
	MEvent->addEventHandler("bmlinstruction", myHandler);
	SmartBody::SBEvent *Event = MEvent->createEvent("bmlinstruction", "the bml instruction");
	Event->setParameters("someparameters");
	Event->setType("bmlinstruction");
	MEvent->handleEvent(Event, 0); // here the onEvent function is being called


	SmartBody::SBMotion *m = new SmartBody::SBMotion;
	m->addEvent(0.2, "sdd", "dsds", false);



	/*JointMapManager functionallities*/

	SmartBody::SBJointMapManager *jm = glGAsb->m_pScene->getJointMapManager();
	SmartBody::SBJointMap *gamap = jm->createJointMap("glGAMap");
	gamap->setMapping("Hips", "base");
#endif

	//Create tweak bars
	TwWindowSize(windowWidth, windowHeight);
	//TwBar* animBar = TwNewBar("Animation Editor");
	//TwDefine("  ");
	//TwBar* skinningBar = TwNewBar("Skinning Editor");
	TwBar* renderingBar = TwNewBar("Rendering Editor");
	TwBar* sceneBar = TwNewBar("Scene Editor");

	// @@@GPTODO
	// create a TwDefineStruct to define a vec4 as structure of doubles
	// and a TwDefineStruct to define a mat4 as structure of vec4s

	//MainBar = TwNewBar("MainBar");
	TwAddVarRW(sceneBar, "ViewerX", TW_TYPE_FLOAT, &g_ZoomX,
		" min=-50.01 max=100.0 step=0.01 KeyIncr=x KeyDecr=X help='Scale the object by moving the viewer at X' ");
	TwAddVarRW(sceneBar, "ViewerY", TW_TYPE_FLOAT, &g_ZoomY,
		" min=-150.01 max=150.0 step=0.01 KeyIncr=y KeyDecr=Y help='Scale the object by moving the viewer at Y' ");
	TwAddVarRW(sceneBar, "ViewerZ", TW_TYPE_FLOAT, &g_Zoom,
		" min=-150.01 max=450.0 step=0.1 KeyIncr=z KeyDecr=Z help='Scale the object by moving the viewer at Z' ");
	TwAddVarRW(sceneBar, "speed", TW_TYPE_DOUBLE, &speed,
		" label='Rot speed' min=0 max=5 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");
	// Add 'wire' to 'bar': it is a modifable variable of type TW_TYPE_BOOL32 (32 bits boolean). Its key shortcut is [w].
	TwAddVarRW(renderingBar, "wire", TW_TYPE_BOOL32, &wire,
		" label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");
	TwAddVarRW(sceneBar, "cameraView", TW_TYPE_BOOL32, &cameraView,
		" label='cameraView mode' key=c help='Toggle camera view mode via mouse&keyboard arrows.' ");
	// Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
	//TwAddVarRW(MainBar, "bgColor", TW_TYPE_COLOR4F, &bgColor, " label='Background color' ");
	TwAddVarRW(sceneBar, "bgColor", TW_TYPE_COLOR4F, glm::value_ptr(bgColor), " label='Background color' ");
	//TwDefine("MainBar alwaystop=true "); // mybar is always on top

	// add the 'Axis' enumeration variable to 'MainBar'
	// (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
	{
		// axisEV associates RotationAxis enum values with labels that will be displayed instead of enum values
		TwEnumVal axisEV[NumAxes] = { { Xaxis, "Xaxis" }, { Yaxis, "Yaxis" }, { Zaxis, "Zaxis" } };
		// Create a type for the enum axisEV
		TwType axisType = TwDefineEnum("Rotation Axis", axisEV, NumAxes);
		// add 'Axis' to 'MainBar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
		TwAddVarRW(sceneBar, "Axis", axisType, &Axis, "keyIncr='<' keyDecr='>' help='Change rotation axis.' ");
	}

	// add a simple rotation widget
	TwAddVarRW(sceneBar, "Rotation", TW_TYPE_QUAT4F, &g_quat, "");

	// add a button to reset the view
	TwAddButton(sceneBar, "ResetView", ResetViewCB, NULL, " label='Reset View' ");


	//here we add widget to the modelviewMatrix of the scene


	//add a simple light direction widget
	TwAddVarRW(renderingBar, "LightDir", TW_TYPE_DIR3D, &g_dir, "");

	// Set GLFW event callbacks
	// - Redirect window size changes to the callback function WindowSizeCB
	glfwSetWindowSizeCallback(WindowSizeCB);

	// - Directly redirect GLFW mouse button events to AntTweakBar
	//glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
	glfwSetMouseButtonCallback(OnMouseButton);

	// - Directly redirect GLFW mouse position events to AntTweakBar
	//glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);
	glfwSetMousePosCallback(OnMousePos);

	// - Directly redirect GLFW mouse wheel events to AntTweakBar
	//glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
	glfwSetMouseWheelCallback(OnMouseWheel);

	// - Directly redirect GLFW key events to AntTweakBar
	//glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
	glfwSetKeyCallback(OnKey);

	// - Directly redirect GLFW char events to AntTweakBar
	//glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);
	glfwSetCharCallback(OnChar);


	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLExitIfError();

	/*------The below code is for priting values of the RigMesh classed used for the particular COLLADA file----------*/

	uint nBones = pRigMesh->numBones();
	std::cout << "pRigMesh->numBones() " << nBones << std::endl;
	std::cout << "pRigMesh->Positions.size() " << pRigMesh->Positions.size() << std::endl;
	std::cout << "pRigMesh->Normals.size() " << pRigMesh->Normals.size() << std::endl;
	std::cout << "pRigMesh->TexCoords.size() " << pRigMesh->TexCoords.size() << std::endl;

	std::cout << "pRigMesh->Bones.size() " << pRigMesh->Bones.size() << std::endl;

	std::cout << "We have 4* " << pRigMesh->Bones.size() << " BoneIDs and 4* " << pRigMesh->Bones.size() << " Weights for it's one of the " << pRigMesh->Bones.size() << " VertexBoneData" << std::endl;

	std::cout << "pRigMesh->Indices.size() " << pRigMesh->Indices.size() << std::endl;
	std::cout << "pRigMesh->m_Entries.size() " << pRigMesh->m_Entries.size() << " NumIndices is " << pRigMesh->m_Entries[0].numIndices << " baseVertex is " << pRigMesh->m_Entries[0].baseVertex <<
		" baseIndex is " << pRigMesh->m_Entries[0].baseVertex << " materialIndex " << pRigMesh->m_Entries[0].materialIndex << std::endl;
	std::cout << "pRigMesh->m_Textures.size() " << pRigMesh->m_Textures.size() << std::endl;


	std::cout << "pRigMesh->m_BoneMapping.size() " << pRigMesh->m_BoneMapping.size() << std::endl;
	std::cout << "pRigMesh->m_NumBones " << pRigMesh->m_NumBones << std::endl;

	std::cout << "pRigMesh->m_BoneInfo.size() " << pRigMesh->m_BoneInfo.size() << std::endl;


	std::cout << "pRigMesh->m_useGAforInterpolation " << pRigMesh->m_useGAforInterpolation << std::endl;
	std::cout << "pRigMesh->m_GAfactorDenominator " << pRigMesh->m_GAfactorDenominator << std::endl;

	std::cout << "pRigMesh->m_pScene->mNumAnimations " << pRigMesh->m_pScene->mNumAnimations << std::endl;

	std::cout << "pRigMesh->m_pScene->mMeshes[0]->mNumFaces " << pRigMesh->m_pScene->mMeshes[0]->mNumFaces << std::endl;

	/*------End-----of----printing-------------------------------------------------------------------*/




	// get the simulation object
	SmartBody::SBSimulationManager* sim = glGAsb->m_pScene->getSimulationManager();
	// if you want to use a real-time clock do the following:
	bool useRealTimeClock = true;
	if (useRealTimeClock)
	{
		sim->setupTimer();
	}
	else
	{
		// otherwise, the time will run according
		sim->setTime(0.0);
	}

	// make the character do something
	//scene->getBmlProcessor->execBML("ChrBrad", "<body posture=\"ChrBrad@Idle01\"/>");

	LOG("Starting the simulation...");
	double lastPrint = 0;

	/*
	*
	* GLFW main display loop
	*/

	sim->start();

	//while (running) {
		while (sim->getTime() < 100.0){
			//Not use the the update scene
			//m_pScene->update();
			if (!useRealTimeClock)
				sim->setTime(sim->getTime() + 0.16); // update at 1/60 of a second when running in simulated time
			else
				sim->updateTimer();
			if (sim->getTime() > lastPrint)
			{
				LOG("Simulation is at time: %5.2f\n", sim->getTime());
				lastPrint = sim->getTime() + 10;
			}



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		//std::cout << calcFPS() << std::endl;              // Print the FPS to the console once per second
		// std::cout << calcFPS(2.0) << std::endl;           // Print the FPS to the console every 2 seconds
		calcFPS(3.0, windowTitle);              // Update the window title to include the FPS details every 2 seconds
		//calcFPS(3.0, "Current FPS: ");          // Update the window title to the string literal "Current FPS: " + the FPS details every 3 seconds

		//calcSimpleFPS();


		//draw our OpenGL geometry
		//GLExitIfError();


#ifdef USE_MESH_DYNAMIC
	displayMeshDynamic();
#endif


		// GL states
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);
		//restore FILL mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//AntTweakBar draw
		TwDraw();

		glfwSwapBuffers();

		//check if ESC was pressed
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
		//Sleep(20000);
}
		sim->stop();

	/*
	*
	* end GLFW main display loop
	*/

	//terminate AntTweakBar
	TwTerminate();

	//cleanup VAO, VBO and shaders
	glDeleteBuffers(1, &buffer);

#if defined USE_MESH_STATIC 
	glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
#endif

#if defined USE_MESH_DYNAMIC
	glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_RigBuffers), m_RigBuffers);
#endif

	glDeleteProgram(program);
	glDeleteProgram(programMesh);
	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &vaoPlane);
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteVertexArrays(1, &mRig_VAO);



	//close OpenGL window and terminate GLFW
	glfwTerminate();


	exit(EXIT_SUCCESS);

}










