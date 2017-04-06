/*
 * Stand-alone minimap over a set of windows
 *
 * Author: Ole Jørgen Brønner (olejorgenb@yahoo.no)
 *
 * Re-purposeded from mini-clutter-wm.c:
 *   Copyright (C) 2008 Andy Wingo <wingo at pobox dot com>
 *   I release this code into the public domain.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

Display *dpy;
ClutterActor *stage;
int stage_w, stage_h;
Window stage_win;

GType texture_pixmap_type;

void
prep_clutter (int *argc, char ***argv)
{
    clutter_init (argc, argv);
    
    if (getenv ("NO_TFP"))
        texture_pixmap_type = CLUTTER_X11_TYPE_TEXTURE_PIXMAP;
    else
        texture_pixmap_type = CLUTTER_GLX_TYPE_TEXTURE_PIXMAP;
}

void
prep_stage()
{
  stage = clutter_stage_get_default ();
  clutter_stage_set_user_resizable(stage, TRUE);
  stage_win = clutter_x11_get_stage_window (CLUTTER_STAGE (stage));
  clutter_actor_show_all (stage);
}

static void
window_position_changed (ClutterActor *tex, GParamSpec *pspec, gpointer unused)
{
    gint x, y, window_x, window_y;
    g_object_get (tex, "x", &x, "y", &y, "window-x", &window_x,
                  "window-y", &window_y, NULL);
    if (x == window_x && y == window_y)
        return;
    printf("Position (%d,%d), (%d,%d)\n", x, y, window_x, window_y);
    
    clutter_actor_set_position (tex, window_x, window_y);
}

static void
window_mapped_changed (ClutterActor *tex, GParamSpec *pspec, gpointer unused)
{
    gint mapped;
    g_object_get (tex, "mapped", &mapped, NULL);

    if (mapped){
        printf("mapped\n");
        clutter_container_add_actor (CLUTTER_CONTAINER (stage), tex);
        clutter_actor_show (tex);
        window_position_changed (tex, NULL, NULL);
    } else {
        clutter_container_remove_actor (CLUTTER_CONTAINER (stage), tex);
    }
}

static void
window_destroyed (ClutterActor *tex, GParamSpec *pspec, gpointer unused)
{
    g_print ("window destroyed\n");
}

static void
add_xwindow (Window w)
{
    XWindowAttributes attr;    
    ClutterActor *tex;

    XGetWindowAttributes (dpy, w, &attr);
    if (attr.class == InputOnly)
        return;
    
    tex = g_object_new (texture_pixmap_type, "window", w,
                        "automatic-updates", TRUE, NULL);

    clutter_container_add_actor (CLUTTER_CONTAINER (stage), tex);
    clutter_actor_show (tex);
    window_position_changed(tex, NULL, NULL);

    g_signal_connect (tex, "notify::mapped", G_CALLBACK (window_mapped_changed), NULL);
    g_signal_connect (tex, "notify::destroyed", G_CALLBACK (window_destroyed), NULL);
    g_signal_connect (tex, "notify::window-x", G_CALLBACK (window_position_changed), NULL);
    g_signal_connect (tex, "notify::window-y", G_CALLBACK (window_position_changed), NULL);

    return ;

    /* { */
    /*     gint mapped, destroyed; */
    /*     g_object_get (tex, "mapped", &mapped, "destroyed", &destroyed, NULL); */
        
    /*     if (mapped) */
    /*         window_mapped_changed (tex, NULL, NULL); */
    /*     if (destroyed) */
    /*         window_destroyed (tex, NULL, NULL); */
    /* } */
}

static ClutterX11FilterReturn
event_filter (XEvent *ev, ClutterEvent *cev, gpointer unused)
{
    switch (ev->type) {
    case ConfigureNotify:
        {
            printf("event_filter pos: %d, %d\n", ev->xconfigure.x, ev->xconfigure.y);
            return CLUTTER_X11_FILTER_CONTINUE;
        }
    /* case MapNotify: */
    /* case UnmapNotify: */
    /* case DestroyNotify: */
    /* case ClientMessage: */
    /* case PropertyNotify: */
    /* case ButtonPress: */
    /* case EnterNotify: */
    /* case LeaveNotify: */
    /* case FocusIn: */
    /* case KeyRelease: */
    /* case VisibilityNotify: */
    /* case ColormapNotify: */
    /* case MappingNotify: */
    /* case MotionNotify: */
    /* case SelectionNotify: */
    default:
        return CLUTTER_X11_FILTER_CONTINUE;
    }
}


static int
parse_xid(const char *id_str, XID *ret)
{
  XID xid = 0;
  sscanf(id_str, "0x%x", &xid);
  if (!xid)
    sscanf(id_str, "%u", &xid);
  if (!xid) {
    return 0;
  }
  *ret = xid;
  return 1;
}

int
main (int argc, char *argv[])
{
    prep_clutter (&argc, &argv);

    // Init global display
    dpy = clutter_x11_get_default_display ();

    prep_stage();

    for(int i = 1; i < argc; i++) {
        Window w;
        parse_xid(argv[i], &w);
        printf("Added %d\n", w);
        add_xwindow (w);
    }

    g_main_loop_run (g_main_loop_new (NULL, FALSE));

    return 0;
}
