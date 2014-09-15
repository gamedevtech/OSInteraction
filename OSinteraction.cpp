﻿#include "osinteraction.h"
bool chatty= false;  /// used only for DEBUG


// Create a 'probe' context on each monitor, see what it (OpenGL) returns (a product ID or something)
//^^^^^^^^^^^^^^^

/* TODO:
 * - all internals to have '_' in front, and all or most funcs that are internal, should be outside their class/
 * - [mac] better glMakeCurrent func, with the coreGl mac stuff << MAX PRIORITY
 * - [all] extensions in win will be tied to glRenderer; inline funcs must be done for EVERY extension;
 *   function pointers are aquired differently on each system;

 * - [win] USING_XINPUT & USING_DIRECTINPUT must be inside OS_WIN
 * - system to create a glRenderer for each graphic card (MUST install a second grcard on a computer) !!!!
 * 
 * - [linux][mac] threads!!!!!!!!!!!!
 *
 * - [linux][mac] set an icon for the window;  [win] WHEN dealing with icons, must remember to develop WM_GETICON too
 *
 * - gamepad vibration under directinput (it is possible) & force feedback (MUST HAVE A VIBRATION GAMEPAD FIRST...)
 *
 * - create a loading window, in the center of the screen? eventually to have image of the game
 *
 * LOWER PRIORITY:
 * - rename Input class... osiInput or osiinput or osiInput (might rename all classes to 'osi' style)
 * - [linux][mac] prevent screensaver/ monitor low power
 * - [win][linux][mac] what happens on sleep? should be handled like 'app lose focus', or better, another flag, as the app must pause or something (some dont pause on app focus)
 * - windowfocus flag. it's there, but not updated at all
 * - [linux] test mouse grab
 * - [linux] test keyboard grab (first make shure it is possible to exit program)
 *
 * - [win][linux][mac] more messing with pixel formats, after a good testing 'chamber' in openGL is created
 * 
 * - [win][linux][mac] joystick, wheel (BUY THEM first)
 *
 * - [WIN] WM_SETFOCUS & WM_KILLFOCUS - develop these? WM_ACTIVATE handles focus change atm.
 * 
 * - if there is 1 glRenderer per window, glShareLists knows not to make duplicates if in the same graphics card??? THIS IS THE QUESTION.
 *
 * - [linux] test monitors on duplicate (on mirror, on whatever crap the os calls them)
 *
 * - window to change a monitor without problems (unplug?)
 *    but on multiple monitor mode, mark it as closed? something like this... notify the program somehow, so a rearrangement will be done
 *
 * - [all]  Libcmt.lib try eliminating printf? alternative : console to write to file; problem: format!!!!
 *
 * - keyboard mode 2 in is getting more and more useless and a big drag... and is not avaible in linux + mac
 * - what happens when the time variables overflow? must do something about that (osi.getNano, etc)
 * - [mac] errorHandling window
*/


// LINUX WINDOW RESEARCH

// ONLY 1 DISPLAY MUST BE CREATED. it is 'the connection' to the 'server'(linux/computer, call it what u like)

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// >>> WHAT HAPPENS WHEN MULTIPLE GRAPHICS CARDS ARE PRESENT? SOME SAY THERE ARE MULTIPLE DISPLAY CONNECTIONS AVAIBLE <<<
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

/* -Two commonly met examples are Maximized and Shaded. A window manager may implement these as proper substates
 *  of NormalState and IconicState, or it may treat them as independent flags, allowing e.g. a maximized window
 *  to be iconified and to re-appear as maximized upon de-iconification.
 *
 * -Root Window Properties (and Related Messages)
 *  Whenever this spec speaks about “sending a message to the root window”, it is understood that the client
 *  is supposed to create a ClientMessage event with the specified contents and send it by using a SendEvent
 *  request with the following arguments:
 *
 *   destination     root
 *   propagate       False
 *   event-mask      (SubstructureNotify|SubstructureRedirect)
 *   event           the specified ClientMessage
 *
 *   _NET_SUPPORTED
 *      _NET_SUPPORTED, ATOM[]/32
 *
 *   This property MUST be set by the Window Manager to indicate which hints it supports.
 *   For example: considering _NET_WM_STATE both this atom and all supported states
 *   e.g. _NET_WM_STATE_MODAL, _NET_WM_STATE_STICKY, would be listed. This assumes
 *   that backwards incompatible changes will not be made to the hints (without being renamed).
 *
 *
  _NET_ACTIVE_WINDOW
      _NET_ACTIVE_WINDOW, WINDOW/32

 The window ID of the currently active window or None if no window has the focus. This is a read-only property
   set by the Window Manager. If a Client wants to activate another window, it MUST send a _NET_ACTIVE_WINDOW
   client message to the root window:
^
_NET_ACTIVE_WINDOW
  window  = window to activate
  message_type = _NET_ACTIVE_WINDOW
  format = 32
  data.l[0] = source indication
  data.l[1] = timestamp
  data.l[2] = requestor's currently active window, 0 if none
  other data.l[] elements = 0

 Source indication should be 1 when the request comes from an application, and 2 when it comes from a pager.
   Clients using older version of this spec use 0 as source indication, see the section called “Source indication
   in requests” for details. The timestamp is Client's last user activity timestamp (see _NET_WM_USER_TIME) at the
   time of the request, and the currently active window is the Client's active toplevel window, if any
   (the Window Manager may be e.g. more likely to obey the request if it will mean transferring focus
   from one active window to another).

 Depending on the information provided with the message, the Window Manager may decide to refuse the request
   (either completely ignore it, or e.g. use _NET_WM_STATE_DEMANDS_ATTENTION).
 *
 _NET_CLOSE_WINDOW
    _NET_CLOSE_WINDOW

 Pagers wanting to close a window MUST send a _NET_CLOSE_WINDOW client message request to the root window:
_NET_CLOSE_WINDOW
  window = window to close
  message_type = _NET_CLOSE_WINDOW
  format = 32
  data.l[0] = timestamp
  data.l[1] = source indication
  other data.l[] elements = 0

 The Window Manager MUST then attempt to close the window specified. See the section called “Source
   indication in requests” for details on the source indication.

 Rationale: A Window Manager might be more clever than the usual method (send WM_DELETE message if the protocol
   is selected, XKillClient otherwise). It might introduce a timeout, for example. Instead of duplicating
   the code, the Window Manager can easily do the job.

 *
 */

/*
opengl extensions; unfortunately, in windows, they are tied to the context; using 2 grcards they are for shure tied to each context
need that dang computer!!! finish the renderer class / osi
the pfnblablaFunc stuff works in windows only, it seems; test in linux
*/
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#ifdef OS_WIN
  #pragma comment(lib, "opengl32")
  #pragma comment(lib, "glu32")
  #pragma comment(lib, "Winmm")               /// joystick api support
  #ifdef USING_DIRECTINPUT
    #pragma comment(lib, "dinput8")
    #pragma comment(lib, "../../dxSDK2010j/lib/x86/dxguid")
    #pragma comment(lib, "../../dxSDK2010j/lib/x64/dxguid")

  #endif
  #ifdef USING_XINPUT

    #pragma comment(lib, "../../dxSDK2010j/lib/x64/xinput")
    #pragma comment(lib, "../../dxSDK2010j/lib/x86/xinput")
  #endif
#endif /// OS_WIN


// >>> OBJECTS CREATION <<< //
///========================///
osinteraction osi;
osiInput in;
ErrorHandling error;


// ################
// Testing purposes
// ################


/*
double pov(double x, double y)
  // GOOD VARIANT

  if(y> 0) {
    if(x>= 0)
      ret= (atan(x/ y))* (180.0/ M_PI);
    else
      ret= (2* M_PI+ atan(x/ y))* (180.0/ M_PI);
  } else if(y< 0) {
    ret= ( M_PI+ atan(x/ y))* (180.0/ M_PI);
  } else if(y == 0) {

    if(x== 0)
      ret= -1;
    else if(x> 0)
      ret= 90;
    else if(x< 0)
      ret= 270;
  }
  return ret;
}
*/
/*
int getX(double a) {
  double x;
  x= 10.0* sin(a* (M_PI/ 180.0));

  return x;
}
int getY(double a) {

  double y;
  y= 10.0* cos(a* (M_PI/ 180.0));

  return y;
}
*/

void drawSomething();
/*
int main() {

  //printf("Q1[% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f]\n", pov( 0,    100), pov( 5,    100), pov( 50,   100), pov( 100,  100), pov( 100,  50),  pov( 100,  5));
  //printf("Q2[% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f]\n", pov( 100,  0),   pov( 100, -5),   pov( 100, -50),  pov( 100, -100), pov( 50,  -100), pov( 5,   -100));
  //printf("Q3[% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f]\n", pov( 0,   -100), pov(-5,   -100), pov(-50,  -100), pov(-100, -100), pov(-100, -50),  pov(-100, -5));
  //printf("Q4[% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f][% 6.1f]\n", pov(-100,  0),   pov(-100,  5),   pov(-100,  50),  pov(-100,  100), pov(-50,   100), pov(-5,    100));

  
  //for(short a= 0; a< 4; a++) {
   // short b= a* 90;
    //printf("Q%d % 4d[% 3d,% 3d] % 4d[% 3d,% 3d] % 4d[% 3d,% 3d] % 4d[% 3d,% 3d]\n", a, b+ 0, getX(b+ 0), getY(b+ 0), b+ 10, getX(b+ 10), getY(b+ 10), b+ 45, getX(b+ 45), getY(b+ 45), b+ 80, getX(b+ 80), getY(b+ 80));
  //}
  //getchar();
  //return 0;
  



  // some string + time tests
  //uint64 t1, t2;
  //string s;

  //osi.getNanosecs(&t1);

  //uint b= 0;
  //s= "繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって繋がって or つながって";
  //for(uint a= 0; a< 10000000; a++) {
    //s+= "繋がって or つながって";
    //string s1= s[4];
    //uint c= s.getChar(b++);
    //if(b>100) b= 0;
    //printf("%d", b);
  //}

  //osi.getNanosecs(&t2);

  //printf("elapsed time= %lu\n", t2- t1);
  //printf("sizeof(bool)= %d\n", sizeof(bool));
  //printf("t1= %lu, t2= %lu", t1, t2);
  //getchar();
  // <end> string + time tests



  error.useWindowsFlag= true;
  //error.window("test");
  
  osi.display.populate(&osi);     // check all monitors/ resolutions/ etc

  osi.createGLWindow(&osi.win[0], &osi.display.monitor[1], "window 0", 1024, 768, 32, 1);
//  osi.createGLWindow(&osi.win[1], &osi.display.monitor[0], "window 1", 1024, 768, 32, 1);
  

//  osi.setProgramIcon("icon_64.ico");
  //osi.createGLWindow(&osi.win[0], &osi.display.monitor[0], "window 0", 1024, 768, 32, 2);
  //osi.createGLWindow(&osi.win[1], &osi.display.monitor[0], "window 2", 400, 400, 32, 3);
  //osi.createGLWindow(&osi.win[0], &osi.display.monitor[1], "window 0", 400, 400, 32, 4);

  //osi.createGLWindow(&osi.win[2], &osi.display.monitor[1], "window 3", 400, 400, 32, 1);

  in.init(1, 1);	// mode 2= manual update() on each frame
  
  // !!!!!
  osi.flags.haveFocus= true;
  
  
  
 // while(!osi.flags.haveFocus)
 //   osi.checkMSG();               // wait for window creation

  #ifdef USING_DIRECTINPUT
  //in.gp[8].aquire();
  //in.vibrate();
  #endif /// USING_DIRECTINPUT


  #ifdef OS_LINUX
  //glViewport(0, 0, osi.lin.gwa[0].width, osi.lin.gwa[0].height);
  #endif

  while(1) {

    osi.checkMSG();		/// operating system messages handling
    in.update();

    if(osi.flags.haveFocus)
      for(short a= 0; a< MAX_WINDOWS; a++)
        if(osi.win[a].isCreated) {
          if(osi.glMakeCurrent(&osi.win[a])) {
            osi.resizeGLScene(osi.win[a].dx, osi.win[a].dy);
            drawSomething();
            osi.swapBuffers(&osi.win[a]);
          }
    }
    
    if(in.k.key[in.Kv.esc] || (in.k.key[in.Kv.lalt] && in.k.key[in.Kv.f4]))
      osi.exit(0);
    if(osi.flags.exit)
      osi.exit(0);

    short n= 0;
    in.gp[n].activate();

    bool showPanel= false; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if(showPanel) {  
    #ifdef OS_WIN
    COORD pos= {0,0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    #else /// OS_LINUX & OS_MAC
    printf("\x1b[H");      // should set cursor position to '0,0' or whatever home means
    #endif /// OS_LINUX & MAC
    
    
    printf("mouse: %05dx %05dy %dl %dr %dm %dx1 %dx2 % dw %d %d %d %d \n", in.m.x, in.m.y, in.m.b[0].down, in.m.b[1].down, in.m.b[2].down, in.m.b[3].down, in.m.b[4].down, in.m.getWheelDu(), in.m.b[5].down, in.m.b[6].down, in.m.b[7].down, in.m.b[8].down);
    printf("last keyboard keys: %03d %03d %03d\n", in.k.lastKey[0].code, in.k.lastKey[1].code, in.k.lastKey[2].code);
    printf("nr: joysticks(%d) gamepads(%d) gamewheels(%d)\n", in.nr.jFound, in.nr.gpFound, in.nr.gwFound);

    printf("gamepad[%d] s1[% 6ld,% 6ld] s2[% 6ld,% 6ld] extra[% 6ld,% 6ld]\n", n, in.gp[n].lx, in.gp[n].ly, in.gp[n].rx, in. gp[n].ry, in.gp[n].u, in.gp[n].v);
    printf("gamepad[%d] lTrigger[% 6ld] rTrigger[% 6ld]\n", n, in.gp[n].lt, in.gp[n].rt);
    printf("gamepad[%d] b01[%d] b02[%d] b03[%d] b04[%d] b05[%d] b06[%d] b07[%d] b08[%d]\n", n, in.gp[n].b[0], in.gp[n].b[1], in.gp[n].b[2], in.gp[n].b[3], in.gp[n].b[4], in.gp[n].b[5], in.gp[n].b[6], in.gp[n].b[7]);
    printf("gamepad[%d] b09[%d] b10[%d] b11[%d] b12[%d] b13[%d] b14[%d] b15[%d] b16[%d]\n", n, in.gp[n].b[8], in.gp[n].b[9], in.gp[n].b[10], in.gp[n].b[11], in.gp[n].b[12], in.gp[n].b[13], in.gp[n].b[14], in.gp[n].b[15]);
    printf("gamepad[%d] pov[%05ld]\n", n, in.gp[n].pov);

    printf("typed char buffer[%d]:", in.k.charTyped.nrNodes);
    for(short a= 0; a< 32; a++) {
      ulong c= (in.k.charTyped.nrNodes> a)? ((Keyboard::chTyped*)in.k.charTyped.get(a))->c: L' ';
      
      printf("%lc", (wchar_t)c);
      printf("%lu", c);
      //printf("%lc", c);
      //printf("% c", (in.k.charTyped.nrNodes> a)? ((Keyboard::chTyped*)in.k.charTyped.get(a))->c: ' ');
    }
    printf("\ncaps[%d] num[%d] scroll[%d]\n", in.k.capsLock, in.k.numLock, in.k.scrollLock);
    
    //    printf("ZE QUESTION: [%d] [%d]\n", osi.display.monitor[0].glRenderer, osi.display.monitor[1].glRenderer);
    
    
    #ifdef OS_LINUX
    
    //for(short a= 0; a< 256; a++) {
//      if(in.k.key[a])
        //printf("keydown: keycode[%02d], keysym[%06lx]\n", a, XkbKeycodeToKeysym(osi.primWin->dis, a, null, 0));
    //}
    

    #endif /// OS_LINUX
    }

    
  }

  return 0;
}
*/

void drawSomething() {
  glDrawBuffer( GL_BACK );
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glBegin(GL_QUADS);
    glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
    glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
    glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
    glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
  glEnd();

}
// ####################
// End testing purposes
// ####################






osinteraction::osinteraction() {
  flags.exit= false;
  flags.haveFocus= false;
  flags.minimized= false;
  flags.buttonPress= false;
  flags.keyPress= false;
  
  primWin= &win[0];                          /// primWin pointer, always to &win[0]
  glr= null;

  #ifdef OS_WIN
  QueryPerformanceFrequency(&timerFreq);     /// read cpu frequency. used for high performance timer (querryPerformanceBlaBla)
  /// 'path' string - program path
  char *buf= new char[512];
  GetCurrentDirectory(511, buf);
  path= buf;
  delete[] buf;
  #endif /// OS_WIN
  
  #ifdef OS_LINUX
/// printf won't work without setlocale. but hopefully it won't be needed.
//    setlocale(LC_ALL, ""); // can't rely on setlocale. everything is different on each os. rely on StringClass32/8 and that's that.
//  setlocale(LC_CTYPE, "");
  primWin->dis= XOpenDisplay(null);          // DISPLAY CONNECTION TO XSERVER can be something like ":0.0" :displayNr.screenNr
  if(primWin->dis == NULL)
    error.simple("Cannot connect to X server\n", true); /// true= exit
  /// 'path' string - program path
  char *buf= new char[512];
  getcwd(buf, 511);
  path= buf;
  delete[] buf;
  #endif /// OS_LINUX

  #ifdef OS_MAC
  mach_timebase_info(&machInfo);            /// read cpu frequency & stuff; used for high performance timer (mac variant, named mach)
  cocoa.setProgramPath();                   /// program path (osi.path stores the string afterwards)
  #endif /// OS_MAC

  getNanosecs(&present);                    /// start with updated present time variable
}

osinteraction::~osinteraction() {
  #ifdef OS_LINUX
// it seems system already destroys the display/windows and calling any of these causes segmentation error
// maybe put them in a program exit function that might be called by the program.
//
  // XSync(primWin->dis, False);
  // delData();
  // XCloseDisplay(primWin->dis);             // DISPLAY CONNECTION TERMINATION
  #endif
}

void osinteraction::delData() {
  /// destroy every window. kinda useless...
  for(short a= 0; a< MAX_WINDOWS; a++)
    if(win[a].isCreated) win[a].delData();
}


// -------------============== WINDOW CREATION ================----------------

// SIMPLE WINDOW CREATION FUNCS. they all call createGLWindow()

// create just a single 'primary' window on 'primary' monitor
bool osinteraction::primaryGLWindow(string name, int dx, int dy, int8 bpp, int8 mode, short freq) {
  return createGLWindow(primWin, display.primary, name, dx, dy, bpp, mode, freq);
}

// create a fullscreen (mode 3) primary window
bool osinteraction::primaryGLWindow() {
  win[0].mode= 3;
  win[0].name= "Primary Program Window";
  win[0].freq= 0;
  win[0].bpp= 32;

  return createGLWindow(&win[0], display.primary, win[0].name, win[0].dx, win[0].dy, win[0].bpp, win[0].mode, win[0].freq);
}


// MAIN CREATE WINDOW FUNC. has every customisation
bool osinteraction::createGLWindow(osiWindow *w, osiMonitor *m, string name, int dx, int dy, int8 bpp, int8 mode, short freq, bool dblBuffer) {

  string func= "osinteraction::createGLWindow: ";
  w->name= name;
  w->monitor= m;

  /// window position
  if(mode== 1) {                                    /// windowed position
    int tx= m->x0+ (m->original.dx/ 2)- (dx/ 2);
    int ty= m->y0+ (m->original.dy/ 2)- (dy/ 2);
    w->x0= (tx< m->x0)? m->x0: tx;
    w->y0= (ty< m->y0)? m->y0: ty;
  } else if((mode == 2) || (mode == 3)) {           /// fullscreen/ fullscreen window position
    w->x0= m->x0;
    w->y0= m->y0;
  } else if (mode== 4) {                            /// full virtual screen
    w->x0= display.vx0;
    w->y0= display.vy0;
  }
  /// fullscreen window size set to monitor resolution size - else use them passed vars
  if(mode == 3) {                                   /// fullscreen window
    w->dx= m->original.dx;
    w->dy= m->original.dy;
  } else if(mode == 4) {                            /// full virtual screen
    w->dx= display.vdx;
    w->dy= display.vdy;
  } else {
    w->dx= (dx> 1)? dx: 1;
    w->dy= (dy> 1)? dy: 1;
  }
  w->bpp= bpp;
  w->mode= mode;
  w->freq= freq;

  #ifdef OS_WIN

  GLuint PixelFormat;   // Holds The Results After Searching For A Match
  WNDCLASS wc;          // Windows Class Structure
  DWORD dwExStyle;      // Window Extended Style
  DWORD dwStyle;        // Window Style
  RECT rect;            // Grabs Rectangle Upper Left / Lower Right Values

  /// fullscreen resolution change
  if(mode== 2) {
    if(!display.changeRes(m, w->dx, w->dy, w->bpp, w->freq)) {
      mode= 1;                            // if it fails, set mode to windowed <<--- ???
      w->mode= 1;
    }
    w->x0= m->x0;
    w->y0= m->y0;
  }

  rect.left=	 w->x0;
  rect.right=	 w->x0+ w->dx;
  rect.top=		 display.vdy- display.vy0- (w->y0+ w->dy);  /// coordonate unification changed
  rect.bottom= display.vdy- display.vy0- w->y0;           /// coordonate unification changed

  w->hInstance = GetModuleHandle(NULL);                   /// grab an instance for window
  wc.style				 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  /// Redraw On Size, And Own DC For Window.
  wc.lpfnWndProc	 = (WNDPROC) processMSG;                // processMSG handles messages
  wc.cbClsExtra		 = 0;                                   /// no extra
  wc.cbWndExtra		 = 0;                                   /// no extra
  wc.hInstance		 = w->hInstance;                        /// set the aquired instance
  wc.hIcon				 = LoadIcon(NULL, IDI_WINLOGO);         // load default icon <<<<<<<<<<<<<<<<<<<<<< ICON WORKS MUST BE MADE
  wc.hCursor			 = LoadCursor(NULL, IDC_ARROW);         /// load arrow pointer
  wc.hbrBackground = NULL;                                /// no backgraound required when using opengl
  wc.lpszMenuName	 = NULL;                                /// no menus
  wc.lpszClassName = name;                                /// class name... dunno for shure what this is

  if (!RegisterClass(&wc)) {                        /// register the window class
    error.simple(func+ "Failed to register wc");
    return false;
  }

  if(mode== 1) {                                    // windowed
    if(w == &win[0]) {                              /// if it's win[0], it's the primary window - if not it's a child window
      dwExStyle= WS_EX_APPWINDOW| WS_EX_WINDOWEDGE;
      dwStyle= WS_OVERLAPPEDWINDOW;
    } else {
      dwExStyle= WS_EX_TOOLWINDOW| WS_EX_WINDOWEDGE;
      dwStyle= WS_OVERLAPPEDWINDOW;                 /// WS_CHILD is out of the equation (these are windows in windows)
    }
  } else if(mode== 2) {                             // fullscreen
    if(w == &win[0]) {
      dwExStyle= WS_EX_APPWINDOW;
      dwStyle= WS_POPUP;
      ShowCursor(FALSE);
    } else {
      dwExStyle= 0;
      dwStyle= WS_POPUP;//WS_CHILD;//WS_OVERLAPPED;// WS_POPUP;//| WS_CHILD;  // <<< wschild???
      ShowCursor(FALSE);
    }
  } else if((mode== 3) || (mode== 4)) {             // fullscreen window
    if(w == &win[0]) {
      dwExStyle= WS_EX_APPWINDOW;
      dwStyle= WS_POPUP;
    }	else {
      dwExStyle= 0;
      dwStyle= WS_POPUP;//| WS_CHILD; // <<< wschild???
    }
  }

  AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size

  // Create The Window
  if (!(w->hWnd= CreateWindowEx(dwExStyle,              // Extended Style For The Window
                name,                   // class name           <--- might want a different class name?
                name,                   /// window title
                dwStyle |               /// defined window style
                WS_CLIPSIBLINGS |       /// Required Window Style ?? not shure
                WS_CLIPCHILDREN,        /// Required Window Style ?? not shure
                w->x0, (display.vdy- display.vy0)- (w->y0+ w->dy), /// window position (coord unification fixed)
                rect.right- rect.left,  /// dx
                rect.bottom- rect.top,  /// dy
                win[0].hWnd,            /// parent window
                NULL,                   /// no menu
                w->hInstance,           /// instance
                NULL)))                 /// don't pass anything to WM_CREATE
  {
    killGLWindow(w);                    // Reset The Display
    error.simple(func+ "Window creation error.");
    return false;
  }
  /// pixel format - MORE TESTING NEEDED HERE. screen blacks out on mode 3 - it shouldn't
  static PIXELFORMATDESCRIPTOR pfd;
  /*
  pfd.nSize= sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion= 1;
  pfd.dwFlags= PFD_DRAW_TO_WINDOW| PFD_DRAW_TO_BITMAP| PFD_SUPPORT_OPENGL| PFD_DOUBLEBUFFER| PFD_STEREO_DONTCARE; //| PFD_SWAP_EXCHANGE;//| PFD_NEED_PALETTE;
  pfd.iPixelType= PFD_TYPE_RGBA;
  pfd.cColorBits= w->bpp;
  pfd.cRedBits= 8;
  pfd.cRedShift= 0;
  pfd.cGreenBits= 8;
  pfd.cGreenShift= 0;
  pfd.cBlueBits= 8;
  pfd.cBlueShift= 0;
  pfd.cAlphaBits= 0;
  pfd.cAlphaShift= 0;
  pfd.cAccumBits= 32;
  pfd.cAccumRedBits= 8;
  pfd.cAccumGreenBits= 8;
  pfd.cAccumBlueBits= 8;
  pfd.cAccumAlphaBits= 8;
  pfd.cDepthBits= 32;
  pfd.cStencilBits= 8;
  pfd.cAuxBuffers= 0;
  pfd.iLayerType= PFD_MAIN_PLANE;
  pfd.bReserved= 0;
  pfd.dwLayerMask= 0;
  pfd.dwVisibleMask= 0;
  pfd.dwDamageMask= 0;
  */
  if (!(w->hDC= GetDC(w->hWnd))) {                  /// get a device context
    killGLWindow(w);
    error.simple(func+ "Can't create a GL DC");
    return false;
  }
  /// get the current pixel format index  
  int pixelf= GetPixelFormat(w->hDC); 
  if(chatty) printf("pixelf= %d\n", pixelf);
  /// obtain a detailed description of that pixel format  
  DescribePixelFormat(w->hDC, pixelf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  // double buffer is needed, but causes screen flicker. must check with the other type of wgl_choosePixelFormat()
  pfd.dwFlags= pfd.dwFlags| PFD_DRAW_TO_WINDOW| PFD_DRAW_TO_BITMAP| PFD_SUPPORT_OPENGL| PFD_STEREO_DONTCARE; //| PFD_SWAP_EXCHANGE;//| PFD_NEED_PALETTE;
  if(dblBuffer)
    pfd.dwFlags|= PFD_DOUBLEBUFFER;

  if(chatty) {
    printf("Current pixel format descriptor:\n");
    printf("pfd.nVersion= %d\n", pfd.nVersion);
    //pfd.dwFlags= PFD_DRAW_TO_WINDOW| PFD_DRAW_TO_BITMAP| PFD_SUPPORT_OPENGL| PFD_DOUBLEBUFFER| PFD_STEREO_DONTCARE; //| PFD_SWAP_EXCHANGE;//| PFD_NEED_PALETTE;
    printf("pfd.iPixelType= %d\n", pfd.iPixelType);
    printf("pfd.cColorBits= %d\n", pfd.cColorBits);
    printf("pfd.cRedBits= %d\n", pfd.cRedBits);
    printf("pfd.cRedShift= %d\n", pfd.cRedShift);
    printf("pfd.cGreenBits= %d\n", pfd.cGreenBits);
    printf("pfd.cGreenShift= %d\n", pfd.cGreenShift);
    printf("pfd.cBlueBits= %d\n", pfd.cBlueBits);
    printf("pfd.cBlueShift= %d\n", pfd.cBlueShift);
    printf("pfd.cAlphaBits= %d\n", pfd.cAlphaBits);
    printf("pfd.cAlphaShift= %d\n", pfd.cAlphaShift);
    printf("pfd.cAccumBits= %d\n", pfd.cAccumBits);
    printf("pfd.cAccumRedBits= %d\n", pfd.cAccumRedBits);
    printf("pfd.cAccumGreenBits= %d\n", pfd.cAccumGreenBits);
    printf("pfd.cAccumBlueBits= %d\n", pfd.cAccumBlueBits);
    printf("pfd.cAccumAlphaBits= %d\n", pfd.cAccumAlphaBits);
    printf("pfd.cDepthBits= %d\n", pfd.cDepthBits);
    printf("pfd.cStencilBits= %d\n", pfd.cStencilBits);
    printf("pfd.cAuxBuffers= %d\n", pfd.cAuxBuffers);
    printf("pfd.iLayerType= %d\n", pfd.iLayerType);
    printf("pfd.bReserved= %d\n", pfd.bReserved);
    printf("pfd.dwLayerMask= %d\n", pfd.dwLayerMask);
    printf("pfd.dwVisibleMask= %d\n", pfd.dwVisibleMask);
    printf("pfd.dwDamageMask= %d\n", pfd.dwDamageMask);
  }


  // MORE TESTS NEEDED. it seems, when everything is 0, some 'default' current mode is in use; can't know for shure until using a more complex opengl scene


   // NEED TO USE THIS INSTEAD, i think
  /*
  BOOL wglChoosePixelFormatARB(   HDC hdc,
                                const int *piAttribIList,
                                const FLOAT *pfAttribFList,
                                UINT nMaxFormats,
                                int *piFormats,
                                UINT *nNumFormats);
  */


  if (!(PixelFormat= ChoosePixelFormat(w->hDC, &pfd))) {  /// lots of checks, don't think any needed
    killGLWindow(w);
    error.simple(func+ "Can't aquire a PixelFormat");
    return false;
  }

  if(!SetPixelFormat(w->hDC, PixelFormat, &pfd)) {        /// lots of checks, don't think any needed
    killGLWindow(w);
    error.simple(func+ "Can't set PixelFormat");
    return false;
  }

  if(!assignRenderer(w)) {
    osi.killGLWindow(w);
    error.simple("FATAL ERROR: Cannot create oGL renderer (context)");
    return false;
  }
  
  // from help: wglMakeCurrent()
  //	All subsequent OpenGL calls made by the thread are drawn on the device identified by hdc.
  //	You can also use wglMakeCurrent to change the calling thread's current rendering context so it's no longer current.


  if(!glMakeCurrent(w)) {
    killGLWindow(w);
    error.simple(func+ "Can't activate GL RC");
    return false;
  }

  ShowWindow(w->hWnd, SW_SHOW);   /// Show The Window
  SetForegroundWindow(w->hWnd);   /// Slightly Higher Priority

  SetFocus(w->hWnd);              /// Sets Keyboard Focus To The Window

  w->isCreated= true;
  m->win= w;

  w->glr->checkExt();
  w->glr->getExtFuncs();        /// once a window is created, getExtensions() aquires oGL extensions functions

  return true;
  #endif /// OS_WIN


  #ifdef OS_LINUX //                <---------------------------LINUX
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;

  GLint att[]= {
                 GLX_RGBA,
                 GLX_RED_SIZE, 8,
                 GLX_GREEN_SIZE, 8,
                 GLX_BLUE_SIZE, 8,
                 GLX_ALPHA_SIZE, 8,
                 GLX_DEPTH_SIZE, 16,
                 GLX_DOUBLEBUFFER,
                 None };

  w->root= m->root;                                        // 'desktop window'
  w->dis= primWin->dis; // server connection, created in osinteraction()

  if(mode == 2)
    if(!display.changeRes(w, m, dx, dy, bpp, freq)) {
      error.simple("osi:createGLwindow: cant change to selected resolution");
      w->dx= m->dx;
      w->dy= m->dy;
      mode= 3;
    }

  w->mode= mode;

  vi= glXChooseVisual(w->dis, m->screen, att);

  /* !!!!!!!!!!!!!!!!!!
  vi= DefaultVisual(display, 0);
  depth= DefaultDepth(display, 0);
  */


  if(vi == NULL)
    error.simple(func+ "no appropriate visual found\n", true);
  else // DELETE <--------------------------------
    if(chatty) printf("\n\tvisual %p selected\n", (void *)vi->visualid); // %p creates hexadecimal output like in glxinfo

  cmap= XCreateColormap(w->dis, w->root, vi->visual, AllocNone);
  swa.colormap= cmap;


  // try only the parent to receive events!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //if(w== primWin)
    swa.event_mask= ExposureMask|

//                    ((w== primWin)? StructureNotifyMask: SubstructureNotifyMask)|
//                    SubstructureNotifyMask|   // send msgs only to parent
                    StructureNotifyMask|      // probly the default, a parent must have this I THINK

                    EnterWindowMask| LeaveWindowMask|     // mouse enters window &other stuff like this
                    FocusChangeMask|
                    KeyPressMask| KeyReleaseMask| ButtonPressMask| ButtonReleaseMask|
                    PointerMotionMask;
  //else
    //swa.event_mask= ExposureMask;

  swa.override_redirect= 0;                             // this is very hard to handle if true

  w->win= XCreateWindow(w->dis, w->root,
                        w->x0, display.vdy- display.vy0- (w->y0+ w->dy), w->dx, w->dy,     // position & size (coord unification fixed)
                        0,                              // border size
                        vi->depth,                      // depth can be CopyFromParent
                        InputOutput,                    // InputOnly/ InputOutput/ CopyFromParent
                        vi->visual,                     // can be CopyFromParent
                        CWColormap| CWEventMask| CWOverrideRedirect,       // tied with &swa
                        &swa);                          //




  // XInput EXPERIMANTAL
  /*
  XIEventMask eventmask;
  unsigned char mask[1] = { 0 }; // the actual mask

  eventmask.deviceid = 2;
  eventmask.mask_len = sizeof(mask); // always in bytes
  eventmask.mask = mask;

  // now set the mask
  XISetMask(mask, XI_ButtonPress);
  XISetMask(mask, XI_Motion);
  //XISetMask(mask, XI_KeyPress);
  //XISetMask(mask, XI_);

  // select on the window
  XISelectEvents(w->dis, w->win, &eventmask, 1);
  */
// XInput EXPERIMANTAL ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



// tring to see what this group hint is

  XWMHints *wh= XAllocWMHints();
  wh->window_group= 1;
  wh->flags= WindowGroupHint;
  XSetWMHints(primWin->dis, w->win, wh);
  XFree(wh);


// ^^^^^^^^ after testing, still dunno what the group does... auto it does nothing

  // _NET_WM_STATE with_NET_WM_STATE_ABOVE gain focus
  // _NET_WM_STATE with_NET_WM_STATE_BELOW lose focus

  if(w== primWin) {
    // ICON

  } else {
    /// rest of windows have no taskbar icons
    //XSetTransientForHint(primWin->dis, w->win, primWin->win); // it seems transientFor is the only thing needed (no taskbar icon)
    w->setWMstate(1, "_NET_WM_STATE_SKIP_TASKBAR");

    // ***NO ICON

    // ***NO TASKBAR
    // option 1:  _NET_WM_WINDOW_TYPE
    //   with  _NET_WM_WINDOW_TYPE_TOOLBAR or _NET_WM_WINDOW_TYPE_UTILITY or _NET_WM_WINDOW_TYPE_DIALOG
    // option 2:  _NET_WM_STATE
    //   with  _NET_WM_STATE_SKIP_TASKBAR

  }
  
  /// fullscreen mode linux specific msgs settings
  //XMoveWindow(w->dis, w->win, w->x0, w->y0);
  if((mode== 2) || (mode== 3) || (mode== 4))
    w->setWMstate(1, "_NET_WM_STATE_FULLSCREEN");
  
  /// Fullscreen window on all monitors - MODE 4. Needs XInerama only for monitor IDs (wich sucks, as Xrandr should handle this little aspect)
  if(mode== 4) {
    Atom fullmons = XInternAtom(w->dis, "_NET_WM_FULLSCREEN_MONITORS", False);
    XEvent xev;
    for(short a= 0; a< sizeof(xev); a++) ((char*)&xev)[a]= 0;
    xev.type = ClientMessage;
    xev.xclient.window= w->win;
    xev.xclient.message_type = fullmons;
    xev.xclient.format = 32;
    
    /// the next values are found in OSdisplay.cpp, updateVirtualDesktop() (back of file)
    /// Xinerama IDs are found in display.populate(), linux part, at the end
    xev.xclient.data.l[0]= display.top;     /// topmost monitor number (Xinerama monitor ID)
    xev.xclient.data.l[1]= display.bottom;  /// bottommost
    xev.xclient.data.l[2]= display.left;    /// leftmost
    xev.xclient.data.l[3]= display.right;   /// rightmost
    printf("top[%d] left[%d] bottom[%d] right[%d]\n", display.top, display.left, display.bottom, display.right);
    xev.xclient.data.l[4]= 1;               /// source indication (1 for normal applications, 2 for pagers and other Clients that represent direct user actions)
    
    XSendEvent (w->dis, DefaultRootWindow(w->dis), False,
                    SubstructureRedirectMask | SubstructureNotifyMask, &xev);
  }
  
  if(w->mode!= 1)                           /// in all other modes but 1, make window 'on top'
    w->setWMstate(1, "_NET_WM_STATE_ABOVE");
  
  XMapWindow(w->dis, w->win);               /// map window= finish creation/ show window
  
  if(w->mode== 1)                           /// MODE1 seems to need to move the window, x0&y0 in createwindow do nothing
    XMoveWindow(w->dis, w->win, w->x0, display.vdy- display.vy0- (w->y0+ w->dy));

  XStoreName(w->dis, w->win, name);         /// window name (top bar description/name)


  // WIP <<<<<<<<<<<<< THIS IS NOT DONE. 'RENDERERS MUST BE HANDLED'
  // NOTE: this func has shareLists in its params !!!!!!!!!!!!!!!!!!!!!
  if(win== primWin)
    w->glRenderer= glXCreateContext(w->dis, vi, NULL, GL_TRUE);
  else
    w->glRenderer= win[0].glRenderer;
  // SAME RENDERER!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATM

/* The last parameter decides if direct rendering is enabled. If you want
   to send the graphical output via network, you have to set it to GL_FALSE.
   If your application puts its output to the computer you are sitting in
   front of, use GL_TRUE. Note that some capabilities like vertex buffer objects
   can only be used with a direct gl context (GL_TRUE). */

  
  glMakeCurrent(w);                         // osi variant. works on every OS

  
  glEnable(GL_DEPTH_TEST);

  ///  handle the close button WM
  if(w== primWin) {
    Atom wmDelete= XInternAtom(w->dis, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(w->dis, w->win, &wmDelete, 1);
  }

  w->monitor= m;
  w->isCreated= true;
  m->win= w;

  w->glr->getExtensions();        /// once a window is created, getExtensions() aquires oGL extensions functions

  return true;

  #endif /// OS_LINUX

  #ifdef OS_MAC // <<<<<<<<<<<<<< MAC >>>>>>>>>>>>>>>
  
  /// window creation is in OScocoa.mm due to Abjective-C
  
  return cocoa.createWindow(w);  /// all window vars are set, just create the window.
  #endif /// OS_MAC
  
/// if program reached this point, there's no OS defined
  error.simple(func+ "no OS specified?");
  return false;
} // osinteraction::createGLWindow END <<<







// ----------------============= GLWINDOW DELETION ==============-------------
bool osinteraction::killPrimaryGLWindow() {
  return killGLWindow(&win[0]);
}


bool osinteraction::killGLWindow(osiWindow *w) {
  
  if(w->mode== 2)
    display.restoreRes(w->monitor);

  if(w->monitor)
    w->monitor->win= null;

  w->delData();

  return true;
  /*
  #ifdef OS_WIN
  /*
  if(w->monitor)
    w->monitor->win= null;

  if (w->mode== 2)
    display.restorePrimary();

  w->delData();
  
  return true;
  
  #endif /// OS_WIN

  #ifdef OS_LINUX
  w->delData();
  return true;
  #endif /// OS_LINUX

  #ifdef OS_MAC
  w->delData();
  return true;
  #endif

  return false;
  */
} /// osinteraction::killGLWindow


void osinteraction::setProgramIcon(string file) {
  #ifdef OS_WIN
  HANDLE hIcon= LoadImage(NULL, file, IMAGE_ICON, 64, 64, LR_LOADFROMFILE);

  SendMessage(primWin->hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

  // small icon???
  //hIconSm = LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
  //SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
  #endif

}

void osinteraction::startThread(void func(void *)) {
  #ifdef OS_WIN
  // CreateThread(..) option 1 - thing is, _beginthread is more advanced than these funcs, i think.
  // >> windows, it seems wants these funcs gone, and replaced in windows 8 with some 'windows store' compatible object which is crap <<
  // >> so support for xp / win 7 will go away? will see in time, what the heck they want <<

  // msdn:
  // Like the Win32 ExitThread API, _endthreadex does not close the thread handle. Therefore, when you use _beginthreadex and _endthreadex,
  // you must close the thread handle by calling the Win32 CloseHandle API.
  _beginthread(func, 0, null);
  #endif /// OS_WIN

  #ifdef OS_LINUX
  makeme
  #endif /// OS_LINUX

  #ifdef OS_MAC
  mekeme
  #endif /// OS_MAC
}










#ifdef OS_WIN
string osinteraction::getWinName(HWND h) {
  for(int a= 0; a< MAX_WINDOWS; a++)
    if(win[a].hWnd== h)
      return win[a].name;
  return "unknown window";
}

osiWindow *osinteraction::getWin(HWND h) {
  for(int a= 0; a< MAX_WINDOWS; a++)
    if(win[a].hWnd== h)
      return &win[a];
  return null;
}
#endif /// OS_WIN

#ifdef OS_LINUX
osiWindow *osinteraction::getWin(Window *w) {
  for(short a= 0; a< MAX_WINDOWS; a++)
    if(win[a].win== *w)
      return &win[a];
  return null;
}
#endif

#ifdef OS_MAC
osiWindow *osinteraction::getWin(void *w) {
  for(short a= 0; a< MAX_WINDOWS; a++)
    if(win[a].win== w)
      return &win[a];
  return null;
}
#endif /// OS_MAC



// system EVENTS / MESSGES handler funcs
// -------------------------------------
#ifdef OS_WIN
LRESULT CALLBACK processMSG(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam) {

  ///===================================================
  // return 0 is faster than defWindowProc(what goto ret does)
  // it would work good for fullscreen/windowed fullscreen, but in windowed mode
  // there is no resize/move for windows, the close button won't work either, i think
  ///===================================================

  bool onlyHandled= true; /// used with chatty
  bool timeFunc= false;   /// measure the time this func takes to finish
  uint64 start, end;      /// used with timeFunc
  osiWindow *w;           /// window that received the message
//  WINDOWPOS *tw;          /// used for window position change messages

  if(timeFunc) osi.getNanosecs(&start);

  int mb= 0;

  // mouse in [MODE 2], THE WHEEL IS NOT POSSIBLE TO READ with funcs, so events are the only way (decent way, anyways)
  if((in.m.mode== 2)&& osi.flags.haveFocus)
    if(m== WM_MOUSEWHEEL) {
      in.m.wheel+= (GET_WHEEL_DELTA_WPARAM(wParam)> 0)? 1: -1;
      if(chatty) printf("mouse: wheel rotated\n");
      goto ret;
    }

  // mouse vals are handled here, in case mouse is set in [MODE 1]
  if((in.m.mode== 1)&& osi.flags.haveFocus)
    switch(m) {
      case WM_MOUSEMOVE:                                          /// mouse movement
        /// removed oldx&y, dx&y, they are updated when in.update() is called; deltas are always on, now.
        /// these are inside window positions
        w= osi.getWin(hWnd);
        in.m.x= ((int)(short)LOWORD(lParam));   /// msdn says not to use loword; this is what GET_X_PARAM does
        in.m.y= ((int)(short)HIWORD(lParam));
        /// coordonate unification
        in.m.y= w->y0+ w->dy- in.m.y;
        in.m.x= w->x0+ in.m.x;
        
        goto ret;
        //return 0; // it is faster, but no windows move/resize!!!
      case WM_SETCURSOR: 
        goto ret;                             /// to be or not to be

      // mouse buttons in MODE 1
      case WM_LBUTTONDOWN:                                        /// left mouse button
        osi.getMillisecs(&in.m.b[0].timeStart);
        in.m.b[0].down= true;
        if(chatty) printf("mouse: l button pressed\n");
        goto ret;
        //return 0;
      case WM_LBUTTONUP:
        in.m.b[0].lastTimeStart= in.m.b[0].timeStart;
        osi.getMillisecs(&in.m.b[0].lastTimeEnded);
        in.m.b[0].lastDT= in.m.b[0].lastTimeEnded- in.m.b[0].lastTimeStart;
        in.m.b[0].down= false;
        if(chatty) printf("mouse: l button released\n");
        goto ret;
        //return 0;
      case WM_RBUTTONDOWN:                                        /// right mouse button
        osi.getMillisecs(&in.m.b[1].timeStart);
        in.m.b[1].down= true;
        if(chatty) printf("mouse: r button pressed\n");
        goto ret;
        //return 0;
      case WM_RBUTTONUP:
        in.m.b[1].lastTimeStart= in.m.b[1].timeStart;
        osi.getMillisecs(&in.m.b[1].lastTimeEnded);
        in.m.b[1].lastDT= in.m.b[1].lastTimeEnded- in.m.b[1].lastTimeStart;
        in.m.b[1].down= false;
        if(chatty) printf("mouse: r button released\n");
        goto ret;
        //return 0;
      case WM_MBUTTONDOWN:                                        /// middle mouse button
        osi.getMillisecs(&in.m.b[2].timeStart);
        in.m.b[2].down= true;
        if(chatty) printf("mouse: m button pressed\n");
        goto ret;
        //return 0;
      case WM_MBUTTONUP:
        in.m.b[2].lastTimeStart= in.m.b[2].timeStart;
        osi.getMillisecs(&in.m.b[2].lastTimeEnded);
        in.m.b[2].lastDT= in.m.b[2].lastTimeEnded- in.m.b[2].lastTimeStart;
        in.m.b[2].down= false;
        if(chatty) printf("mouse: m button released\n");
        goto ret;
        //return 0;
      case WM_MOUSEWHEEL:                                         /// wheel
        in.m.twheel+= (GET_WHEEL_DELTA_WPARAM(wParam)> 0)? 1: -1;
        if(chatty) printf("mouse: wheel rotated\n");
        goto ret;
        //return 0;
      case WM_XBUTTONDOWN:                                        /// buttons 4 & 5
        if(GET_XBUTTON_WPARAM(wParam)== XBUTTON1) {
          osi.getMillisecs(&in.m.b[3].timeStart);
          in.m.b[3].down= true;
          if(chatty) printf("mouse: button 4 pressed\n");
          goto ret;
          //return 0;
        }
        if(GET_XBUTTON_WPARAM(wParam)== XBUTTON2) {
          osi.getMillisecs(&in.m.b[4].timeStart);
          in.m.b[4].down= true;
          if(chatty) printf("mouse: button 5 pressed\n");
          goto ret;
          //return 0;
        }
        break;
      case WM_XBUTTONUP:
        if(GET_XBUTTON_WPARAM(wParam)== XBUTTON1) {
          in.m.b[3].lastTimeStart= in.m.b[3].timeStart;
          in.m.b[3].lastTimeEnded= osi.eventTime;
          osi.getMillisecs(&in.m.b[3].lastTimeEnded);
          in.m.b[3].down= false;
          if(chatty) printf("mouse: button 4 released\n");
          goto ret;
          //return 0;
        }
        if(GET_XBUTTON_WPARAM(wParam)== XBUTTON2) {
          in.m.b[4].lastTimeStart= in.m.b[4].timeStart;
          osi.getMillisecs(&in.m.b[4].lastTimeEnded);
          in.m.b[4].lastDT= in.m.b[4].lastTimeEnded- in.m.b[4].lastTimeStart;
          in.m.b[4].down= false;
          if(chatty) printf("mouse: button 5 released\n");
          goto ret;
          //return 0;
        }
        break;
    };


  // keyboard messages [MODE 1]
  if(in.k.mode== 1) {
    switch(m) {
      case WM_KEYDOWN:                                          // ***key PRESS***
      case WM_SYSKEYDOWN: {
        osi.getMillisecs(&osi.eventTime);     /// using getTIMEXXX() funcs: can't rely on event time sent from system
        in.k.updateLocks();
        osi.flags.keyPress= true;

        int code= GETBYTE2UINT32(lParam);
        uint vcode= (uint)wParam;
        osiKeyboard::KeyPressed k;

        /// left/ right ALT/CONTROL/SHIFT distingush
        if(wParam== VK_SHIFT)   code= (GetKeyState(VK_RSHIFT)& 0x80)?   in.Kv.rshift: in.Kv.lshift;
        if(wParam== VK_CONTROL) code= (GetKeyState(VK_RCONTROL)& 0x80)? in.Kv.rctrl:  in.Kv.lctrl;
        if(wParam== VK_MENU)    code= (GetKeyState(VK_RMENU)& 0x80)?    in.Kv.ralt:   in.Kv.lalt;
        //if(wParam== VK_RETURN)  code= (GetKeyState(VK_RETURN)& 0x80)?   in.Kv.enter:  in.Kv.kpenter; // makes no difference (win's fault)

        /// check if message is a <repeat key press>
        if(KF_REPEAT& HIWORD(lParam)) {
          if(!in.k.key[code]) {               /// in case shit happened (alt tab mess, some crappy windows message block dunno)
            /// log the key
            k.code= code;
            k.checked= false;
            k.timeDown= osi.eventTime;
            k.timeUp= 0;
            k.timeDT= 0;
            in.k.log(k);
            /// set the vars
            in.k.key[code]= 128;
            in.k.keyTime[code]= osi.eventTime;
            //in.k.repeat[code]= 1;           /// with the next repeat increase (next line), the key will be logged as repeated twice, wich is ok
          }
          //in.k.repeat[code]+= KF_REPEAT& HIWORD(lParam);   // THIS MIGHT BE DELETED <-------------------------------------------

          in.k.doManip();                          /// check if current pressed keys form a manip char
          if(chatty) printf("key REPEAT code[0x%X] vcode[0x%X]\n", code, vcode);
          goto ret;
          //return 0;
        }
        /// log the key
        k.code= code;
        k.checked= false;
        k.timeDown= osi.eventTime;
        in.k.log(k);
        /// set the key as pressed & other needed vars
        in.k.key[code]= 128;
        in.k.keyTime[code]= osi.eventTime;
        //in.k.repeat[code]= 1;                  /// a new key press, sets repeat to 1  // MIGHT BE DELETED
        
        in.k.doManip();                          /// check if current pressed keys form a manip char
        if(chatty) printf("key PRESS code[0x%X] vcode[0x%X]\n", code, vcode);
        goto ret;
        //return 0;
      }
      case WM_KEYUP:                              // <<< key DEPRESS >>>
      case WM_SYSKEYUP: {
        in.k.updateLocks();
        osi.flags.keyPress= false;

        osi.getMillisecs(&osi.eventTime);        /// using getTIMEXXX() funcs: can't rely on event time sent from system

        int code= GETBYTE2UINT32(lParam);        /// key code
        uint vcode= (uint)wParam;                /// windows virtual key code
        if(wParam== VK_SHIFT)   code= in.k.key[in.Kv.rshift]? in.Kv.rshift: in.Kv.lshift;
        if(wParam== VK_CONTROL) code= in.k.key[in.Kv.rctrl]?  in.Kv.rctrl:  in.Kv.lctrl;
        if(wParam== VK_MENU)    code= in.k.key[in.Kv.ralt]?		in.Kv.ralt:   in.Kv.lalt;
        //if(wParam== VK_RETURN)  code= in.k.key[VK_RETURN]?    in.Kv.enter:  in.Kv.kpenter;       // makes no difference (win's fault)

        if(chatty) printf("key RELEASE code[0x%X] vcode[0x%X]\n", code, vcode);

        /// log the key in history
        bool found= false;
        for(short a= 0; a< MAX_KEYS_LOGGED; a++)
          if(in.k.lastKey[a].code== code) {
            if(in.k.lastKey[a].timeUp) continue;
            in.k.lastKey[a].timeUp= osi.eventTime;
            in.k.lastKey[a].timeDT= in.k.lastKey[a].timeUp- in.k.lastKey[a].timeDown;
            found= true;
            break;
          }
        /// in case the key was not found in history, add a hist-log with insta-keydown-keyup
        if(!found)  {
          osiKeyboard::KeyPressed k;
          k.code= code;
          k.checked= false;
          k.timeDown= osi.eventTime- 1;   /// 1 milisecond before the keyup
          k.timeUp= osi.eventTime;
          k.timeDT= 1;                    /// timeUp- timeDown
          in.k.log(k);
        }

        /// set the key as pressed & other vars
        in.k.key[code]= 0;
        in.k.keyTime[code]= 0;
        // in.k.repeat[code]= 0;  // repeat[] is subject for deletion

        goto ret;
        //return 0;
      }
    } /// switch
  } /// if keyboard is in mode 1

  if(in.k.mode== 2) {
    switch(m) {
      case WM_KEYDOWN:                    // key PRESS
      case WM_SYSKEYDOWN:
      case WM_KEYUP:                      // key PRESS
      case WM_SYSKEYUP:
        in.k.updateLocks();               /// to be or not to be...  this should be just empty code here...
        in.k.update();
        if(chatty) printf("key update");
        goto ret;
    } /// switch
  } /// if keyboard is in mode 2


  /// ignore some calls to defwindowproc for speed - PROBLEM IS, no window MOVE/RESIZE, probly no close button either

/*
  if((m>= WM_MOUSEFIRST)&& (m<= WM_MOUSELAST))
    return 0;
  if((m>= WM_KEYFIRST)&& (m<= WM_KEYLAST))
    return 0;
*/


  // handled windows messages
  switch(m) {
    case WM_ACTIVATEAPP:
      if(wParam== 1) {                     // ---=== application GAINS focus ===---
        /// if any HIDs are using exclusive mode, aquire it
        in.m.aquire();
        in.k.aquire();
        for(short a= 8; a< 20; a++)       /// direct input & xinput sticks
          if(in.j[a].active)
            in.j[a].aquire();

        /// change resolution in case of fullscreen
        for(short a= 0; a< MAX_WINDOWS; a++) 
          if(osi.win[a].isCreated)
            if(osi.win[a].mode== 2)
              osi.display.changeRes(osi.win[a].monitor, osi.win[a].dx, osi.win[a].dy, osi.win[a].bpp, osi.win[a].freq);

        /// show windows in case they are minimized
        for(short a= 0; a< MAX_WINDOWS; a++) 
          if(osi.win[a].isCreated)
            if(osi.win[a].mode== 2) {
              ShowWindow(osi.win[a].hWnd, SW_RESTORE);
              osi.flags.minimized= false;
              MoveWindow(osi.win[a].hWnd, osi.win[a].monitor->x0, osi.win[a].monitor->y0, osi.win[a].dx, osi.win[a].dy, false);
              if(chatty) printf("window %d x0[%d] y0[%d] dx[%d] dy[%d]\n", a, osi.win[a].monitor->x0, osi.win[a].monitor->y0, osi.win[a].dx, osi.win[a].dy);
            }
        SetForegroundWindow(osi.primWin->hWnd);
        osi.flags.haveFocus= true;        /// set flag, the last

      } else {                             // ---=== application LOSES focus ===---
        osi.flags.haveFocus= false;       /// set flag, first

        /// if any HIDs are using exclusive mode, unaquire it
        in.m.unaquire();
        in.k.unaquire();
        for(short a= 8; a< 20; a++)      /// direct input & xinput sticks unaquire
          if(in.j[a].active)
            in.j[a].unaquire();

        /// in case of alt-tab all current pressed buttons must be reset
        in.resetPressedButtons();

        /// minimize windows in case of fullscreen
        for(short a= 0; a< MAX_WINDOWS; a++) 
          if(osi.win[a].isCreated)
            if(osi.win[a].mode== 2) {
              if(&osi.win[a]== osi.primWin) /// main window gets minimized
                ShowWindow(osi.win[a].hWnd, SW_MINIMIZE);
              else                          /// all other windows must be hidden, else they get minimized into a small box
                ShowWindow(osi.win[a].hWnd, SW_HIDE);
              osi.flags.minimized= true;
            }

        /// change back original monitors resolutions in case of fullscreen
        for(short a= 0; a< MAX_WINDOWS; a++) 
          if(osi.win[a].isCreated)
            if(osi.win[a].mode== 2)
              osi.display.restoreRes(osi.win[a].monitor);



      } /// switch gain/lose focus

      if(chatty) printf("WM_ACTIVATEAPP %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam);
      goto ret;

    case WM_CLOSE:
      if(chatty) printf("WM_CLOSE %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam);
      osi.flags.exit= true;     /// main exit flag
      return 0;
    case WM_CHAR:
      if(chatty) printf("WM_CHAR %s %lc\n", osi.getWinName(hWnd).d, wParam);
      osi.getMillisecs(&osi.eventTime);
      in.k.addChar((ulong)wParam, &osi.eventTime);
      return 0;
    case WM_UNICHAR:
      error.console("WM_UNICHAR not tested");
      if(chatty) printf("WM_UNICHAR %s %lc\n", osi.getWinName(hWnd).d, wParam);
      osi.getMillisecs(&osi.eventTime);
      in.k.addChar((ulong)wParam, &osi.eventTime);
      return 0;
    case WM_DEVICECHANGE:
      in.populate();                        /// a call to in.populate to rescan for joysticks/gamepads/gamewheels
      goto ret;
      break;
    case WM_MOVE:
      w= osi.getWin(hWnd);
      /// hanles normal windows (no fullscreens)
      if(w)
        if(w->mode== 1) {
          w->x0= (int)(short)LOWORD(lParam);
          w->y0= (osi.display.vdy- 1)- (int)(short)HIWORD(lParam)- w->dy;
        }
      goto ret;
      break;
    case WM_SIZE:
      if(wParam== SIZE_RESTORED) {      /// handling only window size change (there are minimize and maximize messages here)
        w= osi.getWin(hWnd);
        if(w)
          if(w->mode== 1) {
            w->dx= LOWORD(lParam);
            w->dy= HIWORD(lParam);
            osi.resizeGLScene(w->dx, w->dy);
          }
      }
      goto ret;
      break;
    case WM_WINDOWPOSCHANGING:
      /*
      tw= (WINDOWPOS *)&lParam;
      w= osi.getWin(hWnd);
      /// hanles normal windows (no fullscreens)
      if(w)
      if(w->mode== 0) {
        w->x0= tw->x;
        w->y0= osi.display.vdy- tw->y- 1;
        w->dx= tw->cx;
        w->dy= tw->cy;
      }
      */
      goto ret;
      break;
    //case WM_PAINT:      // TEST
//      return 0;

    // system commands
    case WM_SYSCOMMAND:
      if(chatty) printf("WM_SYSCOMMAND %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam);

      switch (wParam)	{
        case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				  return 0;                         /// prevent these from happening by not calling DefWinProc
        case SC_CLOSE: 
          if(chatty) printf("  SC_CLOSE %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam);
          osi.flags.exit= true;
          return 0;
      } /// switch the type of WM_SYSCOMMAND
    goto ret;
    //return 0;
  } /// switch message

  /// unhandled frequent windows messages
  if(chatty&& !onlyHandled)
    switch(m) {
      case WM_ACTIVATE: if(chatty) printf("WM_ACTIVATE %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;
      case WM_ERASEBKGND: if(chatty) printf("WM_ERASEBKGND %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;
      case WM_PAINT: if(chatty) printf("WM_PAINT %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;
      case WM_NCPAINT: if(chatty) printf("WM_NCPAINT %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;
      case WM_SETFOCUS: if(chatty) printf("WM_SETFOCUS %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;       /// keyboard focus i think
      case WM_KILLFOCUS: if(chatty) printf("WM_KILLFOCUS %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;     /// keyboard focus i think
      case WM_NCACTIVATE: if(chatty) printf("WM_NCACTIVATE %s 0x%x %d %d\n", osi.getWinName(hWnd).d, m, wParam, lParam); goto ret;
      case WM_GETICON: if(chatty) printf("WM_GETICON\n"); goto ret;              /// usually is used when alt-tabbing, gets an icon for the mini alt-tab list
        // WHEN dealing with icons, must remember to develop WM_GETICON too
      case WM_IME_NOTIFY: if(chatty) printf("WM_IME_NOTIFY\n"); goto ret;
      case WM_IME_SETCONTEXT: if(chatty) printf("WM_IME_SETCONTEXT\n"); goto ret;
      case WM_NCHITTEST: goto ret;            /// something to do with mouse placement
      case WM_NCMOUSEMOVE: goto ret;          /// non client area mouse coords (top title/ moving bar, is a non-client for example)
      case WM_NCMOUSELEAVE: goto ret;         /// non client area mouse leaving window
    } /// switch

  if(chatty&& !onlyHandled)
    printf("UNKNOWN %s 0x%x %d %d\n", osi.getWinName(hWnd), m, wParam, lParam);
  /// this DefWindowProc() handles window movement & resize & rest... without this, moving is not working, for example
ret:
  if(timeFunc) osi.getNanosecs(&end);
  if(timeFunc) printf("processMSG lag: %llu\n", end- start);
  return DefWindowProc(hWnd, m, wParam, lParam);
}

#endif /// OS_WIN



#ifdef OS_LINUX
void osinteraction::processMSG()  {
  XEvent event;
  osiWindow *w= null;

  while(XPending(primWin->dis)) {       /// while there are messages in queue, loop thru them
    XNextEvent(primWin->dis, &event);
    
// ########################### MOUSE EVENTS ################################# //
      
// ---------------============ MOUSE MOVEMENT ===============-------------------
    if(event.type == MotionNotify) { /// this is the first event handled, because it is spammed
      /// oldx&y, dx&y removed; now updated on each in.update() call 
      in.m.x= event.xmotion.x_root;
      in.m.y= osi.display.vdy- event.xmotion.y_root- 1; test this
      coodonate unification missing <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      continue;

// ---------------============ BUTTON PRESS =================-------------------
    } else if(event.type == ButtonPress) {
      flags.buttonPress= true;
      eventTime= event.xbutton.time;          /// compatible with getMillisecs()

      if(event.xbutton.button== 4) {          // wheel up
        in.m.twheel+= 1;
        continue;
      }
      if(event.xbutton.button== 5) {          // wheel down
        in.m.twheel-= 1;
        continue;
      }

      short a;
      if(event.xbutton.button< 4) {           // first 3 buttons
        a= event.xbutton.button- 1;
        /// of course in windows there is another button order than in linux
        if(a== 1)
          a= 2;
        else if(a== 2)
          a= 1;
      } else                                  /// the rest of buttons are located after the mouse wheel
        a= event.xbutton.button- 3;

      in.m.b[a].down= true;
      in.m.b[a].timeStart= eventTime;
      continue;

// ---------------============ BUTTON RELEASE ===============-------------------
    } else if(event.type == ButtonRelease) {
      eventTime= event.xbutton.time;          /// compatible with getMillisecs()
      flags.buttonPress= false;

      /// ignore mouse wheel up msgs (linux sends them @ same time as button press)
      if((event.xbutton.button== 4) || (event.xbutton.button== 5))
        continue;

      short a;
      if(event.xbutton.button< 4) {           /// first 3 buttons?
        a= event.xbutton.button- 1;
        // of course in windows there is another button order than in linux
        if(a== 1)
          a= 2;
        else if(a== 2)
          a= 1;
      } else                                  /// the rest of buttons are located after the mouse wheel
        a= event.xbutton.button- 3;

      if(in.m.b[a].down) {                    /// an alt-bab might mess stuff...
        in.m.b[a].lastTimeStart= in.m.b[a].timeStart;
        in.m.b[a].lastTimeEnded= eventTime;
        in.m.b[a].lastDT= in.m.b[a].lastTimeEnded- in.m.b[a].lastTimeStart;
      } else {                                /// an alt-tab? might mess stuff
        in.m.b[a].lastTimeEnded= eventTime;
        in.m.b[a].lastTimeStart= eventTime- 1;
        in.m.b[a].lastDT= 1;
      }
      in.m.b[a].down= false;

      continue;

    // ############################ KEYBOARD EVENTS ############################# //

    // ---------------================ KEY PRESS ================-------------------
    } else if(event.type == KeyPress) {
      if(in.k.mode!= 1) continue;         /// only keyboard in [mode 1]
      
      flags.keyPress= true;               /// osi main flags
      eventTime= event.xkey.time;         /// compatible with getMillisecs()
      bool repeat= false;                 /// this will hold if it is a repeat character typed
      uint code= event.xkey.keycode;
      
      /// if the key is already down, this is a repeat message
      if(in.k.key[code]== 128)
        repeat= true;
      
      /// characters handling
      
      /// find the linux keysym
      KeySym ks;
      uint mods;
      XkbLookupKeySym(primWin->dis, code, event.xkey.state, &mods, &ks);
      
      /* THERE ARE MODS THAT I CAN'T FIND WHAT THEY DO
      if(mods)
        error.console( (string8("").f("there are still mods not applied to key %d\n", code)).d)
      */

      /// if the keysym can be a character, update the keyboard char stream
      ulong ch;
      in.getUnicode(&ks, &ch);
      if(ch)
        in.k.addChar(ch, &eventTime);
      
      /// if this is a real key press, log it and set vars
      if(!repeat) {
        if(chatty) printf("key PRESS code=%d \n", code);
        Keyboard::KeyPressed k;

        /// log the key
        k.code= code;
        k.checked= false;
        k.timeDown= eventTime;
        k.timeUp= 0;
        k.timeDT= 0;
        in.k.log(k);
        /// set the key as pressed & other needed vars
        in.k.key[code]= 128;
        in.k.keyTime[code]= eventTime;
        // in.k.repeat[code]= 128;        /// a new key press, sets repeat to 128  // MIGHT BE DELETED
      } /// if it's not a repeat
      
      in.k.updateLocks();                 /// update keyboard leds (repeat or not)
      in.k.doManip();                     /// check & handle if pressed keys form a manip char
      
      continue;
      
// ---------------=============== KEY RELEASE ===============-------------------
    } else if(event.type == KeyRelease) {
      if(in.k.mode!= 1) continue;             /// only keyboard in [mode 1]
      
      /// check if this is a repeat message (xlib sends key releases for repeats too...)
      if(XPending(primWin->dis)) {
        XEvent next;
        XPeekEvent(primWin->dis, &next);
        if(next.type== KeyPress)
          if(next.xkey.keycode== event.xkey.keycode)
            if(next.xkey.time== event.xkey.time)
              continue;
      }
      
      eventTime= event.xkey.time;             /// compatible with getMillisecs()
      flags.keyPress= false;
      uint code= event.xkey.keycode;
      
      in.k.updateLocks();
      
      if(chatty) printf("key RELEASE code=%d\n", code);

      /// log the key in history
      bool found= false;
      for(short a= 0; a< MAX_KEYS_LOGGED; a++)
        if(in.k.lastKey[a].code== code) {
          if(in.k.lastKey[a].timeUp) continue;
          in.k.lastKey[a].timeUp= eventTime;
          in.k.lastKey[a].timeDT= in.k.lastKey[a].timeUp- in.k.lastKey[a].timeDown;
          found= true;
          break;
        }
      
      /// in case the key was not found in history, add a hist-log with insta-keydown-keyup
      if(!found)  {
        Keyboard::KeyPressed k;
        k.code= code;
        k.checked= false;
        k.timeUp= eventTime;
        k.timeDown= k.timeUp- 1;              /// 1 milisecond before the keyup
        k.timeDT= 1;                          /// timeUp- timeDown
        in.k.log(k);
      }

      /// set the key as de-pressed & other vars
      in.k.key[code]= 0;
      in.k.keyTime[code]= 0;
      // in.k.repeat[code]= 0;  <<< DELETE ??? (not using)

      continue;



// ###################### WINDOW HANDLING / FOCUS ########################### //
      
// ---------------============== EXPOSE EVENT ===============-------------------
    } else if(event.type == Expose) {

      // IS ANYTHING HERE GOOD FOR ANYTHING????????????????
      // SUBJECT OF DELETION
      w= getWin(&event.xexpose.window);

      if(w->isMapped)
        XGetWindowAttributes(w->dis, w->win, &w->gwa); // update gwa window size / attributes

      continue;
//    } else if(event.type == NoExpose) {
//      continue;
            
// ---------------=========== MAP NOTIFY EVENT ==============-------------------
    } else if(event.type == MapNotify) {

      // IS ANYTHING HERE GOOD FOR ANYTHING????????????????
      // SUBJECT OF DELETION

      w= getWin(&event.xmap.event);
      w->isMapped= true;
      /*
      printf("%s mapped\n", w->name.d);
      if(primWin->isMapped)
        for(short a= 1; a< MAX_WINDOWS; a++)
          if(win[a].isCreated && !win[a].isMapped) {
            XMapWindow(primWin->dis, win[a].win);
            printf("window nr %d mapped\n", a);
            win[a].isMapped= true;

            XStoreName(w->display, w->win, w->name);

          ///  handle the close button WM
            Atom wmDelete= XInternAtom(w->display, "WM_DELETE_WINDOW", True);
            XSetWMProtocols(w->display, w->win, &wmDelete, 1);
          }
  */
      continue;
      
// ---------------=========== UNMAP NOTIFY EVENT ============-------------------
    } else if(event.type == UnmapNotify) {

      // IS ANYTHING HERE GOOD FOR ANYTHING????????????????
      // SUBJECT OF DELETION

      w= getWin(&event.xexpose.window);
      w->isMapped= false;
      if(chatty) printf("window UNmapped [%s]\n", w->name.d);
      continue;
      
// ---------------============ ENTER NOTIFY =================-------------------
/// mouse moves to a certain window... might be usefull in the future
    } else if(event.type == EnterNotify) {
      if((event.xcrossing.mode== NotifyGrab)|| (event.xcrossing.mode==NotifyUngrab)) /// ignore
        continue;

      if(event.xcrossing.mode== NotifyNormal)
        continue;

// ---------------============ LEAVE NOTIFY =================-------------------
/// mouse leaves a certain window... might be usefull in the future
    } else if(event.type == LeaveNotify) {
      if((event.xcrossing.mode== NotifyGrab)|| (event.xcrossing.mode==NotifyUngrab)) /// ignore grabs
        continue;

      if(event.xcrossing.mode== NotifyNormal)
        continue;

// ---------------=============== FOCUS IN ==================-------------------

    /// once a window is mapped, it is in the hands of WM (window manager)
    /// so use only window manager messages ("_NET_BLABLA")
    } else if(event.type == FocusIn) { // these are spammed
      if(chatty) printf("focusIn\n");

      if(event.xfocus.mode== NotifyNormal) {
        if(flags.haveFocus) {                    // ignore if already focused
          if(chatty) printf("FocusIn:IGNORED: xchange focus between internal program windows\n");
          continue;
        }

      if(chatty) printf("FocusIn:NotifyNormal, haveFocus=%d\n", flags.haveFocus);

        /// fullscreen
        if(primWin->mode== 2|| primWin->mode== 3|| primWin->mode== 4)
          for(short a= 0; a< MAX_WINDOWS; a++)  /// for each (created) window
            if(win[a].isCreated) {
              /// [mode 2] resolution change
              if(win[a].mode== 2) {
                if(chatty) printf("Changing resolution for window [%d]\n", a);
                display.changeRes(&win[a], win[a].monitor, win[a].dx, win[a].dy, win[a].bpp, win[a].freq);
              }
              /// set the window 'on top' of everything
              win[a].setWMstate(1, "_NET_WM_STATE_ABOVE");
              win[a].setWMstate(0, "_NET_WM_STATE_BELOW");
              //setFullScreen(&win[a], true); // THIS IS A POSSIBILITY
            } /// is window created

        flags.minimized= false;
        flags.haveFocus= true;
      } /// NotifyNormal

      continue;

// ---------------=============== FOCUS OUT =================-------------------

    /// once a window is mapped, it is in the hands of WM (window manager)
    /// so use only window manager messages ("_NET_BLABLA")
    /// XIconify i think uses WM messages, so it works
    } else if(event.type == FocusOut) { // these are spammed
      if(chatty) printf("focusOut\n");

      if(event.xfocus.mode== NotifyNormal) {      /// it can be a grab or something
        /// if next event is a focus in, there's only a change of focus between program windows,
        /// so this focus is ignored (same as the next, cuz already have focus)
        if(XPending(primWin->dis)) {
          XEvent tmp;
          XPeekEvent(primWin->dis, &tmp);
          if((tmp.type == FocusIn)&& (tmp.xfocus.mode== NotifyNormal)) {
            if(chatty) printf("FocusOut:IGNORED: xchange focus between internal program windows\n");
            continue;
          }
        }

        if(chatty) printf("FocusOut:NotifyNormal, haveFocus=%d\n", flags.haveFocus);

        if(!flags.haveFocus)                      /// ignore if already not focused
          continue;

        /// fullscreen
        if(primWin->mode== 2|| primWin->mode== 3|| primWin->mode== 4)
          for(short a= 0; a< MAX_WINDOWS; a++)     /// for each (created) window
            if(win[a].isCreated) {
              /// set the window 'below' every other windows
              win[a].setWMstate(0, "_NET_WM_STATE_ABOVE");
              win[a].setWMstate(1, "_NET_WM_STATE_BELOW");
              // setFullScreen(&win[a], false); // THIS IS A PosiBILITY

              /// [mode 2] resolution change & window iconification
              if(win[a].mode== 2) {
                if(chatty) printf("Changing to original resolution for window [%d]\n", a);
                XIconifyWindow(win[0].dis, win[a].win, win[a].monitor->screen);
                display.restoreRes(&win[a], win[a].monitor);
                flags.minimized= true;
              }
            } /// if window is created
        
        /// clear every button / key buffer / fix history for every button
        in.k.clearTypedBuffer();
        in.resetPressedButtons();
        
        flags.haveFocus= false;
      } /// if FocusOut:NotifyNormal

      continue;

// ---------------============ CLOSE BUTTON PRESS ===========-------------------
    /// ... it is defined in createGLWindow() as a custom i guess
    /// i think this needs more testing, cuz it seems ALL client msgs are treated
    /// as the close message ATM
    } else if(event.type == ClientMessage) {
      flags.exit= true;
      continue;

// ---------------============== DESTROY NOTIFY =============-------------------
    } else if(event.type == DestroyNotify) {
      error.simple("destroy notify not handled");
      continue;

    } else if(event.type == CirculateNotify) {
      if(chatty) printf("circulate notify\n");
    } else
      if(chatty) printf("Unhandled unknown message\n");

    //} else
    //  XFlush(win[0].display); // why flush msgs? pass thru all, right?
  
  } /// while there are messages in queue
  
} // osinteraction::processMSG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif /// OS_LINUX


#ifdef OS_MAC
// MAC variant of processMSG is in OScocoa.mm. there is Objective-C specific stuff
#endif /// OS_MAC



bool osinteraction::checkMSG() {
  bool ret= false;
  
  getNanosecs(&present);       // current time, or 'present' variable updated here <<<
  
  #ifdef OS_WIN
  while(1)    // loop thru ALL msgs... i used to peek thru only 1 msg, that was baaad... biig LAG
    if(PeekMessage(&win[0].msg, NULL, 0, 0, PM_REMOVE)) {	// Is There A Message Waiting?
      // eventTime= win[0].msg.time; // not reliable. 1 sec after getMillisecs(). this time is in the dang future
      TranslateMessage(&win[0].msg);
      DispatchMessage(&win[0].msg);

      ret= true;
    } else
      break;

  /// i think processMSG is called by WindProc() when DispatchMessage() is called (hopefully !)
  #endif /// OS_WIN

  #ifdef OS_LINUX
  processMSG();
  ret= true;
  #endif /// OS_LINUX

  #ifdef OS_MAC
  processMSG();
  ret= true;
  #endif /// OS_MAC

  return ret;
} /// osinteraction::checkMSG









// -----------============= WINDOW CLASS =============------------------
///=====================================================================
#ifdef OS_WIN
MSG osiWindow::msg= {0};
#endif

#ifdef OS_LINUX
Display *osiWindow::dis= null;
#endif

osiWindow::osiWindow() {
  isCreated= false;
  x0= y0= dx= dy= 0;
  freq= bpp= 0;

  monitor= null;

  #ifdef OS_WIN
  hDC= NULL;
  hInstance= NULL;
  hWnd= NULL;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  root= 0;              /// root window (desktop/server main window/ watever)
  win= 0;               /// window 'handle' or watever (per monitor)

  glRenderer= 0;        /// openGL renderer
  isMapped= false;
  #endif /// OS_LINUX

  #ifdef OS_MAC
  win= null;
  view= null;
  glRenderer= null;   // n/u
  #endif /// OS_MAC
}

osiWindow::~osiWindow() {
  #ifdef OS_WIN
  delData();            // it seems system already destroys windows, and this causes segmentation error
  #endif /// OS_WIN
  
  #ifdef OS_LINUX
  //  delData();        // it seems system already destroys windows, and this causes segmentation error  
  #endif
  
  #ifdef OS_MAC
  // nothing to do atm
  #endif /// OS_MAC
}

void osiWindow::delData() {
  #ifdef OS_WIN
  if(hDC) {
    ReleaseDC(hWnd, hDC);
    hDC= NULL;
  }

  if(hWnd) {
    DestroyWindow(hWnd);
    hWnd= NULL;
  }

  if(name.len) {
    UnregisterClass(name, hInstance);   // << in case class name will differ in future
    hInstance= NULL;
  }
  // do not return;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  if(isCreated) {
    osi.glMakeCurrent(null);
    //osi.glDestroyRenderer(this); // THIS NEEDS TO BE PUT WITH glRenderer STRUCT WIP
    //glXDestroyContext(display, glRenderer);

    if(mode == 2)
      osi.display.restoreRes(this, monitor);

    XDestroyWindow(dis, win);
    this->isMapped= false;
  }
  // do not return
  #endif

  #ifdef OS_MAC
  win= null;
  view= null;

  // do not return
  #endif


  this->isCreated= false;
  this->mode= 0;
  this->bpp= 0;
  this->dx= 0;
  this->dy= 0;
  this->freq= 0;
  this->glr= null;
  this->monitor= null;
  this->name.delData();
  this->x0= 0;
  this->y0= 0;
}



#ifdef OS_LINUX

void osiWindow::setWMprop(string8 wmID, string8 wmProp, uint val1, uint val2) {
  Atom wm= XInternAtom(dis, wmID, False);
  Atom prop= XInternAtom(dis, wmProp, False);
  ulong data[5]= {val1, val2, 0, 0, 0};

  XChangeProperty(dis, win, wm, prop, 32, PropModeReplace, (cuchar*)data, 2);
}


void osiWindow::setWMstate(uint val, string8 prop1, string8 prop2) {
  XEvent xev;
  for(short a= 0; a< sizeof(xev); a++) ((char*)&xev)[a]= 0;               /// clear xev
/// set vals
  xev.type= ClientMessage;
  xev.xclient.window= win;
  xev.xclient.message_type= XInternAtom(dis, "_NET_WM_STATE", False);
  xev.xclient.format= 32;
  xev.xclient.data.l[0]= val;
  xev.xclient.data.l[1]= XInternAtom(dis, prop1, False);
  if(prop2.d)
    xev.xclient.data.l[2]= XInternAtom(dis, prop2, False);
  xev.xclient.data.l[3]= 1; // 0= oldApp/ignore, 1= normal app, 2= pagers/other clients
/// send the message to root
  XSendEvent(dis, root, False, SubstructureRedirectMask| SubstructureNotifyMask, &xev);
}

#endif /// OS_LINUX




// END osiWindow class //
///------=======------///








void osinteraction::getNanosecs(uint64 *out) {
  #ifdef OS_WIN
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  /// "(t.QuadPart* 1000000000)/ timerFreq.QuadPart" would be faster& simpler,
  /// but unfortunately IT REACHES unsigned long long (uint64) LIMIT;
  /// a solution i found is to use 2 uint64 helper vars, each holding half the final result

  uint64 hi= t.QuadPart/ 10000000;
  uint64 lo= t.QuadPart- (hi* 10000000);
  lo= (lo* 1000000000)/ timerFreq.QuadPart;
  hi= (hi* 1000000000)/ timerFreq.QuadPart;
  
  *out= hi* 10000000+ lo;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 1000000000)+ t.tv_nsec;
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= (mach_absolute_time()* machInfo.numer/ machInfo.denom);
  #endif /// OS_MAC
}


void osinteraction::getMicrosecs(uint64 *out) {
  #ifdef OS_WIN
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  /// "(t.QuadPart* 1000000)/ timerFreq.QuadPart" would be faster& simpler,
  /// but unfortunately this ALMOST reaches the maximum value unsigned long long (uint64) can hold;
  /// a solution i found is to use 2 uint64 helper vars, each holding half the final result

  uint64 hi= t.QuadPart/ 10000000;
  uint64 lo= t.QuadPart- (hi* 10000000);
  hi= (hi* 1000000)/ timerFreq.QuadPart;
  lo= (lo* 1000000)/ timerFreq.QuadPart;

  *out= hi* 10000000+ lo;

  #endif /// OS_WIN

  #ifdef OS_LINUX
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 1000000)+ (t.tv_nsec/ 1000);
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= (mach_absolute_time()* machInfo.numer/ machInfo.denom)/ 1000;
  #endif /// OS_MAC
}


void osinteraction::getMillisecs(uint64 *out) {
  #ifdef OS_WIN
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  *out= (t.QuadPart* 1000)/ timerFreq.QuadPart;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 1000)+ (t.tv_nsec/ 1000000);
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= (mach_absolute_time()* machInfo.numer/ machInfo.denom)/ 1000000;
  #endif /// OS_MAC
}

// WIP - linux problems
void osinteraction::getClocks(uint64 *out) {
  #ifdef OS_WIN
  QueryPerformanceCounter((LARGE_INTEGER*)out);
  #endif /// OS_WIN

  #ifdef OS_LINUX
  //clock_getthedamnclocks(); <<<<<<<<<<<<<<<<<<<<
  /*
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 1000)+ (t.tv_nsec/ 1000000);
  */
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= mach_absolute_time();
  #endif /// OS_MAC
}

// WIP - linux problems
void osinteraction::clocks2nanosecs(uint64 *out) {
  #ifdef OS_WIN
  /// there has to be a split because ((*out)* 1000000000)/ timerFreq.QuadPart reaches uint64 limit
  uint64 hi= *out/ 10000000;
  uint64 lo= *out- (hi* 10000000);
  lo= (lo* 1000000000)/ timerFreq.QuadPart;
  hi= (hi* 1000000000)/ timerFreq.QuadPart;
  
  *out= hi* 10000000+ lo;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  //<<< well, shait...
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 1000000000)+ t.tv_nsec;
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= ((*out)* machInfo.numer/ machInfo.denom);
  #endif /// OS_MAC
}

// WIP - linux problems
void osinteraction::clocks2microsecs(uint64 *out) {
  #ifdef OS_WIN
  *out= (*out* 1000000)/ timerFreq.QuadPart;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  //<<< well, shait...
  /*
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 10000bla00000)+ t.tv_nsec;
  */
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= ((*out)* machInfo.numer/ machInfo.denom)/ 1000;
  #endif /// OS_MAC
}

// WIP - linux problems
void osinteraction::clocks2millisecs(uint64 *out) {
  #ifdef OS_WIN
  *out= (*out* 1000)/ timerFreq.QuadPart;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  //<<< well, shait...
  /*
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  *out= (t.tv_sec* 1000000bla000)+ t.tv_nsec;
  */
  #endif /// OS_LINUX

  #ifdef OS_MAC
  *out= ((*out)* machInfo.numer/ machInfo.denom)/ 1000000;
  #endif /// OS_MAC
}




void osinteraction::exit(int retVal) {
  display.restoreAllRes();
  #ifdef OS_WIN
  ::exit(retVal);
  #endif /// OS_WIN

  #ifdef OS_LINUX
  _exit(retVal);
  #endif /// OS_LINUX

  #ifdef OS_MAC
  _exit(retVal);
  #endif /// OS_MAC
}









///-------------=======================-------------///
// ============= OPENGL SPECIFIC STUFF ============= //
///-------------=======================-------------///

/// [internal]
osiRenderer *createRenderer(osiWindow *w) {
  osiRenderer *r= new osiRenderer;
  
  #ifdef OS_WIN
  r->glContext= wglCreateContext(w->hDC);
  #endif /// OS_WIN

  #ifdef OS_LINUX
  makeme
  #endif /// OS_LINUX

  #ifdef OS_MAC
  makeme
  #endif /// OS_MAC

  if(!r->glContext) {
    delete r;
    return null;
  }
  w->glr= r;
  w->monitor->glr= r;
  osi.glRenderers.add(r);
  return r;
}

/// create or assign a renderer to selected window; returns pointer to the renderer or null if failed, somehow
osiRenderer *osinteraction::assignRenderer(osiWindow *w) {
  // make shure w and w's monitor are not null
  if(!w) return null;
  if(!w->monitor) return null;

  osiRenderer *r;                  /// tmp var

  /// if there are existing renderers created, check if the requested window can use one of them
  if(glRenderers.nrNodes) {

    /// check if any renderer is created on current monitor (if there is one, assign it to w)
    if(w->monitor->glr)
      return w->glr= w->monitor->glr;

    // quote from MSDN:
    //  "It need not be the same hdc that was passed to wglCreateContext when hglrc was created,
    //  but it must be on the same device and have the same pixel format."
    // if this is true, BINGO! a test can be done to see if a renderer can be activated on a window; if it can, no need for a new one!

    glMakeCurrent(null);

    /// check if it is possible to activate an existing renderer to this window
    for(r= (osiRenderer *)glRenderers.first; r; r= (osiRenderer *)r->next) {
      w->glr= r;
      if(glMakeCurrent(w))           /// try to make it current
        return w->monitor->glr= r;    // successfull made current
      w->glr= null;
    }

    // other tests can be done... need the new computer to test every possibility and OS

    // more tests to be placed here ^^^
  } /// if there are renderers already created

  if(!createRenderer(w)) return null;         /// reached this point -> create a new renderer

  return w->glr;
}


/// deletes the specified renderer and makes shure that windows and monitors that used it, know about it
void osinteraction::delRenderer(osiRenderer *r) {

  /// if any window uses this renderer, set it to null
  for(short a= 0; a< MAX_WINDOWS; a++)
    if(win[a].glr== r)
      win[a].glr= null;
  
  /// if any monitor uses this renderer, set it to null
  for(short a= 0; a< display.nrMonitors; a++)
    if(display.monitor[a].glr== r)
      display.monitor[a].glr= null;


  /// if the selected renderer is the one that has to be deleted, make shure everything is ok
  if(glr== r) {
    glr= null;
    glMakeCurrent(null);
  }
  
  glRenderers.del(r);
}



///--------------------------------------///
// OPENGL functions that are OS dependant //
///--------------------------------------///

// SWAP BUFFERS
void osinteraction::swapPrimaryBuffers() {
  swapBuffers(&win[0]);
}


void osinteraction::swapBuffers(osiWindow *w) {
  #ifdef OS_WIN
  //SwapBuffers(w->hDC);      /// standard; the wgl one has more possibilities
  wglSwapLayerBuffers(w->hDC, WGL_SWAP_MAIN_PLANE);
  #endif /// OS_WIN

  #ifdef OS_LINUX
  glXSwapBuffers(w->dis, w->win);
  #endif /// OS_LINUX

  #ifdef OS_MAC
  cocoa.swapBuffers(w);
  #endif /// OS_MAC
} // osinteraction::swapBuffers



bool osinteraction::glMakeCurrent(osiWindow *w) {
  if(w) {
    if(glr) glr->isActive= false;     /// set not active flag for current renderer
    glr= w->glr;
    if(glr) glr->isActive= true;      /// set active flag of new renderer
  } else {
    if(glr) glr->isActive= false;     /// set not active flag for current renderer
    glr= null;
  }

  #ifdef OS_WIN
  if(w)
    return wglMakeCurrent(w->hDC, w->glr->glContext)? true: false;
  else
    return wglMakeCurrent(null, null)? true: false;
  #endif /// OS_WIN

  #ifdef OS_LINUX
  if(w)
    return glXMakeCurrent(w->dis, w->win, w->glr->glRenderer);
  else
    make a null check. if a null is called this has to be done
    return true; //glXMakeCurrent(primWin->dis, None, NULL);
  #endif /// OS_LINUX

  #ifdef OS_MAC
  if(w)
    cocoa.makeCurrent(w); // as it is, this is too simple <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< check CGL
    
    this must be changed-> the glc (core)'s function has the possibility to makecurrent(null) and returns an error message if something went wrong
    
  return false; // <<<< better atm
  return true;
  #endif /// OS_MAC
  
  return false;
} // osinteraction::glMakeCurrent













// NOT OS DEPENDANT pure gl stuff <----------------------------------------------------------
// ------------------------------------------------------------------------------------------

bool osinteraction::resizeGLScene(GLsizei dx, GLsizei dy) {
  bool ret= true;
  if (dy==0)	dy= 1; /// prevent a divide by 0

  glViewport(0, 0, dx, dy);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, (GLfloat)dx/ (GLfloat)dy, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return ret;
}






