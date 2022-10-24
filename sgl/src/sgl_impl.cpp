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
#include "sgl_vertex.hpp"

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
	return core->contexts.at(core->get_context()).framebuffer.get_framebuffer_pointer();
}

//---------------------------------------------------------------------------
// Drawing functions
//---------------------------------------------------------------------------

bool check_recording_status(std::string prefix)
{
	// check for calls inside begin/end block or when no context was allocated
	if(core->get_recording() || core->contexts.size() == 0 || core->get_context() == -1) 
	{
		core->set_error(sglEErrorCode::SGL_INVALID_OPERATION);
		SGL_DEBUG_OUT(prefix + " Cannot be called inside sglBegin()/sglEnd() block or if no context was created");
		return false;
	}
	return true;
}

void sglClearColor(float r, float g, float b, float alpha) 
{
	if(!check_recording_status("[sglClearColor()]")) { return; }
	core->contexts.at(core->get_context()).clear_color = Pixel{.r = r, .g = g, .b = b};
}

void sglClear(unsigned what)
{
	if((what & ~(sglEClearBit::SGL_COLOR_BUFFER_BIT | sglEClearBit::SGL_DEPTH_BUFFER_BIT)) != 0x0000)
	{
		SGL_DEBUG_OUT("[sglClear()] Invalid clear bit");
		core->set_error(sglEErrorCode::SGL_INVALID_ENUM);
		return;
	}
	if(!check_recording_status("[sglClearColor()]")) { return; }
	core->contexts.at(core->get_context()).clear(what);
}

void sglBegin(sglEElementType mode) 
{
	if(core->get_recording())
	{
		SGL_DEBUG_OUT("[sglBegin()] Another call to sglBegin() previously without corresponding sglEnd()");
		core->set_error(sglEErrorCode::SGL_INVALID_OPERATION);
		return;
	}
	if(mode >= SGL_LAST_ELEMENT_TYPE || mode < SGL_POINTS)
	{
		core->set_error(sglEErrorCode::SGL_INVALID_ENUM);
		return;
	}

	core->renderer.state.element_type_mode = mode;
	core->set_recording(true);
}

void sglEnd(void)
{
	if(!core->get_recording())
	{
		SGL_DEBUG_OUT("[sglEnd()] No previous unended sglBegin call registered");
		core->set_error(sglEErrorCode::SGL_INVALID_OPERATION);
		return;
	}
	core->set_recording(false);
}

void sglVertex4f(float x, float y, float z, float w) 
{
	if(!core->get_recording()) { SGL_DEBUG_OUT("[sglVertex4f()] Ignoring vertex - no active sglBegin() call"); return; }
	core->push_vertex(SglVertex(x, y, z, w));
}

void sglVertex3f(float x, float y, float z)
{
	if(!core->get_recording()) { SGL_DEBUG_OUT("[sglVertex3f()] Ignoring vertex - no active sglBegin() call"); return; }
	core->push_vertex(SglVertex(x, y, z, 1.0f));
}

void sglVertex2f(float x, float y)
{
	if(!core->get_recording()) { SGL_DEBUG_OUT("[sglVertex2f()] Ignoring vertex - no active sglBegin() call"); return; }
	core->push_vertex(SglVertex(x, y, 0.0f, 1.0f));
}

void sglCircle(float x, float y, float z, float radius)
{
	if(!core->get_recording()) { SGL_DEBUG_OUT("[sglCircle()] Ignoring vertex - no active sglBegin() call"); return; }
	core->draw_circle(SglVertex(x, y, z, 1.0f), radius);
}

void sglEllipse(float x, float y, float z, float a, float b)
{
	if(!core->get_recording()) { SGL_DEBUG_OUT("[sglEllipse()] Ignoring vertex - no active sglBegin() call"); return; }
	core->draw_ellipse(SglVertex(x, y, z, 1.0f), a, b);
}

void sglArc(float x, float y, float z, float radius, float from, float to)
{
	if(!core->get_recording()) { SGL_DEBUG_OUT("[sglArc()] Ignoring vertex - no active sglBegin() call"); return; }
	core->draw_arc(SglVertex(x, y, z, 1.0f), radius, from, to);
}

//---------------------------------------------------------------------------
// Transform functions
//---------------------------------------------------------------------------

void sglMatrixMode(sglEMatrixMode mode) 
{
	if(!check_recording_status("[sglMatrixMode()]")) { return; }
	// check if mode is known sglEMatrixMode value
	if(mode != sglEMatrixMode::SGL_MODELVIEW &&
	   mode != sglEMatrixMode::SGL_PROJECTION)
	{
		SGL_DEBUG_OUT("[sglMatrixMode()] invalid mode enum provided");
		core->set_error(sglEErrorCode::SGL_INVALID_ENUM);
		return;
	}

	core->contexts.at(core->get_context()).set_matrix_mode(mode);
}

void sglPushMatrix(void) 
{
	if(!check_recording_status("[sglPushMatrix()]")) { return; }
	core->contexts.at(core->get_context()).push_matrix();
}

void sglPopMatrix(void) 
{
	if(!check_recording_status("[sglPopMatrix()]")) { return; }
	core->contexts.at(core->get_context()).pop_matrix();
}

void sglLoadIdentity(void) 
{
	if(!check_recording_status("[sglLoadIdentityMatrix()]")) { return; }
	core->contexts.at(core->get_context()).load_identity();
}

void sglLoadMatrix(const float *matrix)
{
	if(!check_recording_status("[sglLoadMatrix()]")) { return; }

	std::array<float, 16> transp_mat;
	for(int col = 0; col < 4; col++)
	{
		for(int row = 0; row < 4; row++)
		{
			transp_mat[row * 4 + col] = matrix[col * 4 + row];
		}
	}
	core->contexts.at(core->get_context()).load_matrix({transp_mat});
}

void sglMultMatrix(const float *matrix)
{
	if(!check_recording_status("[sglMultMatrix()]")) { return; }

	std::array<float, 16> transp_mat;
	for(int col = 0; col < 4; col++)
	{
		for(int row = 0; row < 4; row++)
		{
			transp_mat[row * 4 + col] = matrix[col * 4 + row];
		}
	}
	core->contexts.at(core->get_context()).mult_matrix({transp_mat});
}

void sglTranslate(float x, float y, float z) 
{
	if(!check_recording_status("[sglTranslateMatrix()]")) { return; }
	core->contexts.at(core->get_context()).translate(x, y, z);
}

void sglScale(float scalex, float scaley, float scalez) 
{
	if(!check_recording_status("[sglScale()]")) { return; }
	core->contexts.at(core->get_context()).scale(scalex, scaley, scalez);
}

void sglRotate2D(float angle, float centerx, float centery) 
{
	if(!check_recording_status("[sglRotate2D()]")) { return; }
	core->contexts.at(core->get_context()).rotate_2d(angle, centerx, centery);
}

void sglRotateY(float angle) 
{
	if(!check_recording_status("[sglRotateY()]")) { return; }
	core->contexts.at(core->get_context()).rotate_y(angle);
}

void sglOrtho(float left, float right, float bottom, float top, float near, float far) 
{
	if(!check_recording_status("[sglOrtho()]")) { return; }
	if(left == right || bottom == top || near == far)
	{
		core->set_error(sglEErrorCode::SGL_INVALID_VALUE);
		return;
	}
	core->contexts.at(core->get_context()).ortho(left, right, bottom, top, near, far);
}

void sglFrustum(float left, float right, float bottom, float top, float near, float far) {}

void sglViewport(int x, int y, int width, int height)
{
	if(!check_recording_status("[sglViewport()]")) { return; }
	if(width < 0.0f || height < 0.0f)
	{
		core->set_error(sglEErrorCode::SGL_INVALID_VALUE);
		return;
	}
	core->contexts.at(core->get_context()).viewport(x, y, width, height);
}

//---------------------------------------------------------------------------
// Attribute functions
//---------------------------------------------------------------------------

void sglColor3f(float r, float g, float b) 
{
	if(!check_recording_status("[sglColor3f()]")) { return; }
	core->renderer.state.draw_color = Pixel{.r = r, .g = g, .b = b};
}

void sglAreaMode(sglEAreaMode mode) 
{
	if(!check_recording_status("[sglAreaMode()]")) { return; }
	if(mode < sglEAreaMode::SGL_POINT || mode > sglEAreaMode::SGL_FILL)
	{
		SGL_DEBUG_OUT("[sglAreaMode()] Invalid area mode");
		core->set_error(sglEErrorCode::SGL_INVALID_ENUM);
		return;
	}
	core->renderer.state.area_mode = mode;
}

void sglPointSize(float size)
{
	if(!check_recording_status("[sglPointSize()]")) { return; }
	if(size < 0.0f)
	{
		SGL_DEBUG_OUT("[sglPointSize()] Point size is not allowed to be negative value");
		core->set_error(sglEErrorCode::SGL_INVALID_VALUE);
		return;
	}
	core->renderer.state.point_size = size;
}

void sglEnable(sglEEnableFlags cap) { cap=SGL_DEPTH_TEST; }

void sglDisable(sglEEnableFlags cap) { cap=SGL_NO_DEPTH_TEST; }

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
