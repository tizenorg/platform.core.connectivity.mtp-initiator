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

#include "mtp_daemon_controller.h"

static void _controller_queue_free(gpointer user_data)
{
	mtp_queue_data* data = (mtp_queue_data*)user_data;
	if (data != NULL)
		g_free(data);
}

static gpointer _controller_thread(gpointer user_data)
{
	mtp_context* mtp_ctx = (mtp_context*)user_data;

	if (mtp_ctx == NULL || mtp_ctx->c_queue == NULL) {
		MTP_LOGE("create thread failed");
		g_thread_exit(NULL);

		return NULL;
	}

	mtp_ctx->is_running = true;

	while (mtp_ctx->is_running) {
		mtp_queue_data* data;

		data = g_async_queue_pop(mtp_ctx->c_queue);
		if (data->func)
			data->func(data->data);

		_controller_queue_free(data);
	}

	g_thread_exit(NULL);

	return NULL;
}

mtp_error_e mtp_daemon_controller_push(mtp_controller_func func,
	gpointer user_data, mtp_context *mtp_ctx)
{
	mtp_queue_data *data;
	mtp_error_e ret = MTP_ERROR_NONE;

	RETV_IF(func == NULL, MTP_ERROR_INVALID_PARAMETER);
	RETV_IF(mtp_ctx == NULL, MTP_ERROR_INVALID_PARAMETER);
	RETV_IF(mtp_ctx->c_queue == NULL, MTP_ERROR_INVALID_PARAMETER);

	data = g_new0(mtp_queue_data, 1);
	data->func = func;
	data->data = user_data;

	g_async_queue_push(mtp_ctx->c_queue , data);

	return ret;
}

mtp_error_e mtp_daemon_controller_init(mtp_context *mtp_ctx)
{
	GError *error = NULL;
	mtp_error_e ret = MTP_ERROR_NONE;

	RETV_IF(mtp_ctx == NULL, MTP_ERROR_INVALID_PARAMETER);

	mtp_ctx->c_queue  = g_async_queue_new_full(_controller_queue_free);
	mtp_ctx->c_thread = g_thread_try_new(NULL, _controller_thread, mtp_ctx, &error);

	if (mtp_ctx->c_thread == NULL) {
		MTP_LOGE("Can not create controller thread : %s", error->message);
		g_error_free(error);

		return MTP_ERROR_CONTROLLER;
	}

	return ret;
}

