#ifndef GLOP_HPP
#define GLOP_HPP

// --- Windows Platform ---
#ifndef GLO_WIN

// Check for windows...
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

// Define GLO platform...
#define GLO_WIN

// Windows includes...
#include <Windows.h>
#include <gl/GL.h>
#include "../glext.h"
#include "../wglext.h"

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);

// Platform specific compiler warning...
#define GLO_WARNING(msg) 

#endif

#endif
// --- Windows Platform ---


// --- X.org Platform ---
#ifndef GLO_X

#if defined(__linux__) || defined(__unix__)

#define GLO_X

// X11 includes
#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))glXGetProcAddress((const GLubyte *)#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))glXGetProcAddress((const GLubyte *)#name);

#endif

#endif
// --- X.org Platform ---


// OpenGL versions...


#endif // GLOP_HPP

