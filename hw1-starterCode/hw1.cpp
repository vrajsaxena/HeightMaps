/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code

  Student username: vivekras@usc.edu
  Student Name: Vivek Raj Saxena
  Student ID: 6834311974
*/

#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#ifdef WIN32
char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif



#ifdef WIN32
char texshaderBasePath[1024] = SHADER_BASE_PATH"/tex";
#else
char texshaderBasePath[1024] = "../openGLHelper-starterCode/tex";
#endif


using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, -256.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";


///number of indices in index buffer
int indices = 0;

///number of vertices in vertex buffer
int vertices = 0;

///opengl texture
GLuint texture = 0;

///opengl vertex array object id
GLuint vboVertices = 0;

///opengl vertex buffer
GLuint vboIndices = 0;

///opengl index buffer, used for triangle and wireframe
GLuint vao = 0;

//shader gradient
BasicPipelineProgram shader;

//shader textured
BasicPipelineProgram texshader;

///matrix maniplation
OpenGLMatrix matrix;

///control the drawing mode - fill, wirefram or points

enum drwmode { Fill=0, Wireframe, Points};
int drawmode = Fill;


///control textured or gradient colors
bool drawtex=false;

///request to make screnshots each frame
int makescreenshots = 0;
/// sequential number of auto screenshot
int screenshotindex = 0;

// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void displayFunc()
{

	{///fix the modelbview matrix according to teh current state of rotation translation and scale
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::ModelView);
		matrix.LoadIdentity();

		matrix.Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
		matrix.Scale(landScale[0], landScale[1], landScale[2]);
		matrix.Rotate(landRotate[2], 0, 0, 1);
		matrix.Rotate(landRotate[1], 0, 1, 0);
		matrix.Rotate(landRotate[0], 1, 0, 0);
	}
	{///set modelview matrix to GPU
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::ModelView);
		matrix.GetMatrix(m);
		shader.SetModelViewMatrix(m);
	}
	{///set projection matrix to GPU
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::Projection);
		matrix.GetMatrix(m);
		shader.SetProjectionMatrix(m);
	}
	
	// render some stuff...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///draw using Draw elements and vertex index buffer
	if (drawmode == Fill)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, (void*)0);
	}
	if (drawmode == Wireframe)
	{
		glPolygonMode( GL_FRONT_AND_BACK,GL_LINE);
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, (void*)0);
	}

	if (drawmode == Points)
	{// no need to use indies when eachvertex is ony ysed ONCE per point
		glDrawArrays(GL_POINTS, 0, vertices);
	}

	glutSwapBuffers();
}

void idleFunc()
{
}




///make the animation in the timer func
void timerFunc(int value)
{
	// do some stuff... 

	// for example, here, you can save the screenshots to disk (to make the animation)
	if (makescreenshots)
	{
		makescreenshots--;
		screenshotindex++;
		// write a screenshot to the specified filename

		char filename[256];
		sprintf(filename, "screnshot_%04d.jpg", screenshotindex);
		saveScreenshot(filename);
	}
	// make the screen update 
	glutPostRedisplay();
	glutTimerFunc(33, timerFunc,0);//target 30fps
}
void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);

  // setup perspective matrix...

  {////fix proection matrix
	  float m[16];
	  matrix.SetMatrixMode(OpenGLMatrix::Projection);
	  matrix.LoadIdentity();
	  matrix.Perspective(45, float(w)/h, 0.5, 1000);
  }
}

void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        landTranslate[2] += mousePosDelta[1] * 0.01f;
      }
      break;

    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] += mousePosDelta[1];
        landRotate[1] += mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] += mousePosDelta[1];
      }
      break;

    // scale the landscape
    case SCALE:
      if (leftMouseButton)
      {
        // control x,y scaling via the left mouse button
        landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
		///change the drawing mode
	  drawmode = (drawmode+1)%3;
    break;

	case 's':
		drawtex = !drawtex;
		if (drawtex)texshader.Bind();
		else shader.Bind();
		break;

	case 'x':
		// take a screenshot
		saveScreenshot("screenshot.jpg");
		break;
	
	case 'z':
		// take screenshots
		makescreenshots = 10;
		screenshotindex = 0;
		break;
	
  }
}

void initScene(int argc, char *argv[])
{
	if(argc>=3)
	{
		ImageIO *image = new ImageIO();
		if (image->loadJPEG(argv[2]) != ImageIO::OK)
		{
			cout << "Error reading image " << argv[2] << "." << endl;
			exit(EXIT_FAILURE);
		}

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		// Generate a name for the texture
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		if (image->getWidth() && image->getHeight())
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->getWidth(), image->getHeight(), 0,
			GL_RGB, GL_UNSIGNED_BYTE, image->getPixels());


		delete image;
	}

  // load the image from a jpeg disk file to main memory
	ImageIO * heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // do additional initialization here...

	//enagle the depth test
  glEnable(GL_DEPTH_TEST);

  {//init vertex buffer and index buffer

	  glGenVertexArrays(1, &vao);
	  glBindVertexArray(vao);


	  glGenBuffers(1, &vboVertices);
	  glBindBuffer(GL_ARRAY_BUFFER, vboVertices);

	  ///one vertex for each pixel of image
	  vertices = heightmapImage->getHeight()*heightmapImage->getWidth();

	  GLfloat * vdata = new GLfloat[3*vertices];

	  for (int j = 0; j < heightmapImage->getHeight(); j++)
		  for (int i = 0; i < heightmapImage->getWidth(); i++)
		  {/// make center be at 0,0,
			  int offset = (j*heightmapImage->getWidth() + i);
			  vdata[3 * offset + 0] = (i - (int)heightmapImage->getWidth() / 2) ;
			  vdata[3 * offset + 1] = (j - (int)heightmapImage->getHeight() / 2) ;
			  vdata[3 * offset + 2] = heightmapImage->getPixel(i, j, 0)/8;
		  }

	  glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(GLfloat)*3, vdata, GL_STATIC_DRAW);

	  free(vdata);

	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(0);



	  glGenBuffers(1, &vboIndices);

	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);


	  /// 6 indices for two triangles for each quad of the surface
	  indices = (heightmapImage->getHeight() - 1)*(heightmapImage->getWidth() - 1) * 6;

	  GLuint * idata = new GLuint[indices];

	  for (int j = 0; j < heightmapImage->getHeight() - 1; j++)
		  for (int i = 0; i < heightmapImage->getWidth() - 1; i++)
		  {
			  int offset = (j*heightmapImage->getWidth() + i);
			  int offsettri = (j*(heightmapImage->getWidth() - 1) + i);
			  idata[6 * offsettri + 0] = offset;
			  idata[6 * offsettri + 1] = offset + heightmapImage->getWidth();
			  idata[6 * offsettri + 2] = offset + 1;

			  idata[6 * offsettri + 3] = offset + 1;
			  idata[6 * offsettri + 4] = offset + heightmapImage->getWidth() + 1;
			  idata[6 * offsettri + 5] = offset + heightmapImage->getWidth();
		  }

	  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices * sizeof(GLuint), idata, GL_STATIC_DRAW);

	  free(idata);
  }

  {//initialise and use the shader
	  shader.Init(shaderBasePath);
  }

  {//initialise and use the shader
	  texshader.Init(texshaderBasePath);
	  shader.Bind();
  }

  delete heightmapImage;
}

int main(int argc, char *argv[])
{
  if ((argc != 2)&&(argc!=3))
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file> <optional texture>" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // tells glut to use a particular display function to redraw 
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);
  // callback to request frames on timer
  glutTimerFunc(33, timerFunc,0);//target 30fps


  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif

  // do initialization
  initScene(argc, argv);

  // sink forever into the glut loop
  glutMainLoop();
}


