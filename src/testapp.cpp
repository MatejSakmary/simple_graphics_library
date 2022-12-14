#include <array>
//---------------------------------------------------------------------------
// testapp.cpp
// Testing file for the Simple Graphics Library (SGL)
// Date:  2016/10/24
// Author: Jaroslav Sloup
// Author: Jaroslav Krivanek, Jiri Bittner, CTU Prague
// Edited: Jakub Hendrich, Daniel Meister, CTU Prague
//---------------------------------------------------------------------------

/// uncomment the tests you wish to run

// #define TEST0
// #define TEST1
// #define TEST2
//#define TEST3
//#define TEST4
#define TEST5

#ifdef TEST0
#define WIDTH 800
#define HEIGHT 600
#define TEST_0A
#define TEST_0B
#endif

#ifdef TEST1
#define WIDTH 800
#define HEIGHT 600
#define TEST_1A
#define TEST_1B
#define TEST_1C
#endif

#ifdef TEST2
#define WIDTH 800
#define HEIGHT 600
#define TEST_2A
#define TEST_2B
#define TEST_2C
#define TEST_2D
#endif

#ifdef TEST3
#define WIDTH 512
#define HEIGHT 512
#define TEST_3A
#define TEST_3B
#define TEST_3C
#endif

#ifdef TEST4
#define WIDTH 512
#define HEIGHT 512
#define TEST_4A
#define TEST_4B
#define TEST_4C
#endif

#ifdef TEST5
#define WIDTH 512
#define HEIGHT 512
#define TEST_5A
#define TEST_5B
#define TEST_5C
#define TEST_5D
#endif

#if !defined(USE_GUI) && defined(_MSC_VER)
#define USE_GUI 1
#endif

#ifdef TEST_0A
#define USE_VIEWPORT 0
#endif

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

#if USE_GUI
#include <GLFW/glfw3.h>
#endif

#include "util/nffread.h"
#include "util/nffstore.h"
#include "util/timer.h"
#include "../sgl/src/macros.hpp"
#include "../sgl/include/sgl.h"

#define PRINTVAR(a) cerr << #a "\t" << a << endl;

/// Main windows parameters
#define TITLE "SGL Test Application"
#define NUM_CONTEXTS 8

static int _contexts[10];

float tx = 0, ty = 0, tz = 0, tstep = 0.2;
float rot = M_PI / 3, rotstep = 0.1;
bool show_depth = false;

unsigned int runMultiplier;

/// helper class for sgluLookAt
class Vector3
{
public:
    float x, y, z;
    Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
    inline friend float SqrMagnitude(const Vector3 &v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
    Vector3 &operator/=(float A)
    {
        float a = 1.0f / A;
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }
    inline friend Vector3 CrossProd(const Vector3 &A, const Vector3 &B)
    {
        return Vector3(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x);
    }
};

typedef unsigned char uint8;
typedef unsigned int uint;

/// Stores framebuffer as uncompressed .tga image
void WriteTGA(const char *aFilename)
{
    float *data = (float *)sglGetColorBufferPointer();
    if (data == NULL)
        return;

    uint resX = WIDTH;
    uint resY = HEIGHT;
    char header[18];
    header[0] = 0;
    header[1] = 0;
    header[2] = 2;
    header[3] = 0;
    header[4] = 0;
    header[5] = 0;
    header[6] = 0;
    header[7] = 0;
    header[8] = 0;
    header[9] = 0;
    header[10] = 0;
    header[11] = 0;
    header[12] = static_cast<uint8>(resX % 0x100);
    header[13] = static_cast<uint8>(resX / 0x100);
    header[14] = static_cast<uint8>(resY % 0x100);
    header[15] = static_cast<uint8>(resY / 0x100);
    header[16] = 24;
    header[17] = 0; // set to 32 to flip the image

    std::ofstream outFile;
    outFile.open(aFilename, std::ios::binary);
    if (!outFile)
    {
        // throw std::exception("Could not open required file");
        cerr << "Could not open required file " << aFilename << endl;
    }

    outFile.write(reinterpret_cast<char *>(&header), 18);

    for (uint i = 0; i < (3 * resX * resY);)
    {
        uint8 r, g, b;
        r = static_cast<uint8>(std::max(0.f, std::min(1.f, data[i++])) * 255.f);
        g = static_cast<uint8>(std::max(0.f, std::min(1.f, data[i++])) * 255.f);
        b = static_cast<uint8>(std::max(0.f, std::min(1.f, data[i++])) * 255.f);
        outFile.write(reinterpret_cast<char *>(&b), sizeof(uint8));
        outFile.write(reinterpret_cast<char *>(&g), sizeof(uint8));
        outFile.write(reinterpret_cast<char *>(&r), sizeof(uint8));
    }

    outFile.flush();
    outFile.close();
}

/// like gluLookAt
void sgluLookAt(float eyex, float eyey, float eyez,
                float centerx, float centery, float centerz,
                float upx, float upy, float upz)
{
    float sqrmag;

    /* Make rotation matrix */

    /* Z vector */
    Vector3 z(eyex - centerx, eyey - centery, eyez - centerz);
    sqrmag = SqrMagnitude(z);
    if (sqrmag)
        z /= sqrtf(sqrmag);

    /* Y vector */
    Vector3 y(upx, upy, upz);

    /* X vector = Y cross Z */
    Vector3 x(CrossProd(y, z));

    sqrmag = SqrMagnitude(x);
    if (sqrmag)
        x /= sqrtf(sqrmag);

    /* Recompute Y = Z cross X */
    y = CrossProd(z, x);

    sqrmag = SqrMagnitude(y);
    if (sqrmag)
        y /= sqrtf(sqrmag);

    float m[] = {
        x.x, y.x, z.x, 0,					   // col 1
        x.y, y.y, z.y, 0,					   // col 2
        x.z, y.z, z.z, 0,					   // col 3
        -eyex * x.x - eyey * x.y - eyez * x.z, // col 4
        -eyex * y.x - eyey * y.y - eyez * y.z, // col 4
        -eyex * z.x - eyey * z.y - eyez * z.z, // col 4
        1.0};								   // col 4

    sglMultMatrix(m);
}

/// like gluPerspective
void sgluPerspective(float fovy, float aspect, float zNear, float zFar)
{
    fovy *= (3.1415926535 / 180);
    float h2 = tan(fovy / 2) * zNear;
    float w2 = h2 * aspect;
    sglFrustum(-w2, w2, -h2, h2, zNear, zFar);
}

// ==========================================================================
//  Test cases
// ==========================================================================

/// render a wireframe 2D box
void box()
{
    sglBegin(SGL_LINE_LOOP);
    sglVertex2f(-1, -1);
    sglVertex2f(-1, 1);
    sglVertex2f(1, 1);
    sglVertex2f(1, -1);
    sglEnd();
    sglBegin(SGL_LINES);
    sglVertex2f(0, -1);
    sglVertex2f(0, 1);
    sglEnd();
}

/// render a 4-sided polygon
void poly4(int idx[4], float p[][3])
{
    sglBegin(SGL_POLYGON);
    for (int i = 0; i < 4; i++)
        sglVertex3f(p[idx[i]][0], p[idx[i]][1], p[idx[i]][2]);
    sglEnd();
}

/// render a cube [-1,1]^3
void cube()
{
    float vertices[][3] = {
        {0, 0, 0},
        {1, 1, 1},
        {1, 1, -1},
        {1, -1, 1},
        {1, -1, -1},
        {-1, 1, 1},
        {-1, 1, -1},
        {-1, -1, 1},
        {-1, -1, -1}};
    int indices[][4] = {
        {2, 1, 3, 4},
        {5, 6, 8, 7},
        {1, 2, 6, 5},
        {4, 3, 7, 8},
        {3, 1, 5, 7},
        {2, 4, 8, 6}};

    sglColor3f(0, 0, 1);
    poly4(indices[0], vertices);
    sglColor3f(0, 1, 0);
    poly4(indices[1], vertices);
    sglColor3f(0, 1, 1);
    poly4(indices[2], vertices);
    sglColor3f(1, 0, 0);
    poly4(indices[3], vertices);
    sglColor3f(1, 0, 1);
    poly4(indices[4], vertices);
    sglColor3f(1, 1, 0);
    poly4(indices[5], vertices);
}

/// render cube at (x,y,z) rotate by 'rot' radians
void placeCube(float x, float y, float z)
{
    sglPushMatrix();
    sglTranslate(x, y, z);
    sglRotateY(rot);
    cube();
    sglPopMatrix();
}

/// 3D cube drawing test
void DrawTestScene2C(void)
{
    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // set the projection matrix
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sgluPerspective(45, (float)WIDTH / HEIGHT, 0.1, 20.0);

    // set the modelview matrix
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    sglTranslate(tx, ty, tz - 10);

    sglAreaMode(SGL_FILL);

    // upper row with depth test
    sglEnable(SGL_DEPTH_TEST);
    placeCube(-1, 2, 0);
    placeCube(1, 2, 0);
    placeCube(0, 1.5, 0);

    // lower row without depth test
    sglDisable(SGL_DEPTH_TEST);
    placeCube(-1, -2, 0);
    placeCube(1, -2, 0);
    placeCube(0, -1.5, 0);
}

/// render a non-convex polygon
void butterfly()
{
    sglBegin(SGL_POLYGON);
    sglVertex2f(-2, -2);
    sglVertex2f(0, 1);
    sglVertex2f(2, -2);
    sglVertex2f(2, 2);
    sglVertex2f(0, -1);
    sglVertex2f(-2, 2);
    sglEnd();
}

/// render a bunch of colorful ellipses
void ellipses(float r, float R, float G, float B)
{
    int numInitSegments = 10;
    for (int i = 0; i < numInitSegments; i++)
    {
        float c = float(i) / numInitSegments;
        sglColor3f(c * R, c * G, c * B);
        sglEllipse(0, 0, 0, r * float(i + 1) / numInitSegments, r * float(numInitSegments - i) / numInitSegments);
    }
}

/// render a bunch of colorful arcs
void arcs(float initr, float R, float G, float B)
{
    for (float r = initr; r > 0.1; r -= 0.2)
    {
        float c = r / initr;
        sglColor3f(c * R, c * G, c * B);
        for (float seg = 0; seg < M_PI * 2; seg += M_PI * 2 / 8)
            sglArc(0, 0, 0, r, seg, seg + M_PI * 2 / 16);
    }
}

/// Element filling test
void DrawTestScene2B()
{
    sglDisable(SGL_DEPTH_TEST);

    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // set the projection matrix
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglOrtho(0, 8, -3, 3, -1, 1);

    // set the modelview matrix
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    sglTranslate(1.5, 0, 0);

    float r;
    // circles
    for (r = 1.2; r > 0.1; r -= 0.1)
    {
        sglColor3f(0, r, r);
        sglAreaMode(SGL_FILL);
        sglCircle(0, 0, 0, r);
        sglColor3f(r, r, 0);
        sglAreaMode(SGL_LINE);
        sglCircle(0, 0, 0, r);
        sglColor3f(r, r, r);
        sglAreaMode(SGL_POINT);
        sglCircle(0, 0, 0, r);
    }

    // ellipses
    sglTranslate(2.5, 0, 0);

    sglPushMatrix();
    sglTranslate(0, 1.5, 0);
    sglAreaMode(SGL_FILL);
    ellipses(1.4, 1, 1, 0);
    sglAreaMode(SGL_LINE);
    ellipses(1.4, 0, 1, 1);
    sglTranslate(0, -3, 0);
    sglRotate2D(M_PI * 0.25, 0, 0);
    sglAreaMode(SGL_FILL);
    ellipses(1.4, 1, 1, 0);
    sglAreaMode(SGL_LINE);
    ellipses(1.4, 0, 1, 1);
    sglPopMatrix();

    // arcs
    sglTranslate(2.5, 0, 0);
    sglAreaMode(SGL_FILL);
    arcs(1.3, 1, 0, 1);
    sglAreaMode(SGL_LINE);
    arcs(1.3, 0, 1, 0);
}

/// Polygon fill test
void DrawTestScene2A(void)
{
    sglDisable(SGL_DEPTH_TEST);

    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // set the projection matrix
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglOrtho(-8, 8, -6, 6, -1, 1);

    // set the modelview matrix
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    sglColor3f(1, 0, 0);
    sglAreaMode(SGL_FILL);
    butterfly();
    sglColor3f(1, 1, 0);
    sglAreaMode(SGL_LINE);
    butterfly();

    sglColor3f(0, 1, 1);
    sglAreaMode(SGL_FILL);

    sglTranslate(4, 0, 0);
    sglRotate2D(1, 0, 0);
    butterfly();
    sglColor3f(0, 1, 0);
    sglAreaMode(SGL_LINE);
    butterfly();

    sglLoadIdentity();
    sglTranslate(-4, -1, 0);
    sglRotate2D(-0.5, 0, 0);
    sglAreaMode(SGL_FILL);
    butterfly();
    sglColor3f(1, 1, 0);
    sglAreaMode(SGL_LINE);
    butterfly();
}

/// render a branch of a tree
void branch(int depth, float rot)
{
    sglPushMatrix();
    for (int i = 0; i < 3; i++)
    {
        sglScale(0.87, 0.87, 1);
        sglRotate2D(rot * 0.15, 0, 1);
        sglTranslate(0, 2, 0);
        box();
    }
    if (depth < 3)
    {
        branch(depth + 1, 1);
        sglColor3f(0, 1, 0);
        branch(depth + 1, -1);
    }
    sglPopMatrix();
}

/// Matrix stack test
void DrawTestScene1C(void)
{
    sglDisable(SGL_DEPTH_TEST);

    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // set the projection matrix
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglOrtho(-8, 8, -1, 12, -1, 1);

    // set the modelview matrix
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    // draw coord axes
    sglColor3f(0.5, 0.5, 0.5);
    sglBegin(SGL_LINES);
    sglVertex2f(-7, 0);
    sglVertex2f(7, 0);
    sglVertex2f(0, 0);
    sglVertex2f(0, 10);
    sglEnd();

    sglColor3f(1, 1, 1);

    branch(0, 1);
}

/// Transformation test
void DrawTestScene1B(void)
{
    sglDisable(SGL_DEPTH_TEST);
    sglAreaMode(SGL_LINE);

    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // set the projection matrix
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglOrtho(-8, 8, -6, 6, -1, 1);

    // set the modelview matrix
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    // draw coord axes
    sglColor3f(0.5, 0.5, 0.5);
    sglBegin(SGL_LINES);
    sglVertex2f(-7, 0);
    sglVertex2f(7, 0);
    sglVertex2f(0, -5);
    sglVertex2f(0, 5);
    sglEnd();

    // BOX
    sglColor3f(1, 1, 1);
    box();

    sglColor3f(0, 1, 0);
    sglTranslate(-3, 0, 0);
    box();

    sglColor3f(1, 0, 0);
    sglTranslate(-3, 0, 0);
    sglRotate2D(M_PI * 1 / 8, 0, 0);
    box();

    sglColor3f(0, 0, 1);
    sglLoadIdentity();
    sglTranslate(3, 0, 0);
    sglScale(0.5, 2, 1);
    box();

    sglColor3f(0, 1, 1);
    sglLoadIdentity();
    sglTranslate(6, 0, 0);
    sglRotate2D(M_PI * 1 / 8, 0, 0);
    sglScale(0.5, 2, 1);
    box();

    // ARC
    sglLoadIdentity();
    sglTranslate(0, 4, 0);

    sglColor3f(1, 1, 1);
    sglArc(0, 0, 0, 1, 0, 4);

    sglColor3f(0, 1, 0);
    sglTranslate(-3, 0, 0);
    sglArc(0, 0, 0, 1, 0, 4);

    sglColor3f(1, 0, 0);
    sglTranslate(-3, 0, 0);
    sglRotate2D(M_PI * 1 / 8, 0, 0);
    sglArc(0, 0, 0, 1, 0, 4);

    sglColor3f(0, 0, 1);
    sglLoadIdentity();
    sglTranslate(0, 4, 0);
    sglTranslate(3, 0, 0);
    sglScale(0.5, 2, 1);
    sglArc(0, 0, 0, 1, 0, 4);

    sglColor3f(0, 1, 1);
    sglLoadIdentity();
    sglTranslate(0, 4, 0);
    sglTranslate(6, 0, 0);
    sglRotate2D(M_PI * 1 / 8, 0, 0);
    sglScale(0.5, 2, 1);
    sglArc(0, 0, 0, 1, 0, 4);

    // ELLIPSE
    sglLoadIdentity();
    sglTranslate(0, -4, 0);

    sglColor3f(1, 1, 1);
    sglEllipse(0, 0, 0, 1.5, 0.7);

    sglColor3f(0, 1, 0);
    sglTranslate(-3, 0, 0);
    sglEllipse(0, 0, 0, 1.5, 0.7);

    sglColor3f(1, 0, 0);
    sglTranslate(-3, 0, 0);
    sglRotate2D(M_PI * 1 / 8, 0, 0);
    sglEllipse(0, 0, 0, 1.5, 0.7);

    sglColor3f(0, 0, 1);
    sglLoadIdentity();
    sglTranslate(0, -4, 0);
    sglTranslate(3, 0, 0);
    sglScale(0.5, 2, 1);
    sglEllipse(0, 0, 0, 1.5, 0.7);

    sglColor3f(0, 1, 1);
    sglLoadIdentity();
    sglTranslate(0, -4, 0);
    sglTranslate(6, 0, 0);
    sglRotate2D(M_PI * 1 / 8, 0, 0);
    sglScale(0.5, 2, 1);
    sglEllipse(0, 0, 0, 1.5, 0.7);
}

/// Drawing simple primitives, while assuming incomplete
/// transformation chain (no modeling, projection and window transformations)
/// Warning: will not run properly once the viewport transformation is implemented!!
void DrawTestScene0A(void)
{
    sglViewport(-1, -1, 2, 2);
    sglDisable(SGL_DEPTH_TEST);
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    float centerX = 0, centerY = 0;
    // float centerX=0, centerY=0;

    int numSegments = 32;
    float angleStep = 0.5 * M_PI / (float)numSegments;

    int numCircles = 64;
    float radiusStep = HEIGHT / numCircles;
    // float radiusStep = .5f/numCircles;
    float r = radiusStep;
    for (int j = 0; j < numCircles; j++, r += radiusStep)
    {
        // point test
        sglColor3f(j / (float)numCircles, j / (float)numCircles, j / (float)numCircles);
        sglPointSize(1);
        sglBegin(SGL_POINTS);
        {
            for (int i = 0; i < numSegments; i++)
            {
                float angle = angleStep * (float)i;
                sglVertex2f(centerX + r * cosf(angle), centerY + r * sinf(angle));
            }
        }
        sglEnd();
    }
}

void DrawTestScene0B(void)
{
    sglViewport(-1, -1, 2, 2);
    sglDisable(SGL_DEPTH_TEST);
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    float centerX = 0, centerY = 0;
    //  float centerX=0, centerY=0;
    int numSegments = 64;
    float angleStep = 0.25f * M_PI / (float)numSegments;
    float r = HEIGHT;
    // float r=0.5f;
    //  line test
    sglPointSize(1);
    for (int i = 0; i < numSegments; i++)
    {
        sglColor3f(i / (float)numSegments, i / (float)numSegments, i / (float)numSegments);
        float angle = angleStep * (float)i;
        sglBegin(SGL_LINES);
        sglVertex2f(centerX, centerY);
        sglVertex2f(centerX + r * cosf(angle), centerY + r * sinf(angle));
        sglEnd();
    }
}

/// Drawing of all elements - orthographic tranformation only (scale+translation in 2d)
void DrawTestScene1A(void)
{
    sglDisable(SGL_DEPTH_TEST);

    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // set the projection matrix
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sglOrtho(-10 * WIDTH / HEIGHT, 10 * WIDTH / HEIGHT, -10, 10, -1, 1);

    // set the modelview matrix
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    float offsetx, offsety = -6;
    int numInitSegments = 8, i;
    float segment = 2.0 * M_PI / (float)numInitSegments;

    float r;
    for (r = 0.2; r <= 2.5; r += 0.2)
    {
        offsetx = -7.25;

        // point test
        sglColor3f(r / 3, r / 3, r / 3);
        sglPointSize(3);
        sglBegin(SGL_POINTS);
        {
            for (i = 0; i < numInitSegments; i++)
            {
                float angle = segment * (float)i;
                sglVertex2f(offsetx + r * cosf(angle), offsety + r * sinf(angle));
            }
        }
        sglEnd();

        offsetx += 5;

        // line test
        sglColor3f(1, 0, 0);
        sglBegin(SGL_LINES);
        for (i = 0; i < numInitSegments; i++)
        {
            float angle = segment * (float)i;
            sglVertex2f(offsetx + r * cos(angle), offsety + r * sin(angle));
        }
        sglEnd();

        offsetx += 5;

        // line strip test
        sglColor3f(0, 1, 0);
        sglBegin(SGL_LINE_STRIP);
        for (i = 0; i < numInitSegments; i++)
        {
            float angle = segment * (float)i;
            sglVertex2f(offsetx + r * cos(angle), offsety + r * sin(angle));
        }
        sglEnd();

        offsetx += 5;

        // line loop test
        sglColor3f(0, 0, 1);
        sglBegin(SGL_LINE_LOOP);
        for (i = 0; i < numInitSegments; i++)
        {
            float angle = segment * (float)i;
            sglVertex2f(offsetx + r * cos(angle), offsety + r * sin(angle));
        }
        sglEnd();

    } // for r...

    offsety += 2.5 * r;

    sglAreaMode(SGL_LINE);

    r = 2;

    // ellipse drawing
    sglColor3f(1, 0, 1);
    for (i = 0; i < numInitSegments; i++)
    {
        sglEllipse(0, 0, 0, (i + 1) * 2.5 * r / numInitSegments, (numInitSegments - i) * 2.5 * r / numInitSegments);
    }

    sglColor3f(0, 1, 1);

    sglEllipse(-7, -1, 0, 2.9, 0.2);
    sglEllipse(7, 0, 0, 0.2, 2.9);

    offsetx = -7.25;
    offsety = 6;

    sglColor3f(1, 1, 0);

    for (float rr = r / 5; rr <= r; rr += r / 5)
    {
        sglCircle(offsetx - 1, offsety, 0, 0.5 * rr);
        sglCircle(offsetx + 3, offsety, 0, 1.5 * rr);
    }

    offsetx = r;

    sglColor3f(1, 1, 1);

    for (float rr = 0.4; rr <= 4; rr += 0.4)
    {
        sglArc(offsetx, offsety, 0, rr, 0, M_PI / 2);
        sglArc(offsetx + 4, offsety, 0, rr, M_PI * 2 / 2, M_PI * 3 / 2);
    }
}

/// NFF drawing test
void DrawTestScene2D(NFFStore &nffstore)
{
    sglAreaMode(SGL_FILL);
    sglEnable(SGL_DEPTH_TEST);
    // set viewport
    sglViewport(0, 0, WIDTH, HEIGHT);

    // projection transformation
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sgluPerspective(nffstore.angle, (float)WIDTH / HEIGHT, 1.0, 1800.0);

    // modelview transformation
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();
    sgluLookAt(
        nffstore.from.x,
        nffstore.from.y,
        nffstore.from.z,
        nffstore.at.x,
        nffstore.at.y,
        nffstore.at.z,
        nffstore.up.x,
        nffstore.up.y,
        nffstore.up.z);

    // render the geometry from the NFF file
    NFFStore::TMatGroupList::const_iterator giter = nffstore.matgroups.begin();
    for (; giter != nffstore.matgroups.end(); ++giter)
    {
        const NFFStore::Material &m = giter->material;
        sglColor3f(m.col.r * m.kd, m.col.g * m.kd, m.col.b * m.kd);
        const NFFStore::TriangleList &tlist = giter->geometry;

        NFFStore::TriangleList::const_iterator titer = tlist.begin();
        for (; titer != tlist.end(); ++titer)
        {
            const NFFStore::Triangle &t = *titer;
            sglBegin(SGL_POLYGON);
            for (int i = 0; i < 3; i++)
                sglVertex3f(t.vertices[i].x, t.vertices[i].y, t.vertices[i].z);
            sglEnd();
        }
    }
}

/// NFF drawing test
float RayTraceScene(const char *scenename, unsigned int iter = 1)
{
    NFFStore nffstore(false);
    Timer timer;

    FILE *f = fopen(scenename, "rt");
    if (!f)
    {
        cerr << "Could not open " << scenename << " for reading." << std::endl;
        return 0;
    }

    char errstring[4000];
    if (ReadNFF(f, errstring, &nffstore) < 0)
    {
        cerr << "Error in NFF file " << scenename << ":\n"
             << errstring << std::endl;
        fclose(f);
        return 0;
    }
    fclose(f);

    cout << "NFF file " << scenename << " successfully parsed." << endl;

    timer.Restart();

    // projection transformation
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    // modelview transformation
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();

    /// BEGIN SCENE DEFINITION
    sglBeginScene();
    // iterate over all the geometry from the NFF file
    NFFStore::TMatGroupList::const_iterator giter = nffstore.matgroups.begin();
    for (; giter != nffstore.matgroups.end(); ++giter)
    {
        const NFFStore::Material &m = giter->material;
        sglMaterial(m.col.r,
                    m.col.g,
                    m.col.b,
                    m.kd,
                    m.ks,
                    m.shine,
                    m.T,
                    m.ior);

        /// store all polygons (converted into triangles)
        const NFFStore::TriangleList &tlist = giter->geometry;
        NFFStore::TriangleList::const_iterator titer = tlist.begin();
        for (; titer != tlist.end(); ++titer)
        {
            const NFFStore::Triangle &t = *titer;
            sglBegin(SGL_POLYGON);
            for (int i = 0; i < 3; i++)
                sglVertex3f(t.vertices[i].x, t.vertices[i].y, t.vertices[i].z);
            sglEnd();
        }

        /// store spheres
        const NFFStore::SphereList &slist = giter->spheres;
        NFFStore::SphereList::const_iterator siter = slist.begin();
        for (; siter != slist.end(); ++siter)
        {
            const NFFStore::Sphere &s = *siter;
            sglSphere(s.center.x,
                      s.center.y,
                      s.center.z,
                      s.radius);
        }
    }

    // iterate over all point lights from the NFF file
    std::list<NFFStore::PointLight>::const_iterator liter = nffstore.pointLights.begin();
    for (; liter != nffstore.pointLights.end(); ++liter)
    {
        const NFFStore::PointLight &l = *liter;
        sglPointLight(l.position.x,
                      l.position.y,
                      l.position.z,
                      l.intensity.r,
                      l.intensity.g,
                      l.intensity.b);
    }

    // iterate over all the geometry from the NFF file
    NFFStore::TLightGroupList::const_iterator aliter = nffstore.lightgroups.begin();
    for (; aliter != nffstore.lightgroups.end(); ++aliter)
    {
        sglEmissiveMaterial(aliter->intensity.r,
                            aliter->intensity.g,
                            aliter->intensity.b,
                            aliter->atten.x,
                            aliter->atten.y,
                            aliter->atten.z);

        /// store all polygons (converted into triangles)
        const NFFStore::TriangleList &tlist = aliter->geometry;
        NFFStore::TriangleList::const_iterator titer = tlist.begin();
        for (; titer != tlist.end(); ++titer)
        {
            const NFFStore::Triangle &t = *titer;
            sglBegin(SGL_POLYGON);
            for (int i = 0; i < 3; i++)
                sglVertex3f(t.vertices[i].x, t.vertices[i].y, t.vertices[i].z);
            sglEnd();
        }
    }

    if (nffstore.envMap.cols)
    {
        sglEnvironmentMap(nffstore.envMap.width,
                          nffstore.envMap.height,
                          nffstore.envMap.cols);
    }

    sglEndScene();
    /// END SCENE DEFINITION

    sglAreaMode(SGL_FILL);
    sglEnable(SGL_DEPTH_TEST);
    sglClearColor(nffstore.bg_col.r, nffstore.bg_col.g, nffstore.bg_col.b, 1);
    sglClear(SGL_COLOR_BUFFER_BIT | SGL_DEPTH_BUFFER_BIT);

    // set the viewport transform
    sglViewport(0, 0, WIDTH, HEIGHT);

    // setup the camera using appopriate projection transformation
    // note that the resolution stored in the nff file is ignored
    sglMatrixMode(SGL_PROJECTION);
    sglLoadIdentity();
    sgluPerspective(nffstore.angle, (float)WIDTH / HEIGHT, 1.0, 1800.0);

    // modelview transformation
    sglMatrixMode(SGL_MODELVIEW);
    sglLoadIdentity();
    sgluLookAt(
        nffstore.from.x,
        nffstore.from.y,
        nffstore.from.z,
        nffstore.at.x,
        nffstore.at.y,
        nffstore.at.z,
        nffstore.up.x,
        nffstore.up.y,
        nffstore.up.z);

    // compute a ray traced image and store it in the color buffer
    for (unsigned int i = 0; i < 1; i++)
        sglRayTraceScene();

    return timer.RealTime();
}

/// Init SGL
void Init(void)
{
    sglInit();
    for (int i = 0; i < 10; i++)
        _contexts[i] = -1;
    for (int i = 0; i < NUM_CONTEXTS; i++)
        _contexts[i] = sglCreateContext(WIDTH, HEIGHT);
    sglSetContext(_contexts[0]);
}

/// Clean up SGL
void CleanUp(void)
{
    /// destroys all created contexts
    sglFinish();
}

//#if USE_GUI
////////// GLFW bindings //////////////

/// Callback for processing special key input
void processInput(GLFWwindow *window)
{
    // application finishes upon pressing q
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
       glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, true);
    }
    int context_num = -1;
    if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) { context_num = 0; }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { context_num = 1; }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { context_num = 2; }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { context_num = 3; }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { context_num = 4; }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) { context_num = 5; }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) { context_num = 6; }
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) { context_num = 7; }
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) { context_num = 8; }
    else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) { context_num = 9; }

    if(context_num != -1)
    {
        sglSetContext(_contexts[context_num]);
        sglEErrorCode err = sglGetError();
        if (err != SGL_NO_ERROR)
            cerr << "Failed to switch context: " << sglGetErrorString(err) << endl;
    }

    /// rotate and translate cubes in test 2C
    bool updated = false;
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        updated = true;
        tx -= tstep;
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        updated = true;
        ty += tstep;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        updated = true;
        tx += tstep;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        updated = true;
        ty -= tstep;
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        updated = true;
        tz += tstep;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        updated = true;
        tz -= tstep;
    }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        updated = true;
        rot += rotstep;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        SGL_DEBUG_OUT("Setting show_depth to true");
        show_depth = true;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        SGL_DEBUG_OUT("Setting show_depth to false");
        show_depth = false;
    }
#ifdef TEST_2C
    if (updated)
    {
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT | SGL_DEPTH_BUFFER_BIT);
        DrawTestScene2C();
    }
#endif
}

// #endif
// glfw error callback
void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void sgl_tests()
{
    auto test_err_code = [](sglEErrorCode err){
        if(sglGetError() != err)
        {
            std::cout << "[TestApp::main()] Invalid error code" << std::endl;
            exit(-1);
        }
    };

    sglInit();

    // ============== TEST 1 - get context when no contexts exit ============
    std::cout << "TEST 1 - get context error code" << std::endl;
    sglGetContext();
    test_err_code(sglEErrorCode::SGL_INVALID_OPERATION);
    // =====================================================================

    // ============== TEST 1 - setup contexts ============
    std::cout << "TEST 1.5 - setup contexts" << std::endl;
    int test_ctx1, test_ctx2, test_ctx3;
    test_ctx1 = sglCreateContext(1000, 1000);
    test_ctx2 = sglCreateContext(800, 800);
    test_ctx3 = sglCreateContext(400, 400);
    std::cout << "======================================================\n";
    // =====================================================================

    // ============== TEST 2 - test destroy context ========================
    std::cout << "TEST 2 - test destroy context" << std::endl;
    sglDestroyContext(test_ctx2);
    std::cout << "======================================================\n";
    // =====================================================================

    // ============= TEST 3 - test destroy destroyed context === 
    std::cout << "TEST 3 - test destroy destroyed context : " << std::endl;
    sglDestroyContext(test_ctx2);
    test_err_code(sglEErrorCode::SGL_INVALID_VALUE);
    std::cout << "======================================================\n";
    // =====================================================================

    // ============= TEST 4 - test set destroyed context ===================
    std::cout << "TEST 4 - test set destroyed context" << std::endl;
    sglSetContext(test_ctx2); 
    test_err_code(sglEErrorCode::SGL_INVALID_VALUE);
    std::cout << "======================================================\n";
    // =====================================================================

    // ============ TEST 5 - test create context reuse destroyed idx ======
    std::cout << "TEST 5 - test create context reuse destroyed idx" << std::endl; 
    test_ctx2 = sglCreateContext(800, 800);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 6 - test set context ===============================
    std::cout << "TEST 6 - test set context" << std::endl; 
    sglSetContext(test_ctx1);
    std::cout << "======================================================\n";
    // =====================================================================

    // TEST FOR MATRICES ===================================================
    sglSetContext(test_ctx3);
    // =========== TEST 7 - test pop matrix error ==========================
    std::cout << "TEST 7 - test pop matrix error" << std::endl; 
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_STACK_UNDERFLOW);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 8 - test push matrix ===============================
    std::cout << "TEST 8 - test push matrix" << std::endl; 
    sglPushMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    sglPopMatrix();
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 9 - test load matrix ===============================
    std::cout << "TEST 9 - test load matrix" << std::endl; 
    sglPushMatrix();
    float m[] = {
        1.0f, 1.0f, 1.0f, 1.0f,					   // col 1
        2.0f, 2.0f, 2.0f, 2.0f,					   // col 2
        3.0f, 3.0f, 3.0f, 3.0f,					   // col 3
        4.0f, 4.0f, 4.0f, 4.0f};				   // col 4
    sglLoadMatrix(m);
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    sglPopMatrix();
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 10 - test scale matrix ===============================
    std::cout << "TEST 10 - test scale matrix" << std::endl; 
    sglPushMatrix();
    sglScale(2.0f, 3.0f, 0.5f);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 11 - test load identity matrix ===============================
    std::cout << "TEST 11 - test load identity matrix" << std::endl; 
    sglPushMatrix();
    sglLoadIdentity();
    sglScale(2.0f, 3.0f, 0.5f);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 12 - test mult matrix ===============================
    std::cout << "TEST 12 - test mult matrix" << std::endl; 
    sglPushMatrix();
    sglLoadIdentity();
    float g[] = {
        2.0f, 0.0f, 0.0f, 0.0f,					   // col 1
        0.0f, 3.0f, 0.0f, 0.0f,					   // col 2
        0.0f, 0.0f, 4.0f, 0.0f,					   // col 3
        0.0f, 0.0f, 0.0f, 1.0f};				   // col 4
    sglMultMatrix(g);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 13 - test mult matrix 2 ===============================
    std::cout << "TEST 13 - test mult matrix 2" << std::endl; 
    sglPushMatrix();
    sglLoadIdentity();
    float f[] = {
        1.0f, 0.0f, 0.0f, 0.0f,					   // col 1
        1.0f, 1.0f, 0.0f, 0.0f,					   // col 2
        0.0f, 1.0f, 3.0f, 0.0f,					   // col 3
        0.0f, 0.0f, 0.0f, 1.0f};				   // col 4
    sglMultMatrix(f);
    sglMultMatrix(g);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 14 - test switch mode ==============================
    std::cout << "TEST 14 - test switch mode" << std::endl; 
    sglMatrixMode(sglEMatrixMode::SGL_MODELVIEW);
    sglPushMatrix();
    sglLoadMatrix(f);
    sglMatrixMode(sglEMatrixMode::SGL_PROJECTION);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_STACK_UNDERFLOW);
    sglLoadIdentity();
    sglPushMatrix();
    sglPopMatrix();
    sglMatrixMode(sglEMatrixMode::SGL_MODELVIEW);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 15 - test translate matrix =========================
    std::cout << "TEST 15 - test translate matrix" << std::endl; 
    sglPushMatrix();
    sglLoadIdentity();
    sglTranslate(-2, 3, 4);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 16 - test scale matrix =========================
    std::cout << "TEST 16 - test scale matrix" << std::endl; 
    sglPushMatrix();
    sglLoadIdentity();
    sglScale(2, 0.5, 3.0);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 17 - test rotateY matrix =========================
    std::cout << "TEST 17 - test rotateY matrix" << std::endl; 
    sglPushMatrix();
    sglLoadIdentity();
    sglRotateY(1.57079633f);
    sglPopMatrix();
    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    // =========== TEST 18 - test vertex transform =========================
    std::cout << "TEST 18 - test vertex transform" << std::endl; 
    sglMatrixMode(sglEMatrixMode::SGL_MODELVIEW);
    sglPushMatrix();
    sglLoadIdentity();
    sglTranslate(-2.0f, 3.0f, 0.5f);
    sglScale(3.0f, 3.0f, 3.0f);
    sglMatrixMode(sglEMatrixMode::SGL_PROJECTION);
    sglPushMatrix();
    sglLoadIdentity();
    sglAreaMode(sglEAreaMode::SGL_POINT);
    sglBegin(sglEElementType::SGL_POINTS);
    sglVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
    sglEnd();
    sglPopMatrix();
    sglMatrixMode(sglEMatrixMode::SGL_MODELVIEW);
    sglPopMatrix();

    test_err_code(sglEErrorCode::SGL_NO_ERROR);
    std::cout << "======================================================\n";
    // =====================================================================

    sglFinish();

    exit(1);
}

int main(int argc, char **argv)
{
    runMultiplier = (argc > 1 ? atoi(argv[1]) : 100);

//#if USE_GUI
    // Initialize GLUT
    if(!glfwInit())
    {
        std::cerr << "[[main()] Failed to init GLFW]" << std::endl;
        return 1;
    }
    glfwSetErrorCallback(error_callback);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // TODO(msakmary) Using compat profile is cursed, switch to CORE profile and 
    // do proper implementation
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SGL test app", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        std::cerr << "[[main()] Failed to create window]" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);
//#endif


    // init SGL
    // sgl_tests();
    Init();


    // run and benchmark the tests
    Timer timer;
    double totalTime = 0;

    std::ofstream resultsInfo("results/desc.neon");
    resultsInfo << "res:" << endl;

#ifdef TEST_0A
    {
        cout << "test 0a..." << flush;
        timer.Restart();
        sglSetContext(_contexts[0]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);

        DrawTestScene0A();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test0a.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test0a.tga");
    }
#endif

#ifdef TEST_0B
    {
        cout << "test 0b..." << flush;
        timer.Restart();
        sglSetContext(_contexts[1]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);
        DrawTestScene0B();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test0b.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test0b.tga");
    }
#endif

#ifdef TEST_1A
    {
        cout << "test 1a..." << flush;
        timer.Restart();

        sglSetContext(_contexts[0]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);
        for (int i = 0; i < 1000; i++)
            DrawTestScene1A();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test1a.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test1a.tga");
    }
#endif

#ifdef TEST_1B
    {
        cout << "test 1b..." << flush;
        timer.Restart();

        sglSetContext(_contexts[1]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);
        for (int i = 0; i < 1000; i++)
            DrawTestScene1B();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test1b.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test1b.tga");
    }
#endif

#ifdef TEST_1C
    {
        cout << "test 1c..." << flush;
        timer.Restart();

        sglSetContext(_contexts[2]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);
        for (int i = 0; i < 1000; i++)
            DrawTestScene1C();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test1c.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test1c.tga");
    }
#endif

#ifdef TEST_2A
    {
        cout << "test 2a..." << flush;
        timer.Restart();

        sglSetContext(_contexts[3]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);
        for (int i = 0; i < 150; i++)
            DrawTestScene2A();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test2a.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test2a.tga");
    }
#endif

#ifdef TEST_2B
    {
        cout << "test 2b..." << flush;
        timer.Restart();

        sglSetContext(_contexts[4]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT);
        for (int i = 0; i < 15; i++)
            DrawTestScene2B();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test2b.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test2b.tga");
    }
#endif

#ifdef TEST_2C
    {
        cout << "test 2c..." << flush;
        timer.Restart();

        sglSetContext(_contexts[5]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT | SGL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < 15; i++)
            DrawTestScene2C();

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test2c.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test2c.tga");
    }
#endif

#ifdef TEST_2D
    {
        NFFStore nffstore;

        /// read in the NFF file
        const char *scenename = "resources/cornell-spheres.nff";

        FILE *f = fopen(scenename, "rt");
        if (!f)
        {
            cerr << "Could not open " << scenename << " for reading." << std::endl;
            return 2;
        }

        char errstring[4000];
        if (ReadNFF(f, errstring, &nffstore) < 0)
        {
            cerr << "Error in NFF file " << scenename << ":\n"
                 << errstring << std::endl;
            fclose(f);
            return 2;
        }
        fclose(f);

        cerr << "NFF file " << scenename << " successfully parsed." << endl;

        cout << "test 2d..." << flush;
        timer.Restart();

        sglSetContext(_contexts[0]);
        sglClearColor(0, 0, 0, 1);
        sglClear(SGL_COLOR_BUFFER_BIT | SGL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < 15; i++)
            DrawTestScene2D(nffstore);

        double time = timer.RealTime();
        totalTime += time;

        resultsInfo << "    test2d.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test2d.tga");
    }
#endif

#ifdef TEST_3A
    {
        cout << "test 3a..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/sphere.nff";
        sglSetContext(_contexts[1]);
        double time = RayTraceScene(sceneFile, runMultiplier);
        totalTime += time;

        resultsInfo << "    test3a.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test3a.tga");
    }
#endif

#ifdef TEST_3B
    {
        cout << "test 3b..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/cornell.nff";
        sglSetContext(_contexts[2]);
        double time = RayTraceScene(sceneFile, runMultiplier);
        totalTime += time;

        resultsInfo << "    test3b.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test3b.tga");
    }
#endif

#ifdef TEST_3C
    {
        cout << "test 3c..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/floor_sph.nff";
        sglSetContext(_contexts[3]);
        double time = RayTraceScene(sceneFile, runMultiplier);
        totalTime += time;

        resultsInfo << "    test3c.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test3c.tga");
    }
#endif

#ifdef TEST_4A
    {
        cout << "test 4a..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/cornell-blocks.nff";
        sglSetContext(_contexts[3]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test4a.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test4a.tga");
    }
#endif

#ifdef TEST_4B
    {
        cout << "test 4b..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/cornell-spheres-raytrace.nff";
        sglSetContext(_contexts[4]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test4b.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test4b.tga");
    }
#endif

#ifdef TEST_4C
    {
        cout << "test 4c..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/cornell-spheres-crazy.nff";
        sglSetContext(_contexts[5]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test4c.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test4c.tga");
    }
#endif

#ifdef TEST_5A
    {
        cout << "test 5a..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/cornell-blocks-arealight.nff";
        sglSetContext(_contexts[0]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test5a.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test5a.tga");
    }
#endif

#ifdef TEST_5B
    {
        cout << "test 5b..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/cornell-blocks-arealight-color.nff";
        sglSetContext(_contexts[1]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test5b.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test5b.tga");
    }
#endif

#ifdef TEST_5C
    {
        cout << "test 5c..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/basilica.nff";
        sglSetContext(_contexts[2]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test5c.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test5c.tga");
    }
#endif

#ifdef TEST_5D
    {
        cout << "test 5d..." << flush;

        /// read in the NFF file
        const char *sceneFile = "resources/uffizi.nff";
        sglSetContext(_contexts[3]);
        double time = RayTraceScene(sceneFile);
        totalTime += time;

        resultsInfo << "    test5d.png : " << time << endl;
        cout << "done in " << time << " sec." << endl;
        WriteTGA("results/test5d.tga");
    }
#endif

// #if USE_GUI
    // execute main application loop for event processing
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);
        float *cb = sglGetColorBufferPointer();
        std:array<float, WIDTH * HEIGHT * sizeof(float)> rescaled_depth;
        if(show_depth){
            // rescale depth buffer
            cb = sglGetDepthBufferPointer();
            memcpy(rescaled_depth.data(), cb, rescaled_depth.size());
            float min = 1.0f;
            float max = 0.0f;
            for(int i = 0; i < WIDTH * HEIGHT * sizeof(float); i++)
            {
                if(cb[i] < min && cb[i] >= 0.5f) 
                {
                    min = cb[i];
                }
                if(cb[i] > max) {max = cb[i];}
            }
            for(int i = 0; i < WIDTH * HEIGHT * sizeof(float); i++)
            {
                rescaled_depth[i] = cb[i];
                rescaled_depth[i] -= min;
                rescaled_depth[i] /= (max - min);
            }
        }
        if (cb)
        {
            if(show_depth)
            {
                glDrawPixels(WIDTH, HEIGHT, GL_LUMINANCE, GL_FLOAT, rescaled_depth.data());
            } else {
                glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_FLOAT, cb);
            };
        }

        // swap buffers (float buffering)
        glfwSwapBuffers(window);
    }
// #else
    resultsInfo << "TotalTime : " << totalTime << std::endl;
    std::cout << "TotalTime : " << totalTime << std::endl;
    CleanUp();
// #endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
