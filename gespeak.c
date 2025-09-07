#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//gcc gconsole.c -o gconsole -lX11
#define MAX_TEXT 6553600
char ccc[4096]="espeak -f";
char *c1=ccc;
int main() {
    Display *dpy;
    Window win;
    XEvent e;
    GC gc;
    FILE* f1;
    XFontStruct *font;
    unsigned long yellow, black, white;
    
    char textBuffer[MAX_TEXT] = ":> ";
    int scrollPos = 0;
    c1=ccc+strlen(ccc);
    c1[0]=0;
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Não foi possível abrir o display.\n");
        exit(1);
    }

    int screen = DefaultScreen(dpy);
    yellow = XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
                              "yellow", &(XColor){0}, &(XColor){0})
                 ? XBlackPixel(dpy, screen) ^ 0xFFFF00
                 : WhitePixel(dpy, screen);
    black = BlackPixel(dpy, screen);
    white = WhitePixel(dpy, screen);

    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
                              10, 10, 800, 600, 1,
                              black, yellow);

    XStoreName(dpy, win, "Janela Amarela (X11)");
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(dpy, win);

    gc = XCreateGC(dpy, win, 0, NULL);

    // Fonte 16px
    font = XLoadQueryFont(dpy, "-misc-fixed-*-*-*-*-16-*-*-*-*-*-*-*");
    if (!font) {
        fprintf(stderr, "Fonte não encontrada, a usar padrão.\n");
        font = XLoadQueryFont(dpy, "fixed");
    }
    XSetFont(dpy, gc, font->fid);
    XSetForeground(dpy, gc, black);

    char keybuf[32];
    while (1) {
        XNextEvent(dpy, &e);

        if (e.type == Expose) {
            // limpar fundo
            XSetForeground(dpy, gc, yellow);
            XFillRectangle(dpy, win, gc, 0, 0, 800, 600);
            XSetForeground(dpy, gc, black);

            // desenhar texto (com scroll)
            int y = 20 - scrollPos * 20;
            char *line = strtok(strdup(textBuffer), "\n");
            while (line) {
                XDrawString(dpy, win, gc, 10, y, line, strlen(line));
                
                y += 20;
                line = strtok(NULL, "\n");
            }
        }

        if (e.type == KeyPress) {
            KeySym keysym;
            int len = XLookupString(&e.xkey, keybuf, sizeof(keybuf), &keysym, NULL);
            if (len > 0 && strlen(textBuffer) < MAX_TEXT - 2) {
                if (keybuf[0] == '\r') {
                    strcpy(textBuffer,"");
                    
                    system(ccc);
                    
                    strcpy(ccc,"espeak -f");
                    
                    
                } else {
                    strcpy(textBuffer,ccc); 
                    strncat(textBuffer, keybuf, len);
                    strncat(ccc, keybuf, len);
                }
            }
            XSetForeground(dpy, gc, yellow);
            XFillRectangle(dpy, win, gc, 0, 0, 800, 600);
            XSetForeground(dpy, gc, black);

            // desenhar texto (com scroll)
            int y = 20 - scrollPos * 20;
            char *line = strtok(strdup(textBuffer), "\n");
            while (line) {
                XDrawString(dpy, win, gc, 10, y, line, strlen(line));
                
                y += 20;
                line = strtok(NULL, "\n");
            }

        }

        if (e.type == ButtonPress) {
            if (e.xbutton.button == 4) { // scroll up
                if (scrollPos > 0) scrollPos--;
            } else if (e.xbutton.button == 5) { // scroll down
                scrollPos++;
            }
            XSetForeground(dpy, gc, yellow);
            XFillRectangle(dpy, win, gc, 0, 0, 800, 600);
            XSetForeground(dpy, gc, black);

            // desenhar texto (com scroll)
            int y = 20 - scrollPos * 20;
            char *line = strtok(strdup(textBuffer), "\n");
            while (line) {
                XDrawString(dpy, win, gc, 10, y, line, strlen(line));
                
                y += 20;
                line = strtok(NULL, "\n");
            }


        }
    }

    XCloseDisplay(dpy);
    return 0;
}

