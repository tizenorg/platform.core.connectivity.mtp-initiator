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

#ifndef __MTP_DAEMON_GDBUS_DEVICEINFO_H__
#define __MTP_DAEMON_GDBUS_DEVICEINFO_H__

#include "mtp_daemon.h"

gboolean deviceinfo_get_manufacturername(
		mtpgdbuslibDeviceinfo *device,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean deviceinfo_get_modelname(
		mtpgdbuslibDeviceinfo *device,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean deviceinfo_get_serialnumber(
		mtpgdbuslibDeviceinfo *device,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean deviceinfo_get_deviceversion(
		mtpgdbuslibDeviceinfo *device,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

#endif
