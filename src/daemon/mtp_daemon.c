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

#include "mtp_daemon.h"
#include "mtp_daemon_gdbus.h"
#include "mtp_daemon_controller.h"
#include "mtp_daemon_event.h"
#include "mtp_daemon_db.h"

static mtp_context *_mtp_ctx = NULL;

mtp_context *get_mtp_ctx(void)
{
	MTP_LOGI("%s", __func__);
	return _mtp_ctx;
}

static mtp_error_e __daemon_initalize(mtp_context *mtp_ctx)
{
	mtp_error_e ret = MTP_ERROR_NONE;

	RETV_IF(mtp_ctx == NULL, MTP_ERROR_INVALID_PARAMETER);

	ret = mtp_daemon_controller_init(mtp_ctx);
	if (ret != MTP_ERROR_NONE)
		return ret;

	MTP_LOGI("mtp_daemon_controller_init work successful!!");

	ret = mtp_daemon_db_init(mtp_ctx);
	if (ret != MTP_ERROR_NONE)
		return ret;

	MTP_LOGI("mtp_daemon_db_init work successful!!");

	ret = mtp_daemon_gdbus_init(mtp_ctx);
	if (ret != MTP_ERROR_NONE)
		return ret;

	MTP_LOGI("mtp_daemon_gdbus_init work successful!!");

	mtp_ctx->is_initialized = true;

	return ret;
}

static mtp_error_e __daemon_deinitalize(mtp_context *mtp_ctx)
{
	mtp_error_e ret = MTP_ERROR_NONE;

	RETV_IF(mtp_ctx == NULL, MTP_ERROR_INVALID_PARAMETER);

	/* deinitialize daemon */
	mtp_ctx->is_initialized = false;

	usb_host_unset_event_cb();

	ret = mtp_daemon_db_deinit(mtp_ctx);
	if (ret != MTP_ERROR_NONE)
		return ret;

	g_main_loop_unref(mtp_ctx->main_loop);
	g_free(mtp_ctx);

	_mtp_ctx = NULL;

	remove(MTP_DB_FILE);

	return ret;
}

static void __mtp_signal_handler(int signo)
{
	gint ret;

	if (!_mtp_ctx)
		return;

	switch (signo) {
	case SIGTERM:
		ret = __daemon_deinitalize(_mtp_ctx);
		MTP_LOGD("SIGTERM ret[%d]", ret);
		break;

	default:
		MTP_LOGD("unhandled signal:[%d]", signo);
		exit(EXIT_FAILURE);
		break;
	} /* end switch */

	return;
}

int main(int argc, char *argv[])
{
	mtp_context *mtp_ctx = NULL;
	mtp_error_e ret = MTP_ERROR_NONE;
	struct sigaction sigact;

	MTP_LOGI("mtp_daemon - start!!!");

	remove(MTP_DB_FILE);

	/* 1. signal registration */
	sigact.sa_handler = __mtp_signal_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	if (sigaction(SIGTERM, &sigact, NULL) < 0)
		MTP_LOGW("sigaction(SIGTERM) failed.");

	/* 2. create mtp daemon context */
	mtp_ctx = g_new0(mtp_context, 1);
	if (!mtp_ctx) {
		MTP_LOGE("create mtp_context is failed");
		goto EXIT;
	}

	mtp_ctx->device_list = g_new0(mtp_device_list, 1);
	if (mtp_ctx->device_list == NULL) {
		MTP_LOGE("create mtp_context is failed");
		goto EXIT;
	}

	mtp_ctx->device_list->device_num = 0;

	_mtp_ctx = mtp_ctx;

	mtp_ctx->main_loop = g_main_loop_new(NULL, FALSE);

	/* 3. initialize daemon */
	ret = __daemon_initalize(mtp_ctx);
	if (ret != MTP_ERROR_NONE) {
		MTP_LOGE("mtp_daemon_initalize is failed");
		goto EXIT;
	}

	g_main_loop_run(mtp_ctx->main_loop);

EXIT:
	ret = __daemon_deinitalize(mtp_ctx);
	if (ret != MTP_ERROR_NONE) {
		MTP_LOGE("mtp_daemon_deinitalize is failed");
		return -1;
	}

	MTP_LOGD("mtp_daemon - exit!!!");

	return 0;
}

