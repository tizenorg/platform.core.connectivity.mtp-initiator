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

#include "mtp_daemon_gdbus_manager.h"
#include "mtp_daemon_controller.h"
#include "mtp_daemon_db.h"
#include "mtp_daemon_util.h"

#include <fcntl.h>
#include <unistd.h>

/* Manager */
static void __manager_get_devices_thread_func(gpointer user_data)
{
	/* variable definition */
	int count = 0;
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	mtpgdbuslibManager *object;

	GVariant *gv = NULL;
	GVariantBuilder b;
	int slot;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	object = (mtpgdbuslibManager *)(param->object);

	/* do process */
	g_variant_builder_init(&b, G_VARIANT_TYPE("aa{sv}"));

	count = param->mtp_ctx->device_list->device_num;
	MTP_LOGI("devices_num: %d", count);

	for (slot = 1; slot < MTP_MAX_SLOT; slot++) {
		mtp_device_info *device_info;
		device_info = param->mtp_ctx->device_list->device_info_list[slot];

		if (device_info != NULL) {
			g_variant_builder_open(&b, G_VARIANT_TYPE("a{sv}"));

			g_variant_builder_add(&b, "{sv}", "mtp_device",
				g_variant_new_int32(slot));

			g_variant_builder_close(&b);

			MTP_LOGI("mtp_device : %d, bus_location: %d, device_number : %d",
				slot, device_info->bus_location, device_info->device_number);
		}
	}

	gv = g_variant_builder_end(&b);

	mtp_gdbuslib_manager_complete_get_devices(object, param->invocation, count, gv, result);

	/* deinitializing */
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_storages_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device = NULL;
	LIBMTP_devicestorage_t *storage;
	mtp_device_info *device_info;
	int device_id;
	int count = 0;

	GVariant *gv = NULL;
	GVariantBuilder b;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL)
		device = (LIBMTP_mtpdevice_t *)device_info->device;

	MTP_LOGI("device_id: %d, device: %p", device_id, device);

	/* loop over storages */
	g_variant_builder_init(&b, G_VARIANT_TYPE("aa{sv}"));

	if (device_info != NULL) {
		for (storage = device->storage; storage != NULL; storage = storage->next) {
			MTP_LOGI("storage: %p", storage);

			g_variant_builder_open(&b, G_VARIANT_TYPE("a{sv}"));

			g_variant_builder_add(&b, "{sv}", "mtp_storage",
				g_variant_new_int32(storage->id));

			g_variant_builder_close(&b);

			count++;
		}
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	gv = g_variant_builder_end(&b);

	MTP_LOGI("storage_num: %d", count);

	mtp_gdbuslib_manager_complete_get_storages(param->object,
		param->invocation, count, gv, result);

	/* deinitializing */
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_object_handles_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	mtp_device_info *device_info;
	LIBMTP_mtpdevice_t *device = NULL;

	int ret;
	int slot;
	int device_id;
	int mtp_storage;
	int format;
	int parent_object_handle;
	int *object_list = NULL;
	int target_object_num = 0;
	int total_object_num = 0;

	GVariant *gv = NULL;
	GVariantBuilder b;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	mtp_storage = param->param2;
	format = param->param3;
	parent_object_handle = param->param4;

	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL)
		device = (LIBMTP_mtpdevice_t *)device_info->device;

	MTP_LOGI("device_id: %d, storage id: %d, format: %d, parent: %d, device: %p",
		param->param1, param->param2, param->param3, param->param4, device);

	if (device_info != NULL) {
		ret = LIBMTP_Get_Object_Handles(device, (uint32_t)mtp_storage,
			(uint32_t)format, (uint32_t)parent_object_handle,
			(uint32_t **)&object_list, (uint32_t *)&total_object_num);

		if (ret < 0)
			MTP_LOGE("LIBMTP_Get_Object_Handles is failed : %d", ret);
	}

	/*MTP_LOGI("Total Object number: %d", temp);*/

	/* loop over storages */
	g_variant_builder_init(&b, G_VARIANT_TYPE("aa{sv}"));

	if (device_info != NULL) {
		mtp_daemon_db_begin(param->mtp_ctx);

		for (slot = 0; slot < total_object_num; slot++) {
			MTPObjectInfo *object_info = NULL;
			/*object_info = LIBMTP_Get_Object_Info(device, (uint32_t)object_list[slot]);

			if (object_info != NULL)
				mtp_daemon_db_insert(device_id, storage_id, object_list[slot], object_info, param->mtp_ctx);*/

			if (mtp_daemon_db_is_exist(device_id, object_list[slot], param->mtp_ctx) == false) {
				MTP_LOGI("Objectinfo is Not stored. device_id: %d, object_list[%d]: %d",
					device_id, slot, object_list[slot]);
				object_info = LIBMTP_Get_Object_Info(device, (uint32_t)object_list[slot]);

				if (object_info != NULL)
					mtp_daemon_db_insert(device_id, mtp_storage, object_list[slot], object_info, param->mtp_ctx);
			} else {
				MTP_LOGI("Objectinfo is stored. device_id: %d, object_list[%d]: %d",
					device_id, slot, object_list[slot]);
				object_info = mtp_daemon_db_get_object_info(device_id, object_list[slot], param->mtp_ctx);
			}

			if (object_info != NULL) {
				/*MTP_LOGI("object id: %d, format : %d, filename : %s", object_list[slot],
					object_info->ObjectFormat, object_info->Filename);*/

				if ((format == LIBMTP_FILETYPE_ALL &&
						(object_info->ObjectFormat == LIBMTP_FILETYPE_FOLDER ||
						LIBMTP_FILETYPE_IS_IMAGE(object_info->ObjectFormat))) ||
					(format == LIBMTP_FILETYPE_ALL_IMAGE &&
						LIBMTP_FILETYPE_IS_IMAGE(object_info->ObjectFormat)) ||
					(format == object_info->ObjectFormat)) {
					g_variant_builder_open(&b, G_VARIANT_TYPE("a{sv}"));

					g_variant_builder_add(&b, "{sv}", "object_handle",
					g_variant_new_int32(object_list[slot]));

					g_variant_builder_close(&b);

					target_object_num++;
				}
			}
			g_free(object_info);
		}

		mtp_daemon_db_commit(param->mtp_ctx);
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	gv = g_variant_builder_end(&b);

	/*MTP_LOGI("Real object_num: %d", target_object_num);*/

	mtp_gdbuslib_manager_complete_get_object_handles(param->object,
		param->invocation, target_object_num, gv, result);

	/* deinitializing */
	g_free(object_list);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_object_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	int ret;
	int fd;

	LIBMTP_mtpdevice_t *device;
	mtp_device_info *device_info;
	int device_id;
	int object_handle;
	char *dest_path;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	object_handle = param->param2;
	dest_path = param->char_param1;

	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;
		MTP_LOGI("device_id: %d, device: %p, dest_path: %s", device_id, device, dest_path);

		/* file open */
		if (access(dest_path, F_OK) >= 0) {
			MTP_LOGE("file exist - dest_path: %s", dest_path);

			if (unlink(dest_path) < 0)
				MTP_LOGE("remove fail - dest_path: %s", dest_path);
		}

		fd = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0644);

		if (fd < 0) {
			MTP_LOGE("file open fail - fd: %d, dest_path: %s", fd, dest_path);
			result = MTP_ERROR_IO_ERROR;
		} else {
			ret = LIBMTP_Get_File_To_File_Descriptor(device, object_handle, fd, NULL, NULL);

			if (ret != 0) {
				MTP_LOGE("get descriptor fail - ret: %d", ret);
				result = MTP_ERROR_PLUGIN_FAIL;
			}

			close(fd);
		}
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_manager_complete_get_object(param->object,
		param->invocation, result);

	/* deinitializing */
	g_free(dest_path);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_thumbnail_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	unsigned char *thumb_data;
	unsigned int thumb_size;
	int ret;
	int fd;

	LIBMTP_mtpdevice_t *device;
	mtp_device_info *device_info;
	int device_id;
	int object_handle;
	char *dest_path;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	object_handle = param->param2;
	dest_path = param->char_param1;

	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;
		MTP_LOGI("device_id: %d, device: %p, dest_path: %s", device_id, device, dest_path);

		/* file open */
		if (access(dest_path, F_OK) >= 0) {
			MTP_LOGE("file exist - dest_path: %s", dest_path);

			if (unlink(dest_path) < 0)
				MTP_LOGE("remove fail - dest_path: %s", dest_path);
		}

		fd = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0644);

		if (fd < 0) {
			MTP_LOGE("file open fail - fd: %d, dest_path: %s", fd, dest_path);
			result = MTP_ERROR_IO_ERROR;
		} else {
			ret = LIBMTP_Get_Thumbnail_From_Exif_Data(device, object_handle, &thumb_data, &thumb_size);
			if (ret != 0)
				ret = LIBMTP_Get_Thumbnail(device, object_handle, &thumb_data, &thumb_size);

			if (ret == 0) {
				ret = write(fd, thumb_data, thumb_size);
				if (ret == 0)
					result = MTP_ERROR_OUT_OF_MEMORY;
			} else {
				MTP_LOGE("get thumbnail fail - ret: %d", ret);
				result = MTP_ERROR_PLUGIN_FAIL;
			}

			close(fd);
		}
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_manager_complete_get_thumbnail(param->object,
		param->invocation, result);

	/* deinitializing */
	g_free(dest_path);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_delete_object_thread_func(gpointer user_data)
{
	/* variable definition */
	int ret;
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;

	LIBMTP_mtpdevice_t *device;
	mtp_device_info *device_info;
	int device_id;
	int object_handle;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	device_id = param->param1;
	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device = (LIBMTP_mtpdevice_t *)device_info->device;

		MTP_LOGI("device_id: %d, device: %p", device_id, device);

		object_handle = param->param2;

		ret = LIBMTP_Delete_Object(device, object_handle);
		if (ret != 0)
			result = MTP_ERROR_PLUGIN_FAIL;
	} else {
		MTP_LOGE("!!! no MTP device");
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_manager_complete_delete_object(param->object,
		param->invocation, result);

	/* deinitializing */
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

/* Manager */
gboolean manager_get_devices(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_OUT_OF_MEMORY;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;

	if (mtp_daemon_controller_push(__manager_get_devices_thread_func, param, param->mtp_ctx)
		!= MTP_ERROR_NONE) {
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

	mtp_gdbuslib_manager_complete_get_devices(manager, invocation, 0, NULL, result);

	return TRUE;
}

gboolean manager_get_storages(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_OUT_OF_MEMORY;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;

	if (mtp_daemon_controller_push(__manager_get_storages_thread_func,
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

	mtp_gdbuslib_manager_complete_get_storages(manager,
		invocation, 0, NULL, result);

	return TRUE;
}

gboolean manager_get_object_handles(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gint format,
		gint parent_object_handle,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	/*MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));*/

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_OUT_OF_MEMORY;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = mtp_storage;
	param->param3 = format;
	param->param4 = parent_object_handle;

	if (mtp_daemon_controller_push(__manager_get_object_handles_thread_func,
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

	mtp_gdbuslib_manager_complete_get_object_handles(manager,
		invocation, 0, NULL, result);

	return TRUE;
}

gboolean manager_get_object(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint object_handle,
		gchar *dest_path,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	/*MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));*/

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_OUT_OF_MEMORY;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = object_handle;
	param->char_param1 = g_strdup(dest_path);

	if (mtp_daemon_controller_push(__manager_get_object_thread_func,
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

	mtp_gdbuslib_manager_complete_get_object(manager, invocation, result);

	return TRUE;
}

gboolean manager_get_thumbnail(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint object_handle,
		gchar *dest_path,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	/*MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));*/

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_OUT_OF_MEMORY;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = object_handle;
	param->char_param1 = g_strdup(dest_path);

	if (mtp_daemon_controller_push(__manager_get_thumbnail_thread_func,
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

		if (param->char_param1 != NULL)
			free(param->char_param1);

		g_free(param);
	}

	mtp_gdbuslib_manager_complete_get_thumbnail(manager, invocation, result);

	return TRUE;
}

gboolean manager_delete_object(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint object_handle,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGI(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_OUT_OF_MEMORY;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = mtp_device;
	param->param2 = object_handle;

	if (mtp_daemon_controller_push(__manager_delete_object_thread_func,
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

		if (param->char_param1 != NULL)
			free(param->char_param1);

		g_free(param);
	}

	mtp_gdbuslib_manager_complete_delete_object(manager, invocation, result);

	return TRUE;
}
