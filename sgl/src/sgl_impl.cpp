//---------------------------------------------------------------------------
// sgl.cpp
// Empty implementation of the SGL (Simple Graphics Library)
// Date:  2016/10/24
// Author: Jaroslav Sloup
// Author: Jaroslav Krivanek, Jiri Bittner, CTU Prague
// Edited: Jakub Hendrich, Daniel Meister, CTU Prague
//---------------------------------------------------------------------------

#include "sgl.h"
#include "sgl_core.hpp"

// sglCore instance
std::unique_ptr<SglCore> core = nullptr;

//---------------------------------------------------------------------------
// sglGetError()
//---------------------------------------------------------------------------

sglEErrorCode sglGetError(void)
{
    sglEErrorCode ret;
    if(core) {
        ret = core->get_error();
    } else {
	    ret = SGL_NO_ERROR;
    }
	return ret;
}

//---------------------------------------------------------------------------
// sglGetErrorString()
//---------------------------------------------------------------------------
const char *sglGetErrorString(sglEErrorCode error)
{
	static const char *errStrigTable[] =
		{
			"Operation succeeded",
			"Invalid argument(s) to a call",
			"Invalid enumeration argument(s) to a call",
			"Invalid call",
			"Quota of internal resources exceeded",
			"Internal library error",
			"Matrix stack overflow",
			"Matrix stack underflow",
			"Insufficient memory to finish the requested operation"};

	if ((int)error < (int)SGL_NO_ERROR || (int)error > (int)SGL_OUT_OF_MEMORY)
		return "Invalid value passed to sglGetErrorString()";

	return errStrigTable[(int)error];
}

//---------------------------------------------------------------------------
// Initialization functions
//---------------------------------------------------------------------------


void sglInit(void) 
{
	core = std::make_unique<SglCore>();
}

void sglFinish(void)
{
	core.reset();
}

int sglCreateContext(int width, int height) 
{ 
	return core->create_context(width, height);
}

void sglDestroyContext(int id) 
{
    core->destroy_context(id);
}

void sglSetContext(int id) 
{
    core->set_context(id);
}

int sglGetContext(void) 
{ 
	return core->get_context();
}

float *sglGetColorBufferPointer(void)
{
	// NOTE(msakmary) This is just a test if everything works
	// please change this to the current implementation whenever you wish
	float* array = new float[800*600 * 3];
	for(int i = 0; i < 800*600 * 3; i+=3)
	{
		array[i] = 0.5;
		array[i + 1] = 0.5;
		array[i + 2] = 0;
	}
	return array;
}

//---------------------------------------------------------------------------
// Drawing functions
//---------------------------------------------------------------------------

void sglClearColor(float r, float g, float b, float alpha) {}

void sglClear(unsigned what) {}

void sglBegin(sglEElementType mode) {}

void sglEnd(void) {}

void sglVertex4f(float x, float y, float z, float w) {}

void sglVertex3f(float x, float y, float z) {}

void sglVertex2f(float x, float y) {}

void sglCircle(float x, float y, float z, float radius) {}

void sglEllipse(float x, float y, float z, float a, float b) {}

void sglArc(float x, float y, float z, float radius, float from, float to) {}

//---------------------------------------------------------------------------
// Transform functions
//---------------------------------------------------------------------------

void sglMatrixMode(sglEMatrixMode mode) {}

void sglPushMatrix(void) {}

void sglPopMatrix(void) {}

void sglLoadIdentity(void) {}

void sglLoadMatrix(const float *matrix) {}

void sglMultMatrix(const float *matrix) {}

void sglTranslate(float x, float y, float z) {}

void sglScale(float scalex, float scaley, float scalez) {}

void sglRotate2D(float angle, float centerx, float centery) {}

void sglRotateY(float angle) {}

void sglOrtho(float left, float right, float bottom, float top, float near, float far) {}

void sglFrustum(float left, float right, float bottom, float top, float near, float far) {}

void sglViewport(int x, int y, int width, int height) {}

//---------------------------------------------------------------------------
// Attribute functions
//---------------------------------------------------------------------------

void sglColor3f(float r, float g, float b) {}

void sglAreaMode(sglEAreaMode mode) {}

void sglPointSize(float size) {}

void sglEnable(sglEEnableFlags cap) {}

void sglDisable(sglEEnableFlags cap) {}

//---------------------------------------------------------------------------
// RayTracing oriented functions
//---------------------------------------------------------------------------

void sglBeginScene() {}

void sglEndScene() {}

void sglSphere(const float x,
			   const float y,
			   const float z,
			   const float radius)
{
}

void sglMaterial(const float r,
				 const float g,
				 const float b,
				 const float kd,
				 const float ks,
				 const float shine,
				 const float T,
				 const float ior)
{
}

void sglPointLight(const float x,
				   const float y,
				   const float z,
				   const float r,
				   const float g,
				   const float b)
{
}

void sglRayTraceScene() {}

void sglRasterizeScene() {}

void sglEnvironmentMap(const int width,
					   const int height,
					   float *texels)
{
}

void sglEmissiveMaterial(const float r,
						 const float g,
						 const float b,
						 const float c0,
						 const float c1,
						 const float c2)
{
}
