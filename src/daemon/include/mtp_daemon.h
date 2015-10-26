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

#ifndef __MTP_DAEMON_H__
#define __MTP_DAEMON_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sqlite3.h>

#include <glib.h>
#include <gio/gio.h>

/* tizen specific header */
#include <tizen.h>
#include <dlog.h>
#include <libmtp.h>
#include "mtp_gdbuslib.h"

#define MTP_DBUS_SERVICE	"org.tizen.mtp"
#define MTP_DBUS_MANAGER_PATH		"/org/tizen/mtp/manager"
#define MTP_DBUS_DEVICEINFO_PATH		"/org/tizen/mtp/deviceinfo"
#define MTP_DBUS_STORAGEINFO_PATH		"/org/tizen/mtp/storageinfo"
#define MTP_DBUS_OBJECTINFO_PATH		"/org/tizen/mtp/objectinfo"

#define DLOG_TAG "MTP_INITIATOR"

#define MTP_LOGD(format, arg...) LOG(LOG_ERROR, DLOG_TAG, format, ##arg)
#define MTP_LOGI(format, arg...) LOG(LOG_ERROR, DLOG_TAG, format, ##arg)
#define MTP_LOGW(format, arg...) LOG(LOG_ERROR, DLOG_TAG, format, ##arg)
#define MTP_LOGE(format, arg...) LOG(LOG_ERROR, DLOG_TAG, format, ##arg)

#define MTP_ERROR_CLASS TIZEN_ERROR_NETWORK_CLASS /* Need define mtp error class */

#define RETV_IF(expr, val) \
	do { \
		if (expr) { \
			MTP_LOGE("(%s) return", #expr); \
			return (val); \
		} \
	} while (0)

#define MTP_DB_FILE "/tmp/.mtp.db"
#define MTP_DB_TABLE "mtp_object_info"

typedef void (*mtp_controller_func)(gpointer user_data);

/* enum */
typedef enum {
	MTP_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
	MTP_ERROR_GENERAL = MTP_ERROR_CLASS | 0x01, /**< A general error occurred */
	MTP_ERROR_IO_ERROR = TIZEN_ERROR_IO_ERROR, /**< I/O error */
	MTP_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	MTP_ERROR_NO_DEVICE = MTP_ERROR_CLASS | 0x02, /**< MTP have not any device */
	MTP_ERROR_ALLOC_FAIL = MTP_ERROR_CLASS | 0x03, /**< Memory Allocation failed */
	MTP_ERROR_PLUGIN = MTP_ERROR_CLASS | 0x04, /**< Plugin failed */
	MTP_ERROR_DB = MTP_ERROR_CLASS | 0x05, /**< Plugin failed */
	MTP_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
	MTP_ERROR_NOT_INITIALIZED = MTP_ERROR_CLASS | 0x06, /**< MTP is not supported */
	MTP_ERROR_NOT_ACTIVATED = MTP_ERROR_CLASS | 0x07, /**< MTP is not activated */
	MTP_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED, /**< MTP is not supported */
} mtp_error_e;

typedef enum {
	MTP_PROPERTY_ASSOCIATION_DESC = 1,
	MTP_PROPERTY_ASSOCIATION_TYPE,
	MTP_PROPERTY_SIZE,
	MTP_PROPERTY_DATA_CREATED,
	MTP_PROPERTY_DATA_MODIFIED,
	MTP_PROPERTY_FORMAT,
	MTP_PROPERTY_IMAGE_FIX_DEPTH,
	MTP_PROPERTY_IMAGE_FIX_WIDTH,
	MTP_PROPERTY_IMAGE_FIX_HEIGHT,
	MTP_PROPERTY_PARENT_OBJECT_HANDLE,
	MTP_PROPERTY_STORAGE_ID,
	MTP_PROPERTY_THUMBNAIL_SIZE,
	MTP_PROPERTY_THUMBNAIL_FORMAT,
	MTP_PROPERTY_THUMBNAIL_WIDTH,
	MTP_PROPERTY_THUMBNAIL_HEIGHT,
	MTP_PROPERTY_FILENAME,
	MTP_PROPERTY_KEYWORD
} mtp_property_e;

typedef enum {
	MTP_INITIATOR_EVENT_NONE,
	MTP_INITIATOR_EVENT_STORAGE_ADDED,
	MTP_INITIATOR_EVENT_STORAGE_REMOVED,
	MTP_INITIATOR_EVENT_OBJECT_ADDED,
	MTP_INITIATOR_EVENT_OBJECT_REMOVED,
	MTP_INITIATOR_EVENT_DEVICE_ADDED,
	MTP_INITIATOR_EVENT_DEVICE_REMOVED,
} mtp_event;

/* struct */
typedef struct _mtp_device_info mtp_device_info;
typedef struct _mtp_device_list mtp_device_list;
typedef struct _mtp_context mtp_context;
typedef struct _mtp_queue_data mtp_queue_data;
typedef struct _mtp_param mtp_param;

struct _mtp_device_info {
	LIBMTP_mtpdevice_t *device;
	int bus_location;
	char *model_name;
};

struct _mtp_device_list {
	GList *device_info_list; /* element : mtp_device_info */
	GThreadPool *threads;
	int device_num;
};

struct _mtp_context {
	/* daemon variable */
	bool is_initialized;
	GMainLoop *main_loop;

	/* controller variable */
	GAsyncQueue *c_queue;
	GThread *c_thread;
	bool is_running;

	/* devices */
	mtp_device_list *device_list;

	/* gdbus variable */
	gint bus_id;
	GDBusConnection *connection;
	mtpgdbuslibManager *manager;

	/* db variable */
	sqlite3 *db;
};

struct _mtp_queue_data {
	mtp_controller_func func;
	gpointer data;
};

struct _mtp_param {
	void *object;
	GDBusMethodInvocation *invocation;
	mtp_context *mtp_ctx;
	int param1;
	int param2;
	int param3;
	int param4;
	char *char_param1;
};

#endif
