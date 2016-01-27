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
#include "mtp_daemon_controller.h"

#include "mtp_daemon_gdbus.h"
#include "mtp_daemon_gdbus_manager.h"
#include "mtp_daemon_gdbus_deviceinfo.h"
#include "mtp_daemon_gdbus_storageinfo.h"
#include "mtp_daemon_gdbus_objectinfo.h"

static gboolean __manager_init(mtp_context *mtp_ctx)
{
	gboolean ret = FALSE;
	GError *error = NULL;

	GDBusObjectManagerServer *manager;
	mtpgdbuslibManager *manager_skeleton;

	/* Add interface to default object path */
	manager_skeleton = mtp_gdbuslib_manager_skeleton_new();

	/* Register for method callbacks as signal callbacks */
	g_signal_connect(
			manager_skeleton,
			"handle-get-devices",
			G_CALLBACK(manager_get_devices),
			mtp_ctx);

	g_signal_connect(
			manager_skeleton,
			"handle-get-storages",
			G_CALLBACK(manager_get_storages),
			mtp_ctx);

	g_signal_connect(
			manager_skeleton,
			"handle-get-object-handles",
			G_CALLBACK(manager_get_object_handles),
			mtp_ctx);

	g_signal_connect(
			manager_skeleton,
			"handle-get-object",
			G_CALLBACK(manager_get_object),
			mtp_ctx);

	g_signal_connect(
			manager_skeleton,
			"handle-get-thumbnail",
			G_CALLBACK(manager_get_thumbnail),
			mtp_ctx);

	g_signal_connect(
			manager_skeleton,
			"handle-delete-object",
			G_CALLBACK(manager_delete_object),
			mtp_ctx);

	manager = g_dbus_object_manager_server_new(MTP_DBUS_MANAGER_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, mtp_ctx->connection);

	/* Export 'manager' interface on MTP DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(manager_skeleton),
		mtp_ctx->connection, MTP_DBUS_MANAGER_PATH, &error);

	if (ret == FALSE) {
		MTP_LOGE("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	mtp_ctx->manager = manager_skeleton;

	return ret;
}

static gboolean __deviceinfo_init(mtp_context *mtp_ctx)
{
	gboolean ret = FALSE;
	GError *error = NULL;

	GDBusObjectManagerServer *deviceinfo;
	mtpgdbuslibDeviceinfo *deviceinfo_skeleton;

	/* Add interface to default object path */
	deviceinfo_skeleton = mtp_gdbuslib_deviceinfo_skeleton_new();

	/* Register for method callbacks as signal callbacks */
	g_signal_connect(
			deviceinfo_skeleton,
			"handle-get-manufacturer-name",
			G_CALLBACK(deviceinfo_get_manufacturer_name),
			mtp_ctx);

	g_signal_connect(
			deviceinfo_skeleton,
			"handle-get-model-name",
			G_CALLBACK(deviceinfo_get_model_name),
			mtp_ctx);

	g_signal_connect(
			deviceinfo_skeleton,
			"handle-get-serial-number",
			G_CALLBACK(deviceinfo_get_serial_number),
			mtp_ctx);

	g_signal_connect(
			deviceinfo_skeleton,
			"handle-get-device-version",
			G_CALLBACK(deviceinfo_get_device_version),
			mtp_ctx);

	g_signal_connect(
			deviceinfo_skeleton,
			"handle-get-bus-location",
			G_CALLBACK(deviceinfo_get_bus_location),
			mtp_ctx);

	g_signal_connect(
			deviceinfo_skeleton,
			"handle-get-device-number",
			G_CALLBACK(deviceinfo_get_device_number),
			mtp_ctx);

	deviceinfo = g_dbus_object_manager_server_new(MTP_DBUS_DEVICEINFO_PATH);

	/* Set connection to 'deviceinfo' */
	g_dbus_object_manager_server_set_connection(deviceinfo, mtp_ctx->connection);

	/* Export 'deviceinfo' interface on MTP DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(deviceinfo_skeleton),
		mtp_ctx->connection, MTP_DBUS_DEVICEINFO_PATH, &error);

	if (ret == FALSE) {
		MTP_LOGE("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return ret;
}

static gboolean __storageinfo_init(mtp_context *mtp_ctx)
{
	gboolean ret = FALSE;
	GError *error = NULL;

	GDBusObjectManagerServer *storageinfo;
	mtpgdbuslibStorageinfo *storageinfo_skeleton;

	/* Add interface to default object path */
	storageinfo_skeleton = mtp_gdbuslib_storageinfo_skeleton_new();

	/* Register for method callbacks as signal callbacks */

	g_signal_connect(
			storageinfo_skeleton,
			"handle-get-description",
			G_CALLBACK(storageinfo_get_description),
			mtp_ctx);

	g_signal_connect(
			storageinfo_skeleton,
			"handle-get-free-space",
			G_CALLBACK(storageinfo_get_freespace),
			mtp_ctx);

	g_signal_connect(
			storageinfo_skeleton,
			"handle-get-max-capacity",
			G_CALLBACK(storageinfo_get_maxcapacity),
			mtp_ctx);

	g_signal_connect(
			storageinfo_skeleton,
			"handle-get-storage-type",
			G_CALLBACK(storageinfo_get_storagetype),
			mtp_ctx);

	g_signal_connect(
			storageinfo_skeleton,
			"handle-get-volume-identifier",
			G_CALLBACK(storageinfo_get_volumeidentifier),
			mtp_ctx);

	storageinfo = g_dbus_object_manager_server_new(MTP_DBUS_STORAGEINFO_PATH);

	/* Set connection to 'storageinfo' */
	g_dbus_object_manager_server_set_connection(storageinfo, mtp_ctx->connection);

	/* Export 'storageinfo' interface on MTP DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(storageinfo_skeleton),
		mtp_ctx->connection, MTP_DBUS_STORAGEINFO_PATH, &error);

	if (ret == FALSE) {
		MTP_LOGE("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return ret;
}

static gboolean __objectinfo_init(mtp_context *mtp_ctx)
{
	gboolean ret = FALSE;
	GError *error = NULL;

	GDBusObjectManagerServer *objectinfo;
	mtpgdbuslibObjectinfo *objectinfo_skeleton;

	/* Add interface to default object path */
	objectinfo_skeleton = mtp_gdbuslib_objectinfo_skeleton_new();

	/* Register for method callbacks as signal callbacks */
	g_signal_connect(
			objectinfo_skeleton,
			"handle-get-property",
			G_CALLBACK(objectinfo_get_property),
			mtp_ctx);

	g_signal_connect(
			objectinfo_skeleton,
			"handle-get-property-string",
			G_CALLBACK(objectinfo_get_property_string),
			mtp_ctx);

	objectinfo = g_dbus_object_manager_server_new(MTP_DBUS_OBJECTINFO_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(objectinfo, mtp_ctx->connection);

	/* Export 'objectinfo' interface on MTP DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(objectinfo_skeleton),
		 mtp_ctx->connection, MTP_DBUS_OBJECTINFO_PATH, &error);

	if (ret == FALSE) {
		MTP_LOGE("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return ret;
}

static void on_bus_acquired(GDBusConnection *connection,
			const gchar *path,
			gpointer user_data)
{
	mtp_context* mtp_ctx = (mtp_context*)user_data;

	MTP_LOGI("path: %s", path);

	mtp_ctx->connection = connection;

	if (__manager_init(mtp_ctx) == FALSE) {
		MTP_LOGE("Can not signal connect");
		/* Deinitialize and quit daemon */
	}
	if (__deviceinfo_init(mtp_ctx) == FALSE) {
		MTP_LOGE("Can not signal connect");
		/* Deinitialize and quit daemon */
	}
	if (__storageinfo_init(mtp_ctx) == FALSE) {
		MTP_LOGE("Can not signal connect");
		/* Deinitialize and quit daemon */
	}
	if (__objectinfo_init(mtp_ctx) == FALSE) {
		MTP_LOGE("Can not signal connect");
		/* Deinitialize and quit daemon */
	}
}

static void on_name_acquired(GDBusConnection *connection,
			const gchar *name,
			gpointer user_data)
{
	mtp_context* mtp_ctx = (mtp_context*)user_data;

	MTP_LOGI("name: %s", name);

	if (mtp_daemon_event_init(mtp_ctx) != MTP_ERROR_NONE)
		g_main_loop_quit(mtp_ctx->main_loop);
}

static void on_name_lost(GDBusConnection *connnection,
			const gchar *name,
			gpointer user_data)
{
	MTP_LOGI("name : %s", name);
}

mtp_error_e mtp_daemon_gdbus_init(mtp_context *mtp_ctx)
{
	mtp_ctx->bus_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
		MTP_DBUS_SERVICE,
		G_BUS_NAME_OWNER_FLAGS_NONE,
		on_bus_acquired,
		on_name_acquired,
		on_name_lost,
		mtp_ctx,
		NULL);

	MTP_LOGI("mtp_ctx->bus_id: %d", mtp_ctx->bus_id);

	return MTP_ERROR_NONE;
}

mtp_error_e mtp_daemon_gdbus_emit_event(mtp_event event, int arg1, mtp_context *mtp_ctx)
{
	MTP_LOGI("signal emit event: %d, arg1: %d", event, arg1);

	mtp_gdbuslib_manager_emit_mtp_event(mtp_ctx->manager, event, arg1);

	return MTP_ERROR_NONE;
}
