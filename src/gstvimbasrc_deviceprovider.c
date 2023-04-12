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

#include "gstvimbasrc_deviceprovider.h"
#include "gstvimbasrc_device.h"
#include "gstvimbasrc.h"

#include <gst/gst.h>
#include <gst/video/video-format.h>
#include <VimbaC/Include/VimbaC.h>

#ifdef WIN32
    #define UNUSED_ATTR(x) x
#else
    #define UNUSED_ATTR(x) x __attribute__((unused))
#endif

G_DEFINE_TYPE(GstVimbaSrcDeviceProvider, gst_vimbasrc_device_provider, GST_TYPE_DEVICE_PROVIDER)

/**
 * @brief Create a GstDevice from Vimba camera info object
 *
 * @param camera A camera information pointer from Vimba API (caller owns)
 * @return GstDevice* Created GstDevice (transfer ownership to caller)
 */
GstDevice *
create_gstdevice_from_vmbcamerainfo(VmbCameraInfo_t* camera)
{
  gchar *display_name = g_strdup_printf("%s (id=%s)", camera->cameraName, camera->cameraIdString);
  GstCaps *caps = gst_caps_from_string(GST_VIDEO_CAPS_MAKE(GST_VIDEO_FORMATS_ALL));
  gst_caps_append(caps, gst_caps_from_string(GST_BAYER_CAPS_MAKE(GST_BAYER_FORMATS_ALL)));

  GstVimbaSrcDevice *device = g_object_new(GST_TYPE_VIMBASRC_DEVICE,
                                           "display-name", display_name,
                                           "device-class", "Video/Source",
                                           "caps", caps,
                                           "camera-id", camera->cameraIdString,
                                           "camera-model", camera->modelName,
                                           "camera-name", camera->cameraName,
                                           "interface-id", camera->interfaceIdString,
                                           "serial", camera->serialString,
                                           NULL);
  g_free(display_name);
  gst_caps_unref(caps);

  return GST_DEVICE(device);
}

/**
 * @brief Callback for Vimba API on camera discovery events
 *
 * @param handle Vimba handle the event was emitted for
 * @param name Event name
 * @param context User context (GstDeviceProvider*)
 */
void VMB_CALL
callback_vimba_camera_discovery(VmbHandle_t handle, const char* name, void* context)
{
  GST_TRACE("Vimba callback: %s", name);
  VmbError_t err = VmbErrorSuccess;
  GstDeviceProvider *self = GST_DEVICE_PROVIDER(context);

  char camera_id[255];
  const char* callback_reason = NULL;

  VmbFeatureEnumGet(handle, "DiscoveryCameraEvent", &callback_reason);
  VmbFeatureStringGet(handle, "DiscoveryCameraIdent", camera_id, 255, NULL);

  if (strcmp(callback_reason, "Detected") == 0)
  {
    VmbCameraInfo_t camera;
    err = VmbCameraInfoQuery(camera_id, &camera, sizeof camera);
    if (err != VmbErrorSuccess)
    {
      GST_ERROR_OBJECT(self, "Could not retrieve camera information.");
      return;
    }

    GST_DEBUG_OBJECT(self, "Adding new %s device (id: %s)", camera.cameraName, camera.cameraIdString);

    GstDevice *device = create_gstdevice_from_vmbcamerainfo(&camera);
    gst_device_provider_device_add(self, device);
  }
  else if (strcmp(callback_reason, "Missing") == 0)
  {
    for (GList *item = self->devices; item; item = item->next)
    {
      GstVimbaSrcDevice *device = GST_VIMBASRC_DEVICE(item->data);
      if (strcmp(device->camera_id, camera_id) == 0)
      {
        gst_device_provider_device_remove(self, GST_DEVICE(device));
        break;
      }
    }
  }
  else
  {
    GST_DEBUG_OBJECT(self, "Unhandled option %s", callback_reason);
    // currently noop (Reachable, Unreachable)
  }
}

static void
gst_vimbasrc_device_provider_set_property (GObject *object, guint property_id, const GValue * UNUSED_ATTR(value), GParamSpec *pspec)
{
  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gst_vimbasrc_device_provider_get_property (GObject *object, guint property_id, GValue * UNUSED_ATTR(value), GParamSpec *pspec)
{
  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

/**
 * @brief GStreamer function called to query all connected devices
 *
 * Use the Vimba API to query all connected cameras
 *
 * @param provider GstVimbaSrcDeviceProvider
 * @return GList*
 */
static GList *
gst_vimbasrc_device_provider_probe (GstDeviceProvider *provider)
{
  GstVimbaSrcDeviceProvider *self = GST_VIMBASRC_DEVICE_PROVIDER(provider);

  VmbError_t      err      = VmbErrorSuccess;
  VmbCameraInfo_t *cameras = NULL;
  VmbUint32_t     count    = 0;
  VmbUint32_t     found    = 0;

  // We can not probe for GigE cameras, because that would be blocking (not allowed in _probe)
  GST_DEBUG_OBJECT(self, "Starting probe vimba");

  self->devices = NULL;

  // number of known cameras
  err = VmbCamerasList( NULL, 0, &count, sizeof *cameras);
  if (VmbErrorSuccess == err && count != 0)
  {
    GST_TRACE_OBJECT(self, "Cameras found: %d", count);

    cameras = (VmbCameraInfo_t*)malloc(sizeof *cameras * count);
    if (cameras == NULL)
    {
      GST_ERROR_OBJECT(self, "Could not allocate camera list.");
      return NULL;
    }

    // Query all static details of all known cameras
    err = VmbCamerasList(cameras, count, &found, sizeof *cameras);
    if(VmbErrorSuccess == err ||  VmbErrorMoreData == err)
    {
      if(found < count)
      {
        count = found;
      }

      for (VmbUint32_t i = 0; i < count; ++i)
      {
        gst_device_provider_device_add(provider, create_gstdevice_from_vmbcamerainfo(cameras + i));
      }
    }
    else
    {
      GST_ERROR_OBJECT(self, "Could not retrieve camera list.");
    }
    free(cameras);
  }
  else if (VmbErrorSuccess != err)
  {
    GST_ERROR_OBJECT(self, "Could not list cameras.");
    return NULL;
  }

  return self->devices;
}

/**
 * @brief GStreamer function used to start listening for changed devices
 *
 * Registers the Vimba camera discovery event handler
 *
 * @param provider GstVimbaSrcDeviceProvider
 * @return True if the event handler could be registered
 */
static gboolean
gst_vimbasrc_device_provider_start(GstDeviceProvider *provider)
{
  GstVimbaSrcDeviceProvider *self = GST_VIMBASRC_DEVICE_PROVIDER(provider);

  GST_DEBUG_OBJECT(self, "Starting vimba device provider");

  VmbError_t err = VmbFeatureInvalidationRegister(gVimbaHandle, "DiscoveryCameraEvent", callback_vimba_camera_discovery, self);
  if (err != VmbErrorSuccess) {
    GST_ERROR_OBJECT(self, "Starting vimba device provider failed.");
    return FALSE;
  }

  /* The base class does not call _probe so only added devices will be shown in gst-device-monitor-1.0, this is workaround. */
  gst_vimbasrc_device_provider_probe(provider);

  return TRUE;
}

/**
 * @brief GStreamer function used to stop listening for changed devices
 *
 * Unregisters the Vimba camera discovery event handler
 *
 * @param provider GstVimbaSrcDeviceProvider
 */
static void
gst_vimbasrc_device_provider_stop(GstDeviceProvider *provider)
{
  GstVimbaSrcDeviceProvider *self = GST_VIMBASRC_DEVICE_PROVIDER(provider);

  GST_DEBUG_OBJECT (self, "stopping provider");

  VmbError_t err = VmbFeatureInvalidationUnregister(gVimbaHandle, "DiscoveryCameraEvent", callback_vimba_camera_discovery);
  if (err != VmbErrorSuccess) {
    GST_WARNING_OBJECT(self, "Stopping vimba device provider failed.");
  }
}

/**
 * @brief GLib function to free a GstVimbaSrcDeviceProvider
 *
 * Used to release the Vimba API if needed
 *
 * @param object The GstVimbaSrcDeviceProvider to free
 */
static void
gst_vimbasrc_device_provider_finalize(GObject *object)
{
  GstVimbaSrcDeviceProvider *self = GST_VIMBASRC_DEVICE_PROVIDER(object);

  stop_vimba(GST_OBJECT(self));

  G_OBJECT_CLASS(gst_vimbasrc_device_provider_parent_class)->finalize(object);
}

/**
 * @brief GLib function to initialize the GstVimbaSrcDeviceProvider class struct
 *
 * Used to register out start / stop / probe functions.
 *
 * @param klass
 */
static void
gst_vimbasrc_device_provider_class_init(GstVimbaSrcDeviceProviderClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GstDeviceProviderClass *dp_class = GST_DEVICE_PROVIDER_CLASS(klass);

  gobject_class->set_property = gst_vimbasrc_device_provider_set_property;
  gobject_class->get_property = gst_vimbasrc_device_provider_get_property;
  gobject_class->finalize = gst_vimbasrc_device_provider_finalize;

  dp_class->probe = gst_vimbasrc_device_provider_probe;
  dp_class->start = gst_vimbasrc_device_provider_start;
  dp_class->stop = gst_vimbasrc_device_provider_stop;

  gst_device_provider_class_set_static_metadata (
    dp_class,
    "VimbaSrc Device Provider",
    "Source/Video",
    "List and provide Vimba camera devices",
    "Allied Vision Technologies GmbH"
  );
}

/**
 * @brief GLib function to initialize a GstVimbaSrcDeviceProvider instance
 *
 * Used to start the Vimba API if needed
 *
 * @param self The GstVimbaSrcDeviceProvider to initialize
 */
static void
gst_vimbasrc_device_provider_init(GstVimbaSrcDeviceProvider *self)
{
  GST_TRACE_OBJECT(self, "gst-vimbasrc-device-provider init");
  start_vimba(GST_OBJECT(self));
}
