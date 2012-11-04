/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; c-basic-offset: 2; -*- */

#include "config.h"

#include <glib-object.h>

#include <X11/extensions/Xfixes.h>
#include <meta/util.h>
#include <meta/barrier.h>
#include "display-private.h"
#include "mutter-enum-types.h"

G_DEFINE_TYPE (MetaBarrier, meta_barrier, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), META_TYPE_BARRIER, MetaBarrierPrivate))

enum {
  PROP_0,

  PROP_DISPLAY,

  PROP_X1,
  PROP_Y1,
  PROP_X2,
  PROP_Y2,
  PROP_DIRECTIONS,

  PROP_LAST,
};

static GParamSpec *obj_props[PROP_LAST];

struct _MetaBarrierPrivate
{
  MetaDisplay *display;

  int x1;
  int y1;
  int x2;
  int y2;

  MetaBarrierDirection directions;

  PointerBarrier barrier;
};

static void
meta_barrier_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  MetaBarrier *barrier = META_BARRIER (object);
  MetaBarrierPrivate *priv = barrier->priv;
  switch (prop_id)
    {
    case PROP_DISPLAY:
      g_value_set_object (value, priv->display);
      break;
    case PROP_X1:
      g_value_set_int (value, priv->x1);
      break;
    case PROP_Y1:
      g_value_set_int (value, priv->y1);
      break;
    case PROP_X2:
      g_value_set_int (value, priv->x2);
      break;
    case PROP_Y2:
      g_value_set_int (value, priv->y2);
      break;
    case PROP_DIRECTIONS:
      g_value_set_flags (value, priv->directions);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
meta_barrier_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  MetaBarrier *barrier = META_BARRIER (object);
  MetaBarrierPrivate *priv = barrier->priv;
  switch (prop_id)
    {
    case PROP_DISPLAY:
      priv->display = g_value_get_object (value);
      break;
    case PROP_X1:
      priv->x1 = g_value_get_int (value);
      break;
    case PROP_Y1:
      priv->y1 = g_value_get_int (value);
      break;
    case PROP_X2:
      priv->x2 = g_value_get_int (value);
      break;
    case PROP_Y2:
      priv->y2 = g_value_get_int (value);
      break;
    case PROP_DIRECTIONS:
      priv->directions = g_value_get_flags (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
meta_barrier_dispose (GObject *object)
{
  MetaBarrier *barrier = META_BARRIER (object);
  MetaBarrierPrivate *priv = barrier->priv;

  if (meta_barrier_is_active (barrier))
    {
      meta_bug ("MetaBarrier wrapper %p for X barrier %ld was destroyed"
                "while the X barrier is still active.",
                barrier, priv->barrier);
    }

  G_OBJECT_CLASS (meta_barrier_parent_class)->dispose (object);
}

gboolean
meta_barrier_is_active (MetaBarrier *barrier)
{
  return barrier->priv->barrier > 0;
}

static void
meta_barrier_activate (MetaBarrier *barrier)
{
  MetaBarrierPrivate *priv = barrier->priv;
  Display *dpy;
  Window root;

  if (priv->display == NULL)
    {
      g_warning ("Trying to activate barrier without an associated display.");
      return;
    }

  dpy = priv->display->xdisplay;
  root = DefaultRootWindow (dpy);

  if (meta_barrier_is_active (barrier))
    return;

  priv->barrier = XFixesCreatePointerBarrier (dpy, root,
                                              priv->x1, priv->y1,
                                              priv->x2, priv->y2,
                                              priv->directions, 0, NULL);
}

static void
meta_barrier_constructed (GObject *object)
{
  meta_barrier_activate (META_BARRIER (object));

  G_OBJECT_CLASS (meta_barrier_parent_class)->constructed (object);
}

static void
meta_barrier_class_init (MetaBarrierClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = meta_barrier_get_property;
  object_class->set_property = meta_barrier_set_property;
  object_class->dispose = meta_barrier_dispose;
  object_class->constructed = meta_barrier_constructed;

  obj_props[PROP_DISPLAY] =
    g_param_spec_object ("display", "", "",
                         META_TYPE_DISPLAY,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  obj_props[PROP_X1] =
    g_param_spec_int ("x1", "", "",
                      0, 0xFFFF, 0,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  obj_props[PROP_Y1] =
    g_param_spec_int ("y1", "", "",
                      0, 0xFFFF, 0,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  obj_props[PROP_X2] =
    g_param_spec_int ("x2", "", "",
                      0, 0xFFFF, 0,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  obj_props[PROP_Y2] =
    g_param_spec_int ("y2", "", "",
                      0, 0xFFFF, 0,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  /**
   * MetaBarrier:directions:
   *
   * A set of #MetaBarrierDirections that the pointer barrier
   * should let through.
   */
  obj_props[PROP_DIRECTIONS] =
    g_param_spec_flags ("directions", "", "",
                        META_TYPE_BARRIER_DIRECTION,
                        0,
                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, PROP_LAST, obj_props);

  g_type_class_add_private (object_class, sizeof(MetaBarrierPrivate));
}

void
meta_barrier_destroy (MetaBarrier *barrier)
{
  MetaBarrierPrivate *priv = barrier->priv;
  Display *dpy;

  if (priv->display == NULL)
    {
      g_warning ("Trying to activate barrier without an associated display");
      return;
    }

  dpy = priv->display->xdisplay;

  if (!meta_barrier_is_active (barrier))
    return;

  XFixesDestroyPointerBarrier (dpy, priv->barrier);
  priv->barrier = 0;
}

static void
meta_barrier_init (MetaBarrier *barrier)
{
  barrier->priv = GET_PRIVATE (barrier);
}
