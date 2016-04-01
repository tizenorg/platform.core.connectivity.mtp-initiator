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

#ifndef __MTP_DAEMON_DB_H__
#define __MTP_DAEMON_DB_H__

#include "mtp_daemon.h"

mtp_error_e mtp_daemon_db_init(mtp_context *mtp_ctx);
mtp_error_e mtp_daemon_db_deinit(mtp_context *mtp_ctx);
bool mtp_daemon_db_is_exist(int mtp_device, int object_handle, mtp_context *mtp_ctx);
mtp_error_e mtp_daemon_db_insert(int mtp_device, int mtp_storage,
	int object_handle, MTPObjectInfo *object_info, mtp_context *mtp_ctx);
mtp_error_e mtp_daemon_db_delete(int mtp_device, int mtp_storage,
	int object_handle, mtp_context *mtp_ctx);
mtp_error_e mtp_daemon_db_update(int mtp_device, int object_handle,
	MTPObjectInfo *object_info, mtp_context *mtp_ctx);
MTPObjectInfo* mtp_daemon_db_get_object_info(int mtp_device,
	int object_handle, mtp_context *mtp_ctx);
mtp_error_e mtp_daemon_db_begin(mtp_context *mtp_ctx);
mtp_error_e mtp_daemon_db_commit(mtp_context *mtp_ctx);


#endif
