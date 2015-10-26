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

#include "mtp_daemon_gdbus_deviceinfo.h"
#include "mtp_daemon_controller.h"

/* Device Info */
static void __deviceinfo_get_manufacturername_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device;
	char *name = NULL;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	MTP_LOGE(">>> Call deviceinfo_get_manufacturername_thread_func");

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);

	/* do process */
	name = LIBMTP_Get_Manufacturername(device);

	MTP_LOGE("name : %s, device handle : %p", name, device);

	mtp_gdbuslib_deviceinfo_complete_get_manufacturername(param->object,
		param->invocation, name, result);

	/* deinitializing */
	g_free(name);
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __deviceinfo_get_modelname_thread_func(gpointer user_data)
{
	/* variable definition */
	mtp_param *param  = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device;
	char *name = NULL;

	/* check precondition */
	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	MTP_LOGE(">>> Call deviceinfo_get_modelname_thread_func");

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);

	/* do process */
	name = LIBMTP_Get_Modelname(device);

	MTP_LOGE("name : %s, device handle : %p", name, device);

	mtp_gdbuslib_deviceinfo_complete_get_modelname(param->object,
		param->invocation, name, result);

	/* deinitializing */
	g_free(name);
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __deviceinfo_get_serialnumber_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device;
	char *name = NULL;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	MTP_LOGE(">>> Call deviceinfo_get_serialnumber_thread_func");

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);

	name = LIBMTP_Get_Serialnumber(device);

	MTP_LOGE("name : %s, device handle : %p", name, device);

	mtp_gdbuslib_deviceinfo_complete_get_serialnumber(param->object,
		param->invocation, name, result);

	/* deinitializing */
	g_free(name);
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

static void __deviceinfo_get_deviceversion_thread_func(gpointer user_data)
{
	mtp_param *param = (mtp_param *)user_data;
	mtp_error_e result = MTP_ERROR_NONE;
	LIBMTP_mtpdevice_t *device;
	char *name = NULL;

	g_assert(param != NULL);
	g_assert(param->object != NULL);
	g_assert(param->invocation != NULL);

	MTP_LOGE(">>> Call deviceinfo_get_deviceversion_thread_func");

	/* parameter unpacking */
	device = (LIBMTP_mtpdevice_t *)(param->param1);

	name = LIBMTP_Get_Deviceversion(device);

	MTP_LOGE("name : %s, device handle: %p", name, device);

	mtp_gdbuslib_deviceinfo_complete_get_deviceversion(param->object,
		param->invocation, name, result);

	/* deinitializing */
	g_free(name);
	g_object_unref(param->invocation);
	g_object_unref(param->object);
	g_free(param);
}

gboolean deviceinfo_get_manufacturername(
		mtpgdbuslibDeviceinfo *deviceinfo,
		GDBusMethodInvocation *invocation,
		gint handle,
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

	param->object = g_object_ref(deviceinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = handle;

	if (mtp_daemon_controller_push(__deviceinfo_get_manufacturername_thread_func,
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

	mtp_gdbuslib_deviceinfo_complete_get_manufacturername(deviceinfo,
		invocation, 0, result);

	return TRUE;
}

gboolean deviceinfo_get_modelname(
		mtpgdbuslibDeviceinfo *deviceinfo,
		GDBusMethodInvocation *invocation,
		gint handle,
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

	param->object = g_object_ref(deviceinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = handle;

	if (mtp_daemon_controller_push(__deviceinfo_get_modelname_thread_func,
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

	mtp_gdbuslib_deviceinfo_complete_get_modelname(deviceinfo, invocation, 0, result);

	return TRUE;
}

gboolean deviceinfo_get_serialnumber(
		mtpgdbuslibDeviceinfo *deviceinfo,
		GDBusMethodInvocation *invocation,
		gint handle,
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

	param->object = g_object_ref(deviceinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = handle;

	if (mtp_daemon_controller_push(__deviceinfo_get_serialnumber_thread_func,
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

	mtp_gdbuslib_deviceinfo_complete_get_serialnumber(deviceinfo, invocation, 0, result);

	return TRUE;
}

gboolean deviceinfo_get_deviceversion(
		mtpgdbuslibDeviceinfo *deviceinfo,
		GDBusMethodInvocation *invocation,
		gint handle,
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

	param->object = g_object_ref(deviceinfo);
	param->invocation = g_object_ref(invocation);
	param->mtp_ctx = (mtp_context *)user_data;
	param->param1 = handle;

	if (mtp_daemon_controller_push(__deviceinfo_get_deviceversion_thread_func,
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

	mtp_gdbuslib_deviceinfo_complete_get_deviceversion(deviceinfo, invocation, 0, result);

	return TRUE;
}

