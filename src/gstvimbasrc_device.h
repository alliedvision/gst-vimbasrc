/*
 * Copyright (C) 2023 Allied Vision Technologies GmbH
 * Copyright (C) 2023 Fraunhofer Institute for Material and Beam Technology IWS
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_VIMBASRC_DEVICE_H_
#define _GST_VIMBASRC_DEVICE_H_

#include <glib.h>
#include <gst/base/base.h>

G_BEGIN_DECLS

/* The GstDevice */

typedef struct _GstVimbaSrcDevice GstVimbaSrcDevice;
typedef struct _GstVimbaSrcDeviceClass GstVimbaSrcDeviceClass;

#define GST_TYPE_VIMBASRC_DEVICE                 (gst_vimbasrc_device_get_type())
#define GST_IS_VIMBASRC_DEVICE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VIMBASRC_DEVICE))
#define GST_IS_VIMBASRC_DEVICE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_VIMBASRC_DEVICE))
#define GST_VIMBASRC_DEVICE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_VIMBASRC_DEVICE, GstVimbaSrcDeviceClass))
#define GST_VIMBASRC_DEVICE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_VIMBASRC_DEVICE, GstVimbaSrcDevice))
#define GST_VIMBASRC_DEVICE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_DEVICE, GstVimbaSrcDeviceClass))
#define GST_VIMBASRC_DEVICE_CAST(obj)            ((GstVimbaSrcDevice *)(obj))

struct _GstVimbaSrcDevice {
  GstDevice   parent;

  gchar       *camera_id;
  gchar       *camera_model;
  gchar       *camera_name;
  gchar       *interface_id;
  gchar       *serial;
  const gchar *element;
};

struct _GstVimbaSrcDeviceClass {
  GstDeviceClass    parent_class;
};

GType gst_vimbasrc_device_get_type(void);

G_END_DECLS

#endif // _GST_VIMBASRC_DEVICE_H_
