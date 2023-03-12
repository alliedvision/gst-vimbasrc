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

#include "gstvimbasrc_device.h"
#include "gstvimbasrc.h"

#include <gst/gst.h>

G_DEFINE_TYPE(GstVimbaSrcDevice, gst_vimbasrc_device, GST_TYPE_DEVICE)

enum
{
  PROP_CAMERA_ID = 1,
  PROP_CAMERA_MODEL,
  PROP_CAMERA_NAME,
  PROP_SERIAL,
  PROP_INTERFACE_ID
};

/**
 * @brief Called by GStreamer to create the source element matching the device
 *
 * @param device The device for which to create the source element
 * @param name Unique name of the new element (or NULL for automatic assignment)
 * @return GstElement* vimbasrc element
 */
static GstElement *
gst_vimbasrc_device_create_element(GstDevice *device, const gchar *name)
{
  GstVimbaSrcDevice *vimba_device = GST_VIMBASRC_DEVICE(device);
  GstElement *element = gst_element_factory_make(vimba_device->element, name);

  g_object_set (element, "camera", vimba_device->camera_id, NULL);

  return element;
}

/**
 * @brief Reconfigure a vimbasrc element to use this camera device
 *
 * @param device Device which the vimbasrc should use
 * @param element vimbasrc element to reconfigure
 * @return gboolean
 */
static gboolean
gst_vimbasrc_device_reconfigure_element(GstDevice *device, GstElement *element)
{
  GstVimbaSrcDevice *vimba_device = GST_VIMBASRC_DEVICE(device);
  if (!GST_IS_vimbasrc(element))
  {
      return FALSE;
  }

  g_object_set (element, "camera", vimba_device->camera_id, NULL);
  return TRUE;
}

/**
 * @brief GStreamer function called when getting a g_object property
 *
 * @param object The vimbasrc device
 * @param property_id The id of the property to get
 * @param value Output value pointer
 * @param pspec
 */
static void
gst_vimbasrc_device_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
  GstVimbaSrcDevice *device = GST_VIMBASRC_DEVICE_CAST(object);

  switch (property_id) {
    case PROP_CAMERA_ID:
      g_value_set_string(value, device->camera_id);
      break;
    case PROP_CAMERA_NAME:
      g_value_set_string(value, device->camera_name);
      break;
    case PROP_CAMERA_MODEL:
      g_value_set_string(value, device->camera_model);
      break;
    case PROP_SERIAL:
      g_value_set_string(value, device->serial);
      break;
    case PROP_INTERFACE_ID:
      g_value_set_string(value, device->interface_id);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

/**
 * @brief GStreamer function called when setting a property (GObject)
 *
 * @param object The vimbasrc device
 * @param property_id Id of the property to set
 * @param value Value to assign to the property
 * @param pspec
 */
static void
gst_vimbasrc_device_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
  GstVimbaSrcDevice *device = GST_VIMBASRC_DEVICE_CAST(object);

  switch (property_id) {
    case PROP_CAMERA_ID:
      g_free(device->camera_id);
      device->camera_id = g_value_dup_string(value);
      break;
    case PROP_CAMERA_NAME:
      g_free(device->camera_name);
      device->camera_name = g_value_dup_string(value);
      break;
    case PROP_CAMERA_MODEL:
     g_free(device->camera_model);
      device->camera_model = g_value_dup_string(value);
      break;
    case PROP_SERIAL:
      g_free(device->serial);
      device->serial = g_value_dup_string(value);
      break;
    case PROP_INTERFACE_ID:
      g_free(device->interface_id);
      device->interface_id = g_value_dup_string(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

/**
 * @brief GLib function called to free a VimbaSrcDevice
 *
 * @param object VimbaSrcDevice to free
 */
static void
gst_vimbasrc_device_finalize (GObject *object)
{
  GstVimbaSrcDevice *device = GST_VIMBASRC_DEVICE_CAST(object);
  g_free(device->camera_id);
  g_free(device->camera_name);
  g_free(device->camera_model);
  g_free(device->serial);
  g_free(device->interface_id);

  G_OBJECT_CLASS (gst_vimbasrc_device_parent_class)->finalize(object);
}

/**
 * @brief GLib function called to initialize the VimbaSrcDevice class struct
 *
 * The place to install all properties.
 *
 * @param klass
 */
static void
gst_vimbasrc_device_class_init (GstVimbaSrcDeviceClass * klass)
{
  GstDeviceClass *dev_class = GST_DEVICE_CLASS(klass);
  GObjectClass *object_class = G_OBJECT_CLASS(klass);

  dev_class->create_element = gst_vimbasrc_device_create_element;
  dev_class->reconfigure_element = gst_vimbasrc_device_reconfigure_element;

  object_class->get_property = gst_vimbasrc_device_get_property;
  object_class->set_property = gst_vimbasrc_device_set_property;
  object_class->finalize = gst_vimbasrc_device_finalize;

  g_object_class_install_property(object_class, PROP_CAMERA_ID,
      g_param_spec_string("camera-id",
                          "Camera Id",
                          "Unique identifier for each camera",
                          NULL,
                          (G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY)));
  g_object_class_install_property(object_class, PROP_CAMERA_NAME,
      g_param_spec_string("camera-name",
                          "Camera Name",
                          "Name of the camera",
                          NULL,
                          (G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY)));
  g_object_class_install_property(object_class, PROP_CAMERA_MODEL,
      g_param_spec_string("camera-model",
                          "Camera Model Id",
                          "The camera model name",
                          NULL,
                          (G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY)));
  g_object_class_install_property(object_class, PROP_SERIAL,
      g_param_spec_string("serial",
                          "Camera Serial String",
                          "The serial number of the camera",
                          NULL,
                          (G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY)));
  g_object_class_install_property(object_class, PROP_INTERFACE_ID,
      g_param_spec_string("interface-id",
                          "Interface Id String",
                          "Unique value for each interface or bus",
                          NULL,
                          (G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY)));
}

/**
 * @brief GLib function called to initialize a VimbaSrcDevice instance
 *
 * @param device
 */
static void
gst_vimbasrc_device_init (GstVimbaSrcDevice *device)
{
  GST_TRACE_OBJECT(device, "gst-vimbasrc-device init");
  device->element = "vimbasrc";
}
