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

#include <fcntl.h>
#include <unistd.h>

/* Device Manager */
static void __manager_get_device_list_thread_func(gpointer user_data)
{
	/* variable definition */
	int count = 0;
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	mtpgdbuslibManager *object;

	GVariant *gv = NULL;
	GVariantBuilder b;
	GList *l = NULL;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	object = (mtpgdbuslibManager *)(param->object);

	/* do process */
	g_variant_builder_init(&b, G_VARIANT_TYPE("aa{sv}"));

	count = param->mtp_ctx->device_list->device_num;
	MTP_LOGE("devices_num : %d", count);

	l = param->mtp_ctx->device_list->device_info_list;
	for (l = g_list_first(l); l != NULL; l = l->next) {
		mtp_device_info *device_info;
		device_info = l->data;

		g_variant_builder_open(&b, G_VARIANT_TYPE("a{sv}"));

		g_variant_builder_add(&b, "{sv}", "model_name",
			g_variant_new_string(device_info->model_name));
		g_variant_builder_add(&b, "{sv}", "bus_location",
			g_variant_new_int32(device_info->bus_location));

		g_variant_builder_close(&b);
	}

	gv = g_variant_builder_end(&b);

	mtp_gdbuslib_manager_complete_get_device_list(object, param->invocation, count, gv, result);

	/* deinitializing */
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_device_handle_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	mtpgdbuslibManager *object;

	int busno;
	int handle = -1;
	GList *l = NULL;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	busno = (int)(param->param1);
	object = (mtpgdbuslibManager *)(param->object);

	/* do process */
	l = param->mtp_ctx->device_list->device_info_list;
	for (l = g_list_first(l); l != NULL; l = l->next) {
		mtp_device_info *device_info = NULL;
		device_info = l->data;

		if (busno == device_info->bus_location) {
			handle = (int)device_info->device;
			MTP_LOGE("handle : %d, device_h : %p", handle, device_info->device);
			break;
		}
	}

	if (handle != -1)
		MTP_LOGE("found MTP device - handle : %p, bus: %d", handle, busno);
	else
		MTP_LOGE("!!! not found MTP device handle - bus: %d", busno);

	mtp_gdbuslib_manager_complete_get_device_handle(object, param->invocation, handle, result);

	/* deinitializing */
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_storage_ids_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device;
	LIBMTP_devicestorage_t *storage;
	int count = 0;

	GVariant *gv = NULL;
	GVariantBuilder b;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);

	/* loop over storages */
	g_variant_builder_init(&b, G_VARIANT_TYPE("aa{sv}"));

	for (storage = device->storage; storage != NULL; storage = storage->next) {
		MTP_LOGE("storage : %p", storage);

		g_variant_builder_open(&b, G_VARIANT_TYPE("a{sv}"));

		g_variant_builder_add(&b, "{sv}", "storage_id",
			g_variant_new_int32(storage->id));

		g_variant_builder_close(&b);

		count++;
	}

	gv = g_variant_builder_end(&b);

	MTP_LOGE("storage_num : %d", count);

	mtp_gdbuslib_manager_complete_get_storage_ids(param->object,
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

	LIBMTP_mtpdevice_t *device;
	int ret;
	int i;
	int storage_id;
	int format;
	int parent_object_handle;
	int *object_list = NULL;
	int temp, object_num = 0;

	GVariant *gv = NULL;
	GVariantBuilder b;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */

	device = (LIBMTP_mtpdevice_t *)(param->param1);
	storage_id = param->param2;
	format = param->param3;
	parent_object_handle = param->param4;

	MTP_LOGE("storage id : %d", param->param2);
	MTP_LOGE("format : %d", param->param3);
	MTP_LOGE("parent : %d", param->param4);

	ret = LIBMTP_Get_Object_Handles(device, storage_id, format, parent_object_handle,
		&object_list, &temp);

	if (ret < 0)
		MTP_LOGE("LIBMTP_Get_Object_Handles is failed : %d", ret);

	MTP_LOGE("Total Object number : %d", temp);

	/* loop over storages */
	g_variant_builder_init(&b, G_VARIANT_TYPE("aa{sv}"));

	for (i = 0; i < temp; i++) {
		MTPObjectInfo *object_info = NULL;

		if (mtp_daemon_db_is_exist((int)device, object_list[i], param->mtp_ctx) == false) {
			MTP_LOGE("Objectinfo is not stored. %d, %d", device, object_list[i]);
			object_info = LIBMTP_Get_Object_Info(device, object_list[i]);

			if (object_info != NULL)
				mtp_daemon_db_insert(device, storage_id, object_list[i], object_info, param->mtp_ctx);
		} else {
			MTP_LOGE("Objectinfo is stored. ");
			object_info = mtp_daemon_db_get_object_info(device, object_list[i], param->mtp_ctx);
		}

		if (object_info != NULL) {
			MTP_LOGE("object id : %d, format : %d, filename : %s", object_list[i],
							object_info->ObjectFormat, object_info->Filename);

			if (format == LIBMTP_FILETYPE_ALL || format == object_info->ObjectFormat ||
				(format == LIBMTP_FILETYPE_ALL_IMAGE && LIBMTP_FILETYPE_IS_IMAGE(object_info->ObjectFormat))) {
				g_variant_builder_open(&b, G_VARIANT_TYPE("a{sv}"));

				g_variant_builder_add(&b, "{sv}", "object_handle",
				g_variant_new_int32(object_list[i]));

				g_variant_builder_close(&b);

				object_num++;
			}
		}
		g_free(object_info);
	}

	gv = g_variant_builder_end(&b);

	MTP_LOGE("Real object_num : %d", object_num);

	mtp_gdbuslib_manager_complete_get_object_handles(param->object,
		param->invocation, object_num, gv, result);

	/* deinitializing */
	g_free(object_list);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __manager_get_object_thread_func(gpointer user_data)
{
	/* variable definition */
	int ret;
	int fd;
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;

	LIBMTP_mtpdevice_t *device;
	int object_handle;
	char *dest_path;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);
	object_handle = param->param2;
	dest_path = param->char_param1;

	/* file open */
	fd = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0644);

	if (fd < 0) {
		result = MTP_ERROR_GENERAL;
	} else {
		ret = LIBMTP_Get_File_To_File_Descriptor(device, object_handle, fd, NULL, NULL);

		if (ret != 0)
			result = MTP_ERROR_PLUGIN;

		close(fd);
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
	int ret;
	int fd;
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	unsigned char *thumb_data;
	unsigned int thumb_size;

	LIBMTP_mtpdevice_t *device;
	int object_handle;
	char *dest_path;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);
	object_handle = param->param2;
	dest_path = param->char_param1;

	MTP_LOGE("dest_path : %s", dest_path);

	/* file open */
	fd = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0644);

	if (fd < 0) {
		result = MTP_ERROR_GENERAL;
	} else {
		ret = LIBMTP_Get_Thumbnail(device, object_handle, &thumb_data, &thumb_size);

		if (ret == 0) {
			ret = write(fd, thumb_data, thumb_size);
			if (ret == 0)
				result = MTP_ERROR_ALLOC_FAIL;
		} else {
			result = MTP_ERROR_PLUGIN;
		}

		close(fd);
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
	int object_handle;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);
	g_assert(param->mtp_ctx != NULL);

	MTP_LOGI("%s", __func__);

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);
	object_handle = param->param2;

	ret = LIBMTP_Delete_Object(device, object_handle);
	if (ret != 0)
		result = MTP_ERROR_PLUGIN;

	mtp_gdbuslib_manager_complete_delete_object(param->object,
		param->invocation, result);

	/* deinitializing */
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

/* Device Manager */
gboolean manager_get_device_list(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;

	if (mtp_daemon_controller_push(__manager_get_device_list_thread_func, param, param->mtp_ctx)
		!= MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_manager_complete_get_device_list(manager, invocation, 0, NULL, result);

	return TRUE;
}

gboolean manager_get_device_handle(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint busno,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = busno;

	if (mtp_daemon_controller_push(__manager_get_device_handle_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_manager_complete_get_device_handle(manager, invocation, 0, result);

	return TRUE;
}

gboolean manager_get_storage_ids(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint device_handle,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;

	if (mtp_daemon_controller_push(__manager_get_storage_ids_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

		goto OUT;
	}

	return TRUE;

OUT:
	if (param != NULL) {
		g_object_unref(param->invocation);
		g_object_unref(param->object);

		g_free(param);
	}

	mtp_gdbuslib_manager_complete_get_storage_ids(manager,
		invocation, 0, NULL, result);

	return TRUE;
}

gboolean manager_get_object_handles(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint device_handle,
		gint storage_id,
		gint format,
		gint parent_object_handle,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;
	param->param2 = storage_id;
	param->param3 = format;
	param->param4 = parent_object_handle;

	if (mtp_daemon_controller_push(__manager_get_object_handles_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

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
		gint device_handle,
		gint object_handle,
		gchar *dest_path,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;
	param->param2 = object_handle;
	param->char_param1 = g_strdup(dest_path);

	if (mtp_daemon_controller_push(__manager_get_object_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

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
		gint device_handle,
		gint object_handle,
		gchar *dest_path,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;
	param->param2 = object_handle;
	param->char_param1 = g_strdup(dest_path);

	if (mtp_daemon_controller_push(__manager_get_thumbnail_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

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
		gint device_handle,
		gint object_handle,
		gpointer user_data)
{
	mtp_param *param = NULL;
	gint result = MTP_ERROR_NONE;

	MTP_LOGE(">>> REQUEST from [%s]",
		g_dbus_method_invocation_get_sender(invocation));

	param = g_try_new0(mtp_param, 1);
	if (param == NULL) {
		MTP_LOGE("Memory allocation failed");
		result = MTP_ERROR_ALLOC_FAIL;
		goto OUT;
	}

	param->object = g_object_ref(manager);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;
	param->param2 = object_handle;

	if (mtp_daemon_controller_push(__manager_delete_object_thread_func,
		param, param->mtp_ctx) != MTP_ERROR_NONE) {
		/* return error if queue was blocked */
		MTP_LOGE("controller is processing important message..");
		result = MTP_ERROR_GENERAL;

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
