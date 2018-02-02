#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static const char * GetEGLErrorString(int errID)
{
    switch(errID) {
      case EGL_SUCCESS:
        return "The last function succeeded without error.";

      case EGL_NOT_INITIALIZED:
        return "EGL is not initialized, or could not be initialized, for the specified EGL display connection.";

      case EGL_BAD_ACCESS:
        return "EGL cannot access a requested resource (for example a context is bound in another thread).";

      case EGL_BAD_ALLOC:
        return "EGL failed to allocate resources for the requested operation.";

      case EGL_BAD_ATTRIBUTE:
        return "An unrecognized attribute or attribute value was passed in the attribute list.";

      case EGL_BAD_CONTEXT:
        return "An EGLContext argument does not name a valid EGL rendering context.";

      case EGL_BAD_CONFIG:
        return "An EGLConfig argument does not name a valid EGL frame buffer configuration.";

      case EGL_BAD_CURRENT_SURFACE:
        return "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid.";

      case EGL_BAD_DISPLAY:
        return "An EGLDisplay argument does not name a valid EGL display connection.";

      case EGL_BAD_SURFACE:
        return "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering.";

      case EGL_BAD_MATCH:
        return "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface).";

      case EGL_BAD_PARAMETER:
        return "One or more argument values are invalid.";

      case EGL_BAD_NATIVE_PIXMAP:
        return "A NativePixmapType argument does not refer to a valid native pixmap.";

      case EGL_BAD_NATIVE_WINDOW:
        return "A NativeWindowType argument does not refer to a valid native window.";

      case EGL_CONTEXT_LOST:
        return "A power management event has occurred. The application must destroy all contexts and reinitialise OpenGL ES state and objects to continue rendering.";
        default:
            return "UNKNOWN ERROR";
    }
    return "UNKNOWN ERROR";
}

static inline void CheckEGLError(const char *stmt, const char *fname, int line)
{
    int error = eglGetError();

    //if (error != GL_NO_ERROR)
    if (error != EGL_SUCCESS)
    {
      printf("eglGetError = %d, (%s) at %s:%d - for %s\n", error, GetEGLErrorString(error), fname, line, stmt);
      exit(0);
    }

}


#ifndef GL_CHECK
#define GL_CHECK(stmt) do { \
stmt; \
CheckEGLError(#stmt, __FILE__, __LINE__); \
} while(0);
#else
#define GL_CHECK(stmt) stmt
#endif

static void chooseDisplay( EGLDisplay *eglDpy)
{
  static const EGLint configAttribs[] = {
          EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_DEPTH_SIZE, 8,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
          EGL_NONE
  };    
  // static const EGLint configAttribs[] = {
  //         EGL_BLUE_SIZE, 8,
  //         EGL_GREEN_SIZE, 8,
  //         EGL_RED_SIZE, 8,
  // };    

  static const int pbufferWidth = 9;
  static const int pbufferHeight = 9;

  static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
  };


  EGLint major=0, minor=0;

  EGLBoolean eglInit=EGL_FALSE;
  //GL_CHECK(eglInit = eglInitialize(eglDpy, &major, &minor));
  eglInit = eglInitialize(*eglDpy, &major, &minor);
  printf("initialized=%d, major=%d, minor=%d\n", eglInit, major, minor);
if(eglInit == EGL_FALSE)return;

  // 2. Select an appropriate configuration
  EGLint numConfigs;
  EGLConfig eglCfg;

  GL_CHECK(eglChooseConfig(*eglDpy, configAttribs, &eglCfg, 1, &numConfigs));
  //eglChooseConfig(*eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

  // 3. Create a surface
  EGLSurface eglSurf;
  GL_CHECK(eglSurf = eglCreatePbufferSurface(*eglDpy, eglCfg, pbufferAttribs));
  //eglSurf = eglCreatePbufferSurface(*eglDpy, eglCfg, pbufferAttribs);

  // 4. Bind the API
  GL_CHECK(eglBindAPI(EGL_OPENGL_API));
  //eglBindAPI(EGL_OPENGL_API);

  // 5. Create a context and make it current
  EGLContext eglCtx;
  GL_CHECK(eglCtx = eglCreateContext(*eglDpy, eglCfg, EGL_NO_CONTEXT, NULL));
  //eglCtx = eglCreateContext(*eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);

  //GL_CHECK(eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx));
  eglMakeCurrent(*eglDpy, eglSurf, eglSurf, eglCtx);
  //GL_CHECK(eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, eglCtx));

  // from now on use your OpenGL context

  printf("GL_VERSION %s\n", glGetString(GL_VERSION));
  printf("GL_VENDOR %s\n", glGetString(GL_VENDOR));
  printf("GL_RENDERER %s\n", glGetString(GL_RENDERER));
  const char *the_extensions = (const char*) glGetString(GL_EXTENSIONS);
  if(the_extensions)
  {
    printf("Has Extensinos.\n");
  }
  else
  {
    printf("Does not have Extensions.\n");
  }

  // 6. Terminate EGL when finished
  //GL_CHECK(eglTerminate(eglDpy));
  eglTerminate(eglDpy);
}

int main(int argc, char *argv[])
{
  static const int MAX_DEVICES = 4;
  EGLDeviceEXT eglDevs[MAX_DEVICES];
  EGLint numDevices;
  EGLDisplay eglDpys[MAX_DEVICES];

  PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT;
  GL_CHECK(eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC) eglGetProcAddress("eglQueryDevicesEXT"));
  //eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC) eglGetProcAddress("eglQueryDevicesEXT");
  printf("pass 1\n");

  GL_CHECK(eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices));
  //eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);

  printf("Detected %d devices\n", numDevices);
  assert(numDevices <= MAX_DEVICES && "There are more devices then allocated for.\n");

  PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
  GL_CHECK(eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC) eglGetProcAddress("eglGetPlatformDisplayEXT"));
  //eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC) eglGetProcAddress("eglGetPlatformDisplayEXT");

  GL_CHECK(eglDpys[0] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[0], 0));
  //eglDpys[0] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[0], 0);
  chooseDisplay(&eglDpys[0]);

  GL_CHECK(eglDpys[1] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[1], 0));
  //eglDpys[1] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[1], 0);
  chooseDisplay(&eglDpys[1]);

  GL_CHECK(eglDpys[2] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[2], 0));
  //eglDpys[2] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[2], 0);
  chooseDisplay(&eglDpys[2]);

  GL_CHECK(eglDpys[3] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[3], 0));
  //eglDpys[3] = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[3], 0);
  chooseDisplay(&eglDpys[3]);


}




