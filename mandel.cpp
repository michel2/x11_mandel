#include <X11/Xlib.h>
#include <stdlib.h>

#ifdef Wants_mandel_stats
#include <map> 
#endif

 
int main()
{
  Display* dpy = XOpenDisplay(NULL);
  if (dpy == NULL) 
    exit(1);
  int s = DefaultScreen(dpy);
  Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, s), 0, 0, 1400, 800, 1, BlackPixel(dpy, s), WhitePixel(dpy, s));
  XSelectInput(dpy, win, ExposureMask);
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "mandel");
  Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False); 
  XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);  
#ifdef Wants_mandel_stats
  std::map<unsigned long,int> escapes;
#endif
XEvent e;
  while (1)   {
    XNextEvent(dpy, &e);
    if (e.type == Expose)     {
      XWindowAttributes  wa;
      XGetWindowAttributes(dpy, win, &wa);      
      const long double width = wa.width;
      const long double  height = wa.height;
      const long double minsize = (wa.width<wa.height)?wa.width:wa.height;
      const long double scalex = width/minsize;
      const long double scaley = height/minsize;      
      const long double center = -0.6;
      const unsigned int maxIter = 256*4;
      for(int y = 0; y <height; y++){
	for(int x = 0; x <width; x++){
	  long double c = center + x*scalex / width - scalex/2;
	  long double ci = y*scaley / height - scaley/2;
	  long double z = 0.0;
	  long double zi = 0.0;	    
	  unsigned int index = 0;
	  
	  while(z*z + zi*zi < 4.0 && index<maxIter){
	    long double ztemp = z*z - zi*zi +c;
	    zi = 2*z*zi +ci;
	    z  = ztemp;
	    ++index;
	  }
	  unsigned long color =  (0x2000)*(index)/(maxIter);
	  XSetForeground(dpy,DefaultGC(dpy,s), color);
	  XDrawPoint(dpy, win, DefaultGC(dpy, s), x, y); 
#ifdef Wants_mandel_stats
	  if(index!=maxIter){
	    std::map<unsigned long, int>::iterator it = escapes.find(color);
	    if(it != escapes.end())
	      it->second++;
	    else
	      escapes[color] = 1;
	  }	
#endif
	}//x	
      }//y
#ifdef Wants_mandel_stats 
      std::map<unsigned long,int>::iterator it = escapes.begin();
      while(it != escapes.end()){	
	printf("%X %d\n ", it->first, it->second);
      ++it;
      }
      printf("map sz:%lu\n",escapes.size());
#endif
    }//expose
    if ((e.type == ClientMessage) && 
        (static_cast<unsigned int>(e.xclient.data.l[0]) == WM_DELETE_WINDOW))
      break;
 
  }
 
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  return 0;
}
