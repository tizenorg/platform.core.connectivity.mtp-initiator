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

#include "mtp_daemon_gdbus_storageinfo.h"
#include "mtp_daemon_controller.h"
#include "mtp_daemon_util.h"

static void __storageinfo_get_description_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	int device_id;
	int mtp_storage;
	LIBMTP_mtpdevice_t *device = NULL;
	LIBMTP_devicestorage_t *storage = NULL;
	mtp_device_info *device_info = NULL;
	char *name = NULL;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;

		mtp_storage = param->param2;
		MTP_LOGI("device_id: %d, device: %p, mtp_storage: %d", device_id, device, mtp_storage);

		storage = mtp_daemon_util_get_storage_handle(device,
			mtp_storage, param->mtp_ctx);

		if (storage)
			name = storage->StorageDescription;

		MTP_LOGI("StorageDescription: %s, storage handle: %p", name, storage);
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_storageinfo_complete_get_description(param->object,
		param->invocation, name, result);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __storageinfo_get_freespace_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	int device_id;
	int mtp_storage;
	LIBMTP_mtpdevice_t *device = NULL;
	LIBMTP_devicestorage_t *storage = NULL;
	mtp_device_info *device_info = NULL;
	guint64 value = 0;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;

		mtp_storage = param->param2;
		MTP_LOGI("device_id: %d, device: %p, mtp_storage: %d", device_id, device, mtp_storage);

		storage = mtp_daemon_util_get_storage_handle(device,
			mtp_storage, param->mtp_ctx);

		if (storage)
			value = storage->FreeSpaceInBytes;

		MTP_LOGI("FreeSpaceInBytes: %llu, storage handle: %p", value, storage);
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_storageinfo_complete_get_free_space(param->object,
		param->invocation, value, result);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __storageinfo_get_maxcapacity_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	int device_id;
	int mtp_storage;
	LIBMTP_mtpdevice_t *device = NULL;
	LIBMTP_devicestorage_t *storage = NULL;
	mtp_device_info *device_info = NULL;
	guint64 value = 0;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;

		mtp_storage = param->param2;
		MTP_LOGI("device_id: %d, device: %p, mtp_storage: %d", device_id, device, mtp_storage);

		storage = mtp_daemon_util_get_storage_handle(device,
			mtp_storage, param->mtp_ctx);

		if (storage)
			value = storage->MaxCapacity;

		MTP_LOGI("MaxCapacity: %llu, storage handle: %p", value, storage);
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_storageinfo_complete_get_max_capacity(param->object,
		param->invocation, value, result);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __storageinfo_get_storagetype_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	int device_id;
	int mtp_storage;
	LIBMTP_mtpdevice_t *device = NULL;
	LIBMTP_devicestorage_t *storage = NULL;
	mtp_device_info *device_info = NULL;
	int value = 0;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;

		mtp_storage = param->param2;
		MTP_LOGI("device_id: %d, device: %p, mtp_storage: %d", device_id, device, mtp_storage);

		storage = mtp_daemon_util_get_storage_handle(device,
			mtp_storage, param->mtp_ctx);

		if (storage)
			value = storage->StorageType;

		MTP_LOGI("StorageType: %d, storage handle: %p", value, storage);
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_storageinfo_complete_get_storage_type(param->object,
		param->invocation, value, result);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __storageinfo_get_volumeidentifier_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	int device_id;
	int mtp_storage;
	LIBMTP_mtpdevice_t *device = NULL;
	LIBMTP_devicestorage_t *storage = NULL;
	mtp_device_info *device_info = NULL;
	char *name = NULL;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;

		mtp_storage = param->param2;
		MTP_LOGI("device_id: %d, device: %p, mtp_storage: %d", device_id, device, mtp_storage);

		storage = mtp_daemon_util_get_storage_handle(device,
			mtp_storage, param->mtp_ctx);

		if (storage)
			name = storage->VolumeIdentifier;

		MTP_LOGI("VolumeIdentifier: %s, storage handle: %p", name, storage);
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_storageinfo_complete_get_volume_identifier(param->object,
		param->invocation, name, result);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

gboolean storageinfo_get_description(
		mtpgdbuslibStorageinfo *storageinfo,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(storageinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = mtp_storage;

	if (mtp_daemon_controller_push(__storageinfo_get_description_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_CONTROLLER;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_storageinfo_complete_get_description(storageinfo, invocation, 0, result);

	return TRUE;
}

gboolean storageinfo_get_freespace(
		mtpgdbuslibStorageinfo *storageinfo,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(storageinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = mtp_storage;

	if (mtp_daemon_controller_push(__storageinfo_get_freespace_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_CONTROLLER;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_storageinfo_complete_get_free_space(storageinfo, invocation, 0, result);

	return TRUE;
}

gboolean storageinfo_get_maxcapacity(
		mtpgdbuslibStorageinfo *storageinfo,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(storageinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = mtp_storage;

	if (mtp_daemon_controller_push(__storageinfo_get_maxcapacity_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_CONTROLLER;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_storageinfo_complete_get_max_capacity(storageinfo, invocation, 0, result);

	return TRUE;
}

gboolean storageinfo_get_storagetype(
		mtpgdbuslibStorageinfo *storageinfo,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(storageinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = mtp_storage;

	if (mtp_daemon_controller_push(__storageinfo_get_storagetype_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_CONTROLLER;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_storageinfo_complete_get_storage_type(storageinfo, invocation, 0, result);

	return TRUE;
}

gboolean storageinfo_get_volumeidentifier(
		mtpgdbuslibStorageinfo *storageinfo,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(storageinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = mtp_storage;

	if (mtp_daemon_controller_push(__storageinfo_get_volumeidentifier_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_CONTROLLER;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_storageinfo_complete_get_volume_identifier(storageinfo,
		invocation, 0, result);

	return TRUE;
}
