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

#ifndef __MTP_DAEMON_GDBUS_MANAGER_H__
#define __MTP_DAEMON_GDBUS_MANAGER_H__

#include "mtp_daemon.h"

gboolean manager_get_devices(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gpointer user_data);

gboolean manager_get_storages(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gpointer user_data);

gboolean manager_get_object_handles(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint mtp_storage,
		gint format,
		gint parent_object_handle,
		gpointer user_data);

gboolean manager_get_object(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint object_handle,
		gchar *dest_path,
		gpointer user_data);

gboolean manager_get_thumbnail(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint object_handle,
		gchar *dest_path,
		gpointer user_data);

gboolean manager_delete_object(
		mtpgdbuslibManager *manager,
		GDBusMethodInvocation *invocation,
		gint mtp_device,
		gint object_handle,
		gpointer user_data);

#endif
