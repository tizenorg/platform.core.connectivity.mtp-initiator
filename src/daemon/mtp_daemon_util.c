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
	LIBMTP_mtpdevice_t *device_handle, int storage_id, mtp_context *mtp_ctx)
{
	LIBMTP_devicestorage_t *storage = NULL;
	GList *l;

	/* search device */
	for (l = g_list_first(mtp_ctx->device_list->device_info_list); l != NULL; l = l->next) {
		mtp_device_info *device_info;
		device_info = l->data;

		if (device_info->device == device_handle) {
			for (storage = device_info->device->storage; storage != NULL; storage = storage->next) {
				if (storage->id == storage_id)
					return storage;
			}
		}
	}

	return NULL;
}

