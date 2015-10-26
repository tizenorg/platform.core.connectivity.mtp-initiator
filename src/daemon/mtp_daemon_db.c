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

#include "mtp_daemon_db.h"

mtp_error_e mtp_daemon_db_delete(int device_handle, int storage_id,
	int object_handle, mtp_context *mtp_ctx)
{
	int ret = MTP_ERROR_NONE;
	int sql_ret;
	char *sql = NULL;
	char *error = NULL;

	if (mtp_ctx->db == NULL) {
		MTP_LOGE("mtp_ctx->db is NULL");
		return MTP_ERROR_DB;
	}

	if (device_handle == 0) {
		MTP_LOGE("device handle is 0");
		return MTP_ERROR_DB;
	}

	if (storage_id == 0 && object_handle == 0) {
		sql = sqlite3_mprintf("DELETE FROM %s WHERE device_handle=%d",
			MTP_DB_TABLE,
			device_handle);
	} else if (storage_id != 0 && object_handle == 0) {
		sql = sqlite3_mprintf("DELETE FROM %s WHERE device_handle=%d and storage_id=%d",
			MTP_DB_TABLE,
			device_handle,
			storage_id);
	} else {
		sql = sqlite3_mprintf("DELETE FROM %s WHERE device_handle=%d and object_handle=%d",
			MTP_DB_TABLE,
			device_handle,
			storage_id,
			object_handle);
	}

	if (sql != NULL) {
		sql_ret = sqlite3_exec(mtp_ctx->db, sql, NULL, NULL, &error);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_exec failed, [%d] : %s", ret, error);
			ret = MTP_ERROR_DB;
			sqlite3_free(error);
		}
		sqlite3_free(sql);
	} else {
		MTP_LOGE("sql is NULL");
		ret = MTP_ERROR_DB;
	}

	return ret;
}

mtp_error_e mtp_daemon_db_update(int device_handle, int object_handle,
	MTPObjectInfo* object_info, mtp_context *mtp_ctx)
{
	int ret = MTP_ERROR_NONE;
	int sql_ret;
	char *sql = NULL;
	char *error = NULL;

	if (mtp_ctx->db == NULL) {
		MTP_LOGE("mtp_ctx->db is NULL");
		return MTP_ERROR_DB;
	}

	sql = sqlite3_mprintf("UPDATE %s SET ObjectFormat=%d, ProtectionStatus=%d, ObjectCompressedSize=%d, ThumbFormat=%d, ThumbCompressedSize=%d, ThumbPixWidth=%d, ThumbPixHeight=%d, ImagePixWidth=%d, ImagePixHeight=%d, ImageBitDepth=%d, ParentObject=%d, AssociationType=%d, AssociationDesc=%d, SequenceNumber=%d WHERE device_handle=%d and object_handle=%d",
		MTP_DB_TABLE,
		object_info->ObjectFormat,
		object_info->ProtectionStatus,
		object_info->ObjectCompressedSize,
		object_info->ThumbFormat,
		object_info->ThumbCompressedSize,
		object_info->ThumbPixWidth,
		object_info->ThumbPixHeight,
		object_info->ImagePixWidth,
		object_info->ImagePixHeight,
		object_info->ImageBitDepth,
		object_info->ParentObject,
		object_info->AssociationType,
		object_info->AssociationDesc,
		object_info->SequenceNumber,
		device_handle,
		object_handle);

	if (sql != NULL) {
		sql_ret = sqlite3_exec(mtp_ctx->db, sql, NULL, NULL, &error);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_exec failed, [%d] : %s", ret, error);
			ret = MTP_ERROR_DB;
			sqlite3_free(error);
		}
		sqlite3_free(sql);
	} else {
		MTP_LOGE("sql is NULL");
		ret = MTP_ERROR_DB;
	}

	return ret;
}

MTPObjectInfo* mtp_daemon_db_get_object_info(int device_handle,
	int object_handle, mtp_context *mtp_ctx)
{
	int sql_ret;
	char *sql = NULL;
	MTPObjectInfo* object_info;
	sqlite3_stmt *stmt = NULL;

	if (mtp_ctx->db == NULL) {
		MTP_LOGE("mtp_ctx->db is NULL");
		return NULL;
	}

	object_info = (MTPObjectInfo *)malloc(sizeof(MTPObjectInfo));

	sql = sqlite3_mprintf("SELECT * FROM %s WHERE device_handle=%d and object_handle=%d;",
		MTP_DB_TABLE, device_handle, object_handle);

	if (sql != NULL && object_info != NULL) {
		sql_ret = sqlite3_prepare_v2(mtp_ctx->db, sql, strlen(sql), &stmt, NULL);
		if (stmt == NULL) {
			MTP_LOGE("sqlite3_prepare_v2 failed, [%d], %s", sql_ret, sqlite3_errmsg(mtp_ctx->db));
			g_free(object_info);
			return NULL;
		}

		sql_ret = sqlite3_step(stmt);
		if (sql_ret != SQLITE_ROW) {
			MTP_LOGE("sqlite3_step failed, [%d]", sql_ret);
			g_free(object_info);
			return NULL;
		}

		object_info->StorageID = sqlite3_column_int(stmt, 2);
		object_info->ObjectFormat = sqlite3_column_int(stmt, 4);
		object_info->ProtectionStatus = sqlite3_column_int(stmt, 5);
		object_info->ObjectCompressedSize = sqlite3_column_int(stmt, 6);
		object_info->ThumbFormat = sqlite3_column_int(stmt, 7);
		object_info->ThumbCompressedSize = sqlite3_column_int(stmt, 8);
		object_info->ThumbPixWidth = sqlite3_column_int(stmt, 9);
		object_info->ThumbPixHeight = sqlite3_column_int(stmt, 10);
		object_info->ImagePixWidth = sqlite3_column_int(stmt, 11);
		object_info->ImagePixHeight = sqlite3_column_int(stmt, 12);
		object_info->ImageBitDepth = sqlite3_column_int(stmt, 13);
		object_info->ParentObject = sqlite3_column_int(stmt, 14);
		object_info->AssociationType = sqlite3_column_int(stmt, 15);
		object_info->AssociationDesc = sqlite3_column_int(stmt, 16);
		object_info->SequenceNumber = sqlite3_column_int(stmt, 17);
		object_info->Filename = g_strdup(sqlite3_column_text(stmt, 18));
		object_info->CaptureDate = sqlite3_column_int(stmt, 19);
		object_info->ModificationDate = sqlite3_column_int(stmt, 20);
		object_info->Keywords = g_strdup(sqlite3_column_text(stmt, 21));

		MTP_LOGE("object_info->Filename : %s, %ld, object_info->ModificationDate : %ld",
			object_info->Filename, (long)object_info->CaptureDate, (long)object_info->ModificationDate);

		sqlite3_finalize(stmt);
		sqlite3_free(sql);
	} else {
		g_free(object_info);
		return NULL;
	}

	return object_info;
}

/* db exist check function */
bool mtp_daemon_db_is_exist(int device_handle, int object_handle, mtp_context *mtp_ctx)
{
	int sql_ret;
	int count = 0;
	char *sql = NULL;
	sqlite3_stmt *stmt = NULL;

	if (mtp_ctx->db == NULL) {
		MTP_LOGE("mtp_ctx->db is NULL");
		return false;
	}

	sql = sqlite3_mprintf("SELECT count(*) FROM %s WHERE device_handle=%d and object_handle=%d;",
		MTP_DB_TABLE, device_handle, object_handle);

	if (sql != NULL) {
		sql_ret = sqlite3_prepare_v2(mtp_ctx->db, sql, strlen(sql), &stmt, NULL);
		if (stmt == NULL) {
			MTP_LOGE("sqlite3_prepare_v2 failed, [%d], %s", sql_ret, sqlite3_errmsg(mtp_ctx->db));
			return false;
		}

		sql_ret = sqlite3_step(stmt);
		if (sql_ret != SQLITE_ROW) {
			MTP_LOGE("sqlite3_step failed, [%d]", sql_ret);
			return false;
		}

		count = sqlite3_column_int(stmt, 0);

		MTP_LOGE("device : %d, object_id : %d, count is [%d]", device_handle,
			object_handle, count);

		sqlite3_finalize(stmt);
		sqlite3_free(sql);
	} else {
		return false;
	}

	return (count > 0) ? true : false;
}

/* db insert is called where get_object_handles function. */
/* every ptp protocol "get object info" call, called this function. */
mtp_error_e mtp_daemon_db_insert(int device_handle, int storage_id,
	int object_handle, MTPObjectInfo* object_info, mtp_context *mtp_ctx)
{
	int ret = MTP_ERROR_NONE;
	int sql_ret;
	char *sql = NULL;
	char *error = NULL;

	if (mtp_ctx->db == NULL) {
		MTP_LOGE("mtp_ctx->db is NULL");
		return MTP_ERROR_DB;
	}

	sql = sqlite3_mprintf("INSERT INTO %s (device_handle, storage_id, object_handle, ObjectFormat, ProtectionStatus, ObjectCompressedSize, ThumbFormat, ThumbCompressedSize, ThumbPixWidth, ThumbPixHeight, ImagePixWidth, ImagePixHeight, ImageBitDepth, ParentObject, AssociationType, AssociationDesc, SequenceNumber, Filename, CaptureDate, ModificationDate) values(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %Q, %ld, %ld);",
		MTP_DB_TABLE,
		device_handle,
		storage_id,
		object_handle,
		(int)object_info->ObjectFormat,
		(int)object_info->ProtectionStatus,
		(int)object_info->ObjectCompressedSize,
		(int)object_info->ThumbFormat,
		(int)object_info->ThumbCompressedSize,
		(int)object_info->ThumbPixWidth,
		(int)object_info->ThumbPixHeight,
		(int)object_info->ImagePixWidth,
		(int)object_info->ImagePixHeight,
		(int)object_info->ImageBitDepth,
		(int)object_info->ParentObject,
		(int)object_info->AssociationType,
		(int)object_info->AssociationDesc,
		(int)object_info->SequenceNumber,
		object_info->Filename,
		(long)object_info->CaptureDate,
		(long)object_info->ModificationDate);

	if (sql != NULL) {
		sql_ret = sqlite3_exec(mtp_ctx->db, sql, NULL, NULL, &error);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_exec failed, [%d] : %s", ret, error);
			ret = MTP_ERROR_DB;
			sqlite3_free(error);
		}
		sqlite3_free(sql);
	} else {
		MTP_LOGE("sql is NULL");
		ret = MTP_ERROR_DB;
	}

	return ret;
}

static mtp_error_e __create_table(mtp_context *mtp_ctx)
{
	int ret = MTP_ERROR_NONE;
	int sql_ret;
	char *sql = NULL;
	char *error = NULL;

	if (mtp_ctx->db == NULL) {
		MTP_LOGE("mtp_ctx->db is NULL");
		return MTP_ERROR_DB;
	}

	sql = sqlite3_mprintf("CREATE TABLE %s(id INTEGER PRIMARY KEY, device_handle INTEGER, storage_id INTEGER, object_handle INTEGER, ObjectFormat INTEGER, ProtectionStatus INTEGER, ObjectCompressedSize INTEGER, ThumbFormat INTEGER, ThumbCompressedSize INTEGER, ThumbPixWidth INTEGER, ThumbPixHeight INTEGER, ImagePixWidth INTEGER, ImagePixHeight INTEGER, ImageBitDepth INTEGER, ParentObject INTEGER, AssociationType INTEGER, AssociationDesc INTEGER, SequenceNumber INTEGER, Filename TEXT, CaptureDate INTEGER, ModificationDate INTEGER, Keywords TEXT);",
		MTP_DB_TABLE);

	if (sql != NULL) {
		sql_ret = sqlite3_exec(mtp_ctx->db, sql, NULL, NULL, &error);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_exec failed, [%d] : %s", ret, error);
			ret = MTP_ERROR_DB;
			sqlite3_free(error);
		}
		sqlite3_free(sql);
	} else {
		MTP_LOGE("sql is NULL");
		ret = MTP_ERROR_DB;
	}

	return ret;
}

mtp_error_e mtp_daemon_db_init(mtp_context *mtp_ctx)
{
	int ret = MTP_ERROR_NONE;
	int sql_ret;
	char *error = NULL;

	if (mtp_ctx->db == NULL) {
		sql_ret = sqlite3_open_v2(MTP_DB_FILE, &mtp_ctx->db,
			SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_open_v2 failed, [%d]", sql_ret);
			return MTP_ERROR_DB;
		}

		sql_ret = sqlite3_exec(mtp_ctx->db, "PRAGMA journal_mode = PERSIST",
			NULL, NULL, &error);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_exec failed, [%d] : %s", sql_ret, error);
			if (mtp_ctx->db != NULL) {
					sql_ret = sqlite3_close(mtp_ctx->db);
					if (sql_ret == SQLITE_OK)
						mtp_ctx->db = NULL;
			}
			sqlite3_free(error);
			return MTP_ERROR_DB;
		}

		ret = __create_table(mtp_ctx);
	}

	return ret;
}

mtp_error_e mtp_daemon_db_deinit(mtp_context *mtp_ctx)
{
	int ret = MTP_ERROR_NONE;
	int sql_ret;

	if (mtp_ctx->db != NULL) {
		sql_ret = sqlite3_close(mtp_ctx->db);
		if (sql_ret != SQLITE_OK) {
			MTP_LOGE("sqlite3_close failed, [%d]", sql_ret);
			return MTP_ERROR_DB;
		}
		mtp_ctx->db = NULL;
	}

	return ret;
}
