/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; c-basic-offset: 2; -*- */

#include "config.h"

#include <glib-object.h>

#include <X11/extensions/XInput2.h>
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
  PROP_DEVICES,

  PROP_LAST,
};

static GParamSpec *obj_props[PROP_LAST];

enum {
  HIT,

  LAST_SIGNAL,
};

static guint obj_signals[LAST_SIGNAL];

struct _MetaBarrierPrivate
{
  MetaDisplay *display;

  int x1;
  int y1;
  int x2;
  int y2;

  MetaBarrierDirection directions;
  GPtrArray *devices;

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
    case PROP_DEVICES:
      g_value_set_boxed (value, priv->devices);
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
    case PROP_DEVICES:
      priv->devices = g_value_dup_boxed (value);
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

  g_ptr_array_unref (priv->devices);

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
  int n_devices, *devices;

  if (priv->display == NULL)
    {
      g_warning ("Trying to activate barrier without an associated display.");
      return;
    }

  dpy = priv->display->xdisplay;
  root = DefaultRootWindow (dpy);

  if (meta_barrier_is_active (barrier))
    return;

  if (priv->devices != NULL)
    {
      int i;
      n_devices = priv->devices->len;
      devices = g_new (int, n_devices);
      for (i = 0; i < n_devices; i++)
        devices[i] = meta_device_get_id (g_ptr_array_index (priv->devices, i));
    }
  else
    {
      n_devices = 0;
      devices = NULL;
    }

  priv->barrier = XFixesCreatePointerBarrier (dpy, root,
                                              priv->x1, priv->y1,
                                              priv->x2, priv->y2,
                                              priv->directions,
                                              n_devices, devices);
  g_hash_table_insert (priv->display->barriers, GINT_TO_POINTER (priv->barrier), g_object_ref (barrier));
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

  /**
   * MetaBarrier:devices:
   *
   * An array of #MetaDevices that the poiner barrier should block.
   *
   * Type: GLib.PtrArray(Meta.Device)
   * Transfer: container
   */
  obj_props[PROP_DEVICES] =
    g_param_spec_boxed ("devices", "", "",
                        G_TYPE_PTR_ARRAY,
                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, PROP_LAST, obj_props);

  /**
   * MetaBarrier::hit:
   * @barrier: The #MetaBarrier that was hit
   * @event: A #MetaBarrierEvent that has the details of how
   * things were hit.
   *
   * When a pointer barrier is hit, this will trigger. This
   * requires an XI2-enabled server.
   */
  obj_signals[HIT] =
    g_signal_new ("hit",
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_FIRST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  META_TYPE_BARRIER_HIT_EVENT);

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
  g_hash_table_remove (priv->display->barriers, GINT_TO_POINTER (priv->barrier));
  priv->barrier = 0;
}

static void
meta_barrier_init (MetaBarrier *barrier)
{
  barrier->priv = GET_PRIVATE (barrier);
}

static void
meta_barrier_hit (MetaBarrier          *barrier,
                  XIBarrierNotifyEvent *xevent)
{
  MetaBarrierHitEvent *event = g_slice_new0 (MetaBarrierHitEvent);

  event->ref_count = 1;
  event->event_id = xevent->event_id;
  event->dt = xevent->dt;

  event->x = xevent->x;
  event->y = xevent->y;
  event->dx = xevent->dx;
  event->dy = xevent->dy;
  event->raw_dx = xevent->raw_dx;
  event->raw_dy = xevent->raw_dy;

  g_signal_emit (barrier, obj_signals[HIT], 0, event);
}

gboolean
meta_display_process_barrier_event (MetaDisplay *display,
                                    XEvent      *ev)
{
  if (ev->type == GenericEvent &&
      ev->xcookie.extension == display->xinput2_opcode)
    {
      MetaBarrier *barrier;
      XIBarrierNotifyEvent *xev;

      g_assert (display->have_xinput2 == TRUE);

      xev = (XIBarrierNotifyEvent *) ev->xcookie.data;

      if (xev->evtype != XI_BarrierHitNotify)
        return FALSE;

      barrier = g_hash_table_lookup (display->barriers, GINT_TO_POINTER (xev->barrier));
      if (barrier != NULL)
        {
          meta_barrier_hit (barrier, xev);
          return TRUE;
        }
    }
  return FALSE;
}

static MetaBarrierHitEvent *
meta_barrier_hit_event_ref (MetaBarrierHitEvent *event)
{
  g_return_val_if_fail (event != NULL, NULL);
  g_return_val_if_fail (event->ref_count > 0, NULL);

  g_atomic_int_inc ((volatile int *)&event->ref_count);
  return event;
}

static void
meta_barrier_hit_event_unref (MetaBarrierHitEvent *event)
{
  g_return_if_fail (event != NULL);
  g_return_if_fail (event->ref_count > 0);

  if (g_atomic_int_dec_and_test ((volatile int *)&event->ref_count))
    g_slice_free (MetaBarrierHitEvent, event);
}

G_DEFINE_BOXED_TYPE (MetaBarrierHitEvent,
                     meta_barrier_hit_event,
                     meta_barrier_hit_event_ref,
                     meta_barrier_hit_event_unref)
