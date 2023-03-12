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

#ifndef _GST_vimbaprovider_H_
#define _GST_vimbaprovider_H_

#include <glib.h>
#include <gst/base/base.h>

G_BEGIN_DECLS

/* The GstDeviceProvider */

typedef struct _GstVimbaSrcDeviceProvider GstVimbaSrcDeviceProvider;
typedef struct _GstVimbaSrcDeviceProviderClass GstVimbaSrcDeviceProviderClass;

#define GST_TYPE_VIMBASRC_DEVICE_PROVIDER             (gst_vimbasrc_device_provider_get_type())
#define GST_IS_VIMBASRC_DEVICE_PROVIDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VIMBASRC_DEVICE_PROVIDER))
#define GST_IS_VIMBASRC_DEVICE_PROVIDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_VIMBASRC_DEVICE_PROVIDER))
#define GST_VIMBASRC_DEVICE_PROVIDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_VIMBASRC_DEVICE_PROVIDER, GstVimbaSrcDeviceProviderClass))
#define GST_VIMBASRC_DEVICE_PROVIDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_VIMBASRC_DEVICE_PROVIDER, GstVimbaSrcDeviceProvider))
#define GST_VIMBASRC_DEVICE_PROVIDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_DEVICE_PROVIDER, GstVimbaSrcDeviceProviderClass))
#define GST_VIMBASRC_DEVICE_PROVIDER_CAST(obj)        ((GstVimbaSrcDeviceProvider *)(obj))

struct _GstVimbaSrcDeviceProvider {
  GstDeviceProvider         parent;
  GstDeviceProviderFactory* factory;

  GList *devices;
};

struct _GstVimbaSrcDeviceProviderClass {
  GstDeviceProviderClass    parent_class;
};

GType gst_vimbasrc_device_provider_get_type(void);

G_END_DECLS

#endif
