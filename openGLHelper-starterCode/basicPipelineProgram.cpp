#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "basicPipelineProgram.h"
using namespace std;

int BasicPipelineProgram::Init(const char * shaderBasePath) 
{
  if (BuildShadersFromFiles(shaderBasePath, "basic.vertexShader.glsl", "basic.fragmentShader.glsl") != 0)
  {
    cout << "Failed to build the pipeline program." << endl;
    return 1;
  }

  cout << "Successfully built the pipeline program." << endl;
  return 0;
}

void BasicPipelineProgram::SetModelViewMatrix(const float * m) 
{
  // pass "m" to the pipeline program, as the modelview matrix
  // students need to implement this
	glUniformMatrix4fv(h_modelViewMatrix, 1, false, m);
}

void BasicPipelineProgram::SetProjectionMatrix(const float * m) 
{
  // pass "m" to the pipeline program, as the projection matrix
  // students need to implement this
	glUniformMatrix4fv(h_projectionMatrix, 1, false, m);
}

int BasicPipelineProgram::SetShaderVariableHandles() 
{
  // set h_modelViewMatrix and h_projectionMatrix
  // students need to implement this

	h_modelViewMatrix = glGetUniformLocation(GetProgramHandle(), "modelViewMatrix");
	if (h_modelViewMatrix == -1)
	{
		cout << "Failed to find modelViewMatrix" << endl;
		return 1;
	}

	h_projectionMatrix = glGetUniformLocation(GetProgramHandle(), "projectionMatrix");
	if (h_projectionMatrix == -1)
	{
		cout << "Failed to find projectionMatrix" << endl;
		return 1;
	}

	{///bind texture0 if available
		int h_tex0 = glGetUniformLocation(GetProgramHandle(), "tex0");
		if (h_tex0 != -1)glUniform1i(h_tex0, 0); //Texture unit 0 is for base images
	}
  return 0;
}

