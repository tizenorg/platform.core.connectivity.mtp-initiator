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

#include "mtp_daemon_util.h"

LIBMTP_devicestorage_t *mtp_daemon_util_get_storage_handle(
	LIBMTP_mtpdevice_t *mtp_device, int mtp_storage, mtp_context *mtp_ctx)
{
	LIBMTP_devicestorage_t *storage = NULL;
	int slot;
	int device_num = mtp_ctx->device_list->device_num;

	MTP_LOGI("device_num: %d", device_num);

	/* search device */
	for (slot = 1; slot < MTP_MAX_SLOT; slot++) {
		mtp_device_info *device_info;
		device_info = mtp_ctx->device_list->device_info_list[slot];

		if (device_info != NULL && device_info->device == mtp_device) {
			for (storage = device_info->device->storage; storage != NULL; storage = storage->next) {
				if (storage->id == mtp_storage)
					return storage;
			}
		}
	}

	return NULL;
}

int mtp_daemon_util_get_device_id(LIBMTP_mtpdevice_t *device, mtp_context *mtp_ctx)
{
	int slot;

	MTP_LOGI("device: %p, device_num: %d", device, mtp_ctx->device_list->device_num);

	for (slot = 1; slot < MTP_MAX_SLOT; slot++) {
		mtp_device_info *device_info;
		device_info = mtp_ctx->device_list->device_info_list[slot];

		if (device_info != NULL && device_info->device == device) {
			MTP_LOGI("device id: %d", slot);
			return slot;
		}
	}

	return -1;
}

