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

#include "mtp_daemon_gdbus_objectinfo.h"
#include "mtp_daemon_controller.h"
#include "mtp_daemon_db.h"
#include "mtp_daemon_util.h"

static void __objectinfo_get_property_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device_handle;
	mtp_device_info *device_info;
	int device_id;
	int object_handle;
	mtp_property_e property;
	int property_value = 0;
	MTPObjectInfo *object_info;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	object_handle = param->param2;
	property = param->param3;

	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device_handle = (LIBMTP_mtpdevice_t *)device_info->device;
		/*MTP_LOGI("device_id: %d, device: %p", device_id, device_handle);*/

		MTP_LOGI("Get property start - property %d", property);

		if (mtp_daemon_db_is_exist(device_id, object_handle, param->mtp_ctx) == false) {
			object_info = LIBMTP_Get_Object_Info(device_handle, object_handle);

			if (object_info != NULL)
				mtp_daemon_db_insert(device_id, object_info->StorageID, object_handle, object_info, param->mtp_ctx);
		} else {
			object_info = mtp_daemon_db_get_object_info(device_id, object_handle, param->mtp_ctx);
		}

		if (object_info != NULL) {
			switch (property) {
			case MTP_PROPERTY_ASSOCIATION_DESC:
				property_value = object_info->AssociationDesc;
				break;
			case MTP_PROPERTY_ASSOCIATION_TYPE:
				property_value = object_info->AssociationType;
				break;
			case MTP_PROPERTY_SIZE:
				property_value = object_info->ObjectCompressedSize;
				break;
			case MTP_PROPERTY_FORMAT:
				property_value = object_info->ObjectFormat;
				break;
			case MTP_PROPERTY_IMAGE_FIX_DEPTH:
				property_value = object_info->ImageBitDepth;
				break;
			case MTP_PROPERTY_IMAGE_FIX_WIDTH:
				property_value = object_info->ImagePixWidth;
				break;
			case MTP_PROPERTY_IMAGE_FIX_HEIGHT:
				property_value = object_info->ImagePixHeight;
				break;
			case MTP_PROPERTY_PARENT_OBJECT_HANDLE:
				property_value = object_info->ParentObject;
				break;
			case MTP_PROPERTY_STORAGE_ID:
				property_value = object_info->StorageID;
				break;
			case MTP_PROPERTY_THUMBNAIL_SIZE:
				property_value = object_info->ThumbCompressedSize;
				break;
			case MTP_PROPERTY_THUMBNAIL_FORMAT:
				property_value = object_info->ThumbFormat;
				break;
			case MTP_PROPERTY_THUMBNAIL_WIDTH:
				property_value = object_info->ThumbPixWidth;
				break;
			case MTP_PROPERTY_THUMBNAIL_HEIGHT:
				property_value = object_info->ThumbPixHeight;
				break;
			case MTP_PROPERTY_DATA_CREATED:
				property_value = object_info->CaptureDate;
				break;
			case MTP_PROPERTY_DATA_MODIFIED:
				property_value = object_info->ModificationDate;
				break;
			default:
				property_value = 0;
				break;
			}
			MTP_LOGI("Get property end, value is %d", property_value);
		} else {
			property_value = -999;
			MTP_LOGE("object_info is NULL!!");
		}
	} else {
		MTP_LOGE("!!! no MTP device");
		property_value = -999;
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_objectinfo_complete_get_property(param->object,
		param->invocation, property_value, result);

	g_free(object_info);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __objectinfo_get_property_string_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device_handle;
	mtp_device_info *device_info;
	int device_id;
	int object_handle;
	mtp_property_e property;
	char *property_value = NULL;
	MTPObjectInfo *object_info;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	/*MTP_LOGI("%s", __func__);*/

	/* parameter unpacking */
	device_id = param->param1;
	object_handle = param->param2;
	property = param->param3;

	device_info = (mtp_device_info *)param->mtp_ctx->device_list->device_info_list[device_id];

	if (device_info != NULL) {
		device_handle = (LIBMTP_mtpdevice_t *)device_info->device;
		/*MTP_LOGI("device_id: %d, device: %p", device_id, device_handle);*/

		MTP_LOGI("Get property start - property %d", property);

		if (mtp_daemon_db_is_exist(device_id, object_handle, param->mtp_ctx) == false) {
			object_info = LIBMTP_Get_Object_Info(device_handle, object_handle);

			if (object_info != NULL)
				mtp_daemon_db_insert(device_id, object_info->StorageID, object_handle, object_info, param->mtp_ctx);
		} else {
			object_info = mtp_daemon_db_get_object_info(device_id, object_handle, param->mtp_ctx);
		}

		if (object_info != NULL) {
			switch (property) {
			case MTP_PROPERTY_FILENAME:
				property_value = object_info->Filename;
				break;
			case MTP_PROPERTY_KEYWORD:
				property_value = object_info->Keywords;
				break;
			default:
				property_value = NULL;
				break;
			}
			MTP_LOGI("Get property end, value is %d", property_value);
		} else {
			property_value = NULL;
			MTP_LOGE("object_info is NULL!!");
		}
	} else {
		MTP_LOGE("!!! no MTP device");
		property_value = NULL;
		result = MTP_ERROR_NO_DEVICE;
	}

	mtp_gdbuslib_objectinfo_complete_get_property_string(param->object,
		param->invocation, property_value, result);

	g_free(object_info);

	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

gboolean objectinfo_get_property(
		mtpgdbuslibObjectinfo *objectinfo,
		GDBusMethodInvocation *invocation,
		gint device_handle,
		gint object_handle,
		gint property,
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

	param->object = g_object_ref(objectinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;
	param->param2 = object_handle;
	param->param3 = property;

	if (mtp_daemon_controller_push(__objectinfo_get_property_thread_func,
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

	mtp_gdbuslib_objectinfo_complete_get_property(objectinfo,
		invocation, 0, result);

	return TRUE;
}

gboolean objectinfo_get_property_string(
		mtpgdbuslibObjectinfo *objectinfo,
		GDBusMethodInvocation *invocation,
		gint device_handle,
		gint object_handle,
		gint property,
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

	param->object = g_object_ref(objectinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = device_handle;
	param->param2 = object_handle;
	param->param3 = property;

	if (mtp_daemon_controller_push(__objectinfo_get_property_string_thread_func,
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

	mtp_gdbuslib_objectinfo_complete_get_property_string(objectinfo,
		invocation, NULL, result);

	return TRUE;
}

