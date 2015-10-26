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

#ifndef __MTP_DAEMON_UTIL_H__
#define __MTP_DAEMON_UTIL_H__

#include "mtp_daemon.h"

LIBMTP_devicestorage_t *mtp_daemon_util_get_storage_handle(
	LIBMTP_mtpdevice_t *device_handle, int storage_id, mtp_context *mtp_ctx);

#endif

