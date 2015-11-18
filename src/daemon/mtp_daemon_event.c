/*
 * Copyright (c) 2012, 2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mtp_daemon_event.h"
#include "mtp_daemon_gdbus.h"
#include "mtp_daemon_db.h"

#define DEVICE_MAX 5

#define DEVICED_BUS_NAME			"org.tizen.system.deviced"
#define DEVICED_OBJECT_PATH			"/Org/Tizen/System/DeviceD"
#define DEVICED_INTERFACE_NAME		DEVICED_BUS_NAME

#define DEVICED_PATH_USBHOST		DEVICED_OBJECT_PATH"/Usbhost"
#define DEVICED_INTERFACE_USBHOST	DEVICED_INTERFACE_NAME".Usbhost"

#define SIGNAL_USB_HOST_CHANGED "ChangedDevice"

static GDBusConnection *g_usb_bus;
static int g_usb_handler;

typedef enum {
	USB_HOST_REMOVED = 0,
	USB_HOST_ADDED = 1,
	USB_MAX,
} usbhost_state;

typedef void (*device_changed_cb)(const char *dev_path, int bus_no, usbhost_state host_status, void *user_data);

typedef struct _device_cb_data device_cb_data;

struct _device_cb_data {
	device_changed_cb usr_cb;
	void *usr_data;
};

device_cb_data *g_usb_cb_data = NULL;

static void __print_device_list(mtp_context *mtp_ctx)
{
	GList *l;
	int i = 0;
	MTP_LOGE(" ");
	MTP_LOGE("<------print list------->");

	for (l = g_list_first(mtp_ctx->device_list->device_info_list); l != NULL; l = l->next) {
		i++;
		mtp_device_info *device_info;
		device_info = (mtp_device_info *)l->data;

		MTP_LOGE("%d. bus %d , %s", i, device_info->bus_location, device_info->model_name);
	}

	MTP_LOGE("<------print list end------->");
	MTP_LOGE(" ");
}

static int __parsing_usb_busno(const char *devpath)
{
	int busno = -1;
	char *pos = NULL;

	pos = strrchr(devpath, '/');

	if (pos != NULL)
		busno = atoi(&pos[1]);
	else
		MTP_LOGE("pos is NULL");

	MTP_LOGI("busno : %d, pos : %s, devpath : %s", busno, pos, devpath);

	return busno;
}

static void __dbus_usb_host_event_cb(GDBusConnection* connection,
					const gchar* sender_name,
					const gchar* object_path,
					const gchar* interface_name,
					const gchar* signal_name,
					GVariant* parameters,
					gpointer user_data)
{
	GVariant *tmp;
	const char *devpath = NULL;
	int state;
	int vendorid;
	int productid;
	int busno = 0;

	int baseclass;
	int subclass;
	int protocol;
	const char *manufacturer = NULL;
	const char *product = NULL;
	const char *serial = NULL;

	gsize size = 0;
	device_cb_data *cb_data = (device_cb_data *)user_data;
	void *usr_cb = cb_data->usr_cb;
	void *usr_data = cb_data->usr_data;

	tmp = g_variant_get_child_value(parameters, 0);
	state = g_variant_get_int32(tmp);
	MTP_LOGI("state : %d", state);

	tmp = g_variant_get_child_value(parameters, 1);
	devpath = g_variant_get_string(tmp, &size);
	MTP_LOGI("devpath : %s", devpath);

	tmp = g_variant_get_child_value(parameters, 2);
	baseclass = g_variant_get_int32(tmp);
	MTP_LOGI("baseclass : %d", baseclass);

	tmp = g_variant_get_child_value(parameters, 3);
	subclass = g_variant_get_int32(tmp);
	MTP_LOGI("subclass : %d", subclass);

	tmp = g_variant_get_child_value(parameters, 4);
	protocol = g_variant_get_int32(tmp);
	MTP_LOGI("protocol : %d", protocol);

	tmp = g_variant_get_child_value(parameters, 5);
	vendorid = g_variant_get_int32(tmp);
	MTP_LOGI("vendorid : %d", vendorid);

	tmp = g_variant_get_child_value(parameters, 6);
	productid = g_variant_get_int32(tmp);
	MTP_LOGI("productid : %d", productid);

	tmp = g_variant_get_child_value(parameters, 7);
	manufacturer = g_variant_get_string(tmp, &size);
	MTP_LOGI("manufacturer : %s", manufacturer);

	tmp = g_variant_get_child_value(parameters, 8);
	product = g_variant_get_string(tmp, &size);
	MTP_LOGI("product : %s", product);

	tmp = g_variant_get_child_value(parameters, 9);
	serial = g_variant_get_string(tmp, &size);
	MTP_LOGI("serial : %s", serial);

	busno = __parsing_usb_busno(devpath);

	if (busno >= 0)
		((device_changed_cb)usr_cb)(devpath, busno, state, usr_data);

	MTP_LOGI("user callback done");
}

static int __dbus_subscribe_usb_host_event(device_changed_cb usr_callback, void *usr_data)
{
	int ret = MTP_ERROR_NONE;
	GError *error = NULL;

	MTP_LOGE("BEGIN");

	g_usb_cb_data = malloc(sizeof(device_cb_data));
	if (g_usb_cb_data == NULL) {
		MTP_LOGE("malloc failed");
		return MTP_ERROR_ALLOC_FAIL;
	}

	g_usb_cb_data->usr_cb = usr_callback;
	g_usb_cb_data->usr_data = usr_data;

	if (g_usb_bus == NULL) {
		g_usb_bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
		if (!g_usb_bus) {
			MTP_LOGE("Failed to connect to the g D-BUS daemon: %s", error->message);
			g_error_free(error);
			ret = MTP_ERROR_PLUGIN;
			goto ERROR;
		}
	}

	/* listening to messages from all objects as no path is specified */
	g_usb_handler = g_dbus_connection_signal_subscribe(
					g_usb_bus,
					NULL,
					DEVICED_INTERFACE_USBHOST,
					SIGNAL_USB_HOST_CHANGED,
					NULL,
					NULL,
					G_DBUS_SIGNAL_FLAGS_NONE,
					__dbus_usb_host_event_cb,
					g_usb_cb_data,
					NULL);

	MTP_LOGE("END");

	return MTP_ERROR_NONE;

ERROR:

	if (g_usb_bus != NULL) {
		g_object_unref(g_usb_bus);
		g_usb_bus = NULL;
	}

	g_free(g_usb_cb_data);

	MTP_LOGE("ERROR");

	return ret;
}

void __usb_host_status_changed_cb(const char *dev_path, int bus_no, usbhost_state host_status, void *user_data)
{
	int i;
	int num_of_devices;
	mtp_context *mtp_ctx = (mtp_context *)user_data;
	LIBMTP_raw_device_t *raw_devices;
	LIBMTP_mtpdevice_t *device;
	mtp_device_info *device_info;

	MTP_LOGE("host_status : %d, bus_no : %d, dev_path : %s", host_status, bus_no, dev_path);

	if (host_status == USB_HOST_ADDED) {
		LIBMTP_Detect_Raw_Devices(&raw_devices, &num_of_devices);
		for (i = 0; i < num_of_devices; i++) {
			if (bus_no == raw_devices[i].bus_location) {
				MTP_LOGE("connected bus_no : %d", bus_no);
				device_info = g_new0(mtp_device_info, 1);
				device = LIBMTP_Open_Raw_Device_Uncached(&raw_devices[i]);

				if (device == NULL) {
					MTP_LOGE("Unable to open raw device[%d]", i);
					continue;
				}

				device_info->device = device;
				device_info->bus_location = raw_devices[i].bus_location;
				device_info->model_name = LIBMTP_Get_Modelname(device);

				mtp_ctx->device_list->device_info_list
					= g_list_prepend(mtp_ctx->device_list->device_info_list, device_info);
				mtp_ctx->device_list->device_num++;

				g_thread_pool_push(mtp_ctx->device_list->threads, (gpointer)device, NULL);

				mtp_daemon_gdbus_emit_event(MTP_INITIATOR_EVENT_DEVICE_ADDED, device, mtp_ctx);
			}
		}
		g_free(raw_devices);
		__print_device_list(mtp_ctx);
	} else if (host_status == USB_HOST_REMOVED) {
		MTP_LOGE("disconnected bus_no : %d", bus_no);
	}
}

static void* __event_thread(gpointer dev, gpointer data)
{
	int ret;
	LIBMTP_mtpdevice_t *device = (LIBMTP_mtpdevice_t *)dev;
	mtp_context *mtp_ctx = (mtp_context *)data;
	LIBMTP_event_t device_event;
	uint32_t param1;
	GList *l;

	MTP_LOGI("Event loop is started");

	mtp_daemon_gdbus_emit_event(MTP_INITIATOR_EVENT_DEVICE_ADDED, device, mtp_ctx);

	while (LIBMTP_Read_Event(device, &device_event, &param1) == 0) {
		MTP_LOGI("device %d device_event is occured %d", device, device_event);

		if (device_event == LIBMTP_EVENT_STORE_ADDED) {
			LIBMTP_Get_Storage(device, LIBMTP_STORAGE_SORTBY_NOTSORTED);
		} else if (device_event == LIBMTP_EVENT_STORE_REMOVED) {
			mtp_daemon_db_delete((int)device, param1, 0, mtp_ctx);
			LIBMTP_Get_Storage(device, LIBMTP_STORAGE_SORTBY_NOTSORTED);
		} else if (device_event == LIBMTP_EVENT_OBJECT_REMOVED) {
			mtp_daemon_db_delete((int)device, 0, param1, mtp_ctx);
		} else if (device_event == LIBMTP_EVENT_OBJECT_ADDED) {
			MTPObjectInfo *object_info;
			object_info = LIBMTP_Get_Object_Info(device, param1);

			/* scan db and if exist, then update db */
			if (object_info != NULL) {
				if (mtp_daemon_db_is_exist((int)device, param1, mtp_ctx) == true) {
					MTP_LOGI("DB Update");
					mtp_daemon_db_update((int)device, param1, object_info, mtp_ctx);
				} else {
					MTP_LOGI("DB Insert");
					mtp_daemon_db_insert((int)device, object_info->StorageID, param1, object_info, mtp_ctx);
				}
			}
		}

		mtp_daemon_gdbus_emit_event(device_event, param1, mtp_ctx);
	}

	MTP_LOGI("device is closing down!!");

	mtp_daemon_gdbus_emit_event(MTP_INITIATOR_EVENT_DEVICE_REMOVED, device, mtp_ctx);

	/* remove the device into device list */
	for (l = g_list_first(mtp_ctx->device_list->device_info_list); l != NULL; l = l->next) {
		mtp_device_info *device_info;
		device_info = l->data;

		if (device_info->device == device) {
			MTP_LOGI("remove device from the list!!");

			mtp_ctx->device_list->device_info_list
				= g_list_remove(mtp_ctx->device_list->device_info_list, device_info);
			mtp_ctx->device_list->device_num--;

			LIBMTP_Release_Device(device_info->device);
			mtp_daemon_db_delete((int)device, 0, 0, mtp_ctx);

			g_free(device_info->model_name);
			g_free(device_info);
		}
	}

	/* TODO : deactivate signal to clients using g_idle_add */
	__print_device_list(mtp_ctx);

	if (mtp_ctx->device_list->device_num == 0) {
		mtp_daemon_gdbus_emit_event(MTP_INITIATOR_EVENT_DAEMON_TERMINATED, device, mtp_ctx);
		g_main_loop_quit(mtp_ctx->main_loop);
	}

	return NULL;
}

mtp_error_e __device_list_init(mtp_context *mtp_ctx)
{
	mtp_error_e ret = MTP_ERROR_NONE;
	LIBMTP_raw_device_t *rawdevices;
	int i, numrawdevices = 0;
	char *tmp_name = "Unknown Device";

	RETV_IF(mtp_ctx == NULL, MTP_ERROR_INVALID_PARAMETER);

	LIBMTP_Init();
	LIBMTP_Detect_Raw_Devices(&rawdevices, &numrawdevices);
	MTP_LOGE("detected device num %d", numrawdevices);

	if (numrawdevices == 0) {
		MTP_LOGE("MTP Host have not any mtp device");
		g_free(rawdevices);
		return MTP_ERROR_NO_DEVICE;
	}

	for (i = 0; i < numrawdevices; i++) {
		LIBMTP_mtpdevice_t *device;
		mtp_device_info *device_info;

		device_info = g_new0(mtp_device_info, 1);

		device = LIBMTP_Open_Raw_Device_Uncached(&rawdevices[i]);
		if (device == NULL) {
			MTP_LOGE("Unable to open raw device[%d]", i);
			continue;
		}

		device_info->device = device;
		device_info->bus_location = rawdevices[i].bus_location;
		device_info->model_name = LIBMTP_Get_Modelname(device);

		if (device_info->model_name == NULL) {
			MTP_LOGE("Device: (NULL)");
			device_info->model_name = strdup(tmp_name);
		}
		MTP_LOGI("Device: %s, Bus: %d", device_info->model_name, device_info->bus_location);

		mtp_ctx->device_list->device_info_list
			= g_list_prepend(mtp_ctx->device_list->device_info_list, device_info);
		mtp_ctx->device_list->device_num++;
	}
	g_free(rawdevices);

	return ret;
}

int usb_host_set_event_cb(device_changed_cb usr_callback, void *usr_data)
{
	int ret = MTP_ERROR_NONE;

	MTP_LOGE();

	ret = __dbus_subscribe_usb_host_event(usr_callback, usr_data);

	return ret;
}

int usb_host_unset_event_cb(void)
{
	if (g_usb_bus == NULL)
		return MTP_ERROR_NONE;

	g_dbus_connection_signal_unsubscribe(g_usb_bus, g_usb_handler);
	g_object_unref(g_usb_bus);
	g_usb_bus = NULL;

	/*Release Callback*/
	g_free(g_usb_cb_data);

	return MTP_ERROR_NONE;
}

mtp_error_e mtp_daemon_event_init(mtp_context *mtp_ctx)
{
	GList *l;
	mtp_error_e ret = MTP_ERROR_NONE;

	RETV_IF(mtp_ctx == NULL, MTP_ERROR_INVALID_PARAMETER);

	/* mtp device list init */
	__device_list_init(mtp_ctx);

	/* usb event setting */
	usb_host_set_event_cb(__usb_host_status_changed_cb, mtp_ctx);

	/* create thread pool */
	mtp_ctx->device_list->threads
		= g_thread_pool_new((GFunc) __event_thread, mtp_ctx, DEVICE_MAX, TRUE, NULL);

	/* create thread each mtp device */
	for (l = g_list_first(mtp_ctx->device_list->device_info_list); l != NULL; l = l->next) {
		mtp_device_info *device_info;
		device_info = l->data;

		g_thread_pool_push(mtp_ctx->device_list->threads,
			(gpointer) device_info->device, NULL);
	}

	MTP_LOGE("now, number of devices and thread is %d", mtp_ctx->device_list->device_num);

	if (mtp_ctx->device_list->device_num == 0)
		return MTP_ERROR_NO_DEVICE;

	return ret;
}
