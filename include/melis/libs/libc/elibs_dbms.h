/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : dbms.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms wappper interface header file.
* Update  : date                auther      ver     notes
*           2010-8-21 10:24:47  Sunny       1.0     cretae this file.
*********************************************************************************************************
*/

#ifndef     __DBMS_H__
#define     __DBMS_H__

/* error codes */
#define DB_ERROR_BADOBJECT      (-8)
#define DB_ERROR_BADTYPE        (-7)
#define DB_ERROR_BADIDX         (-6)
#define DB_ERROR_BADNAME        (-5)
#define DB_ERROR_UNSUPPORTED    (-4)
#define DB_ERROR_NOCONN         (-3)
#define DB_ERROR_NOMEM          (-2)
#define DB_ERROR_BADPTR         (-1)
#define DB_ERROR_NONE           (0)
/* from 0 begin is the SQLite error number, detail see mod_dbms.h */


/* column data types */
#define COLUMN_TYPE_ERROR     0         /* invalid data type */
#define COLUMN_TYPE_INTEGER   1
#define COLUMN_TYPE_DECIMAL   2
#define COLUMN_TYPE_STRING    3
#define COLUMN_TYPE_BINARY    4
#define COLUMN_TYPE_NULL      5

/* database connection query work mode */
#define DBCONN_QUERY_ALL      1
#define DBCONN_QUERY_STEP     2

/* functions with a return type of size_t return this in case of an
   error if 0 is a valid return value */
#define DB_LENGTH_ERROR       (0xFFFFFFFF)

/* handle of database connection */
typedef void *HDBConn;

/* handle of database query result cursor */
typedef void *HDBCursor;

/*
 *------------------------------------------------------------------------------
 *
 *                      database libary init and exit
 *
 *------------------------------------------------------------------------------
 */
int         DBLibary_Init(void);
int         DBLibary_Exit(void);
const char *DBLibary_Vesion(void);

/*
 *------------------------------------------------------------------------------
 *
 *                      database connection operations
 *
 *------------------------------------------------------------------------------
 */
int     DB_Create(const char *dbname);
int     DB_Delete(const char *dbname);
HDBConn DB_Connect(const char *dbname);
int     DB_Disconnect(HDBConn conn);

int       DBConn_SetQueryMode(HDBConn conn, int mode, int RowsNumber);
HDBCursor DBConn_GetTableList(HDBConn conn, const char *pattern);
HDBCursor DBConn_Query(HDBConn conn, __bool distinct, const char *table,
                       const char *columns, const char *pattern);
int       DBConn_Execute(HDBConn conn, const char *statement);

int DBConn_BeginTransaction(HDBConn conn);
int DBConn_CommitTransaction(HDBConn conn);
int DBConn_RollbackTransaction(HDBConn conn);

int DBConn_LastInsertRowid(HDBConn conn);
int DBConn_RowsChanged(HDBConn conn);


/*
 *------------------------------------------------------------------------------
 *
 *                      database connection error handling
 *
 *------------------------------------------------------------------------------
 */
const char *DBConn_ErrorMsg(HDBConn conn);
int         DBConn_ErrorCode(HDBConn conn);


/*
 *------------------------------------------------------------------------------
 *
 *                      query reslut cursor management
 *
 *------------------------------------------------------------------------------
 */
HDBConn DBCursor_GetConnect(HDBCursor cursor);
int     DBCursor_GetRowsNumber(HDBCursor cursor);
int     DBCursor_Move(HDBCursor cursor, int offset);
int     DBCursor_MoveToPosition(HDBCursor cursor, int rowIdx);
int     DBCursor_MoveToFirst(HDBCursor cursor);
int     DBCursor_MoveToLast(HDBCursor cursor);
int     DBCursor_MoveToPrev(HDBCursor cursor);
int     DBCursor_MoveToNext(HDBCursor cursor);
int     DBCursor_GetCurRowIdx(HDBCursor cursor);
int     DBCursor_IsFirst(HDBCursor cursor);
int     DBCursor_IsLast(HDBCursor cursor);
int     DBCursor_Free(HDBCursor cursor);


/*
 *------------------------------------------------------------------------------
 *
 *                      Retrieving column data
 *
 *------------------------------------------------------------------------------
 */
int         DBCursor_GetColumnsNumber(HDBCursor cursor);
int         DBCursor_GetColumnIdx(HDBCursor cursor, const char *columnName);
const char *DBCursor_GetColumnName(HDBCursor cursor, int columnIdx);

int         DBCursor_GetColumnTypeByIdx(HDBCursor cursor, int columnIdx);
size_t      DBCursor_GetColumnLengthByIdx(HDBCursor cursor, int columnIdx);
int         DBCursor_GetIntByIdx(HDBCursor cursor, int columnIdx);
__s64       DBCursor_GetInt64ByIdx(HDBCursor cursor, int columnIdx);
double      DBCursor_GetDoubleByIdx(HDBCursor cursor, int columnIdx);
const char *DBCursor_GetStringByIdx(HDBCursor cursor, int columnIdx);
const void *DBCursor_GetBinaryByIdx(HDBCursor cursor, int columnIdx);

int         DBCursor_GetColumnType(HDBCursor cursor, const char *columnName);
size_t      DBCursor_GetColumnLength(HDBCursor cursor, const char *columnName);
int         DBCursor_GetInt(HDBCursor cursor, const char *columnName);
__s64       DBCursor_GetInt64(HDBCursor cursor, const char *columnName);
double      DBCursor_GetDouble(HDBCursor cursor, const char *columnName);
const char *DBCursor_GetString(HDBCursor cursor, const char *columnName);
const void *DBCursor_GetBinary(HDBCursor cursor, const char *columnName);


/*
 *------------------------------------------------------------------------------
 *
 *                          insert row
 *
 *------------------------------------------------------------------------------
 */
int DBCursor_InsertRow(HDBCursor cursor, const char *format, ...);
int DBCursor_BindBinary(HDBCursor cursor, int rowIdx, int columnIdx, const void *binary, size_t size);
int DBCursor_BindString(HDBCursor cursor, int rowIdx, int columnIdx, const char *string);
int DBCursor_BindDouble(HDBCursor cursor, int rowIdx, int columnIdx, double value);
int DBCursor_BindInt(HDBCursor cursor, int rowIdx, int columnIdx, int value);
int DBCursor_BindInt64(HDBCursor cursor, int rowIdx, int columnIdx, __s64 value);
int DBCursor_CommitInsert(HDBCursor cursor, int RowIdx);


/*
 *------------------------------------------------------------------------------
 *
 *                          delete row
 *
 *------------------------------------------------------------------------------
 */
int DBCursor_DeleteCurrentRow(HDBCursor cursor);
int DBCursor_DeleteRowByIdx(HDBCursor cursor, int rowIdx);


/*
 *------------------------------------------------------------------------------
 *
 *                          update row
 *
 *------------------------------------------------------------------------------
 */
int DBCursor_UpdateIntByIdx(HDBCursor cursor, int columnIdx, int value);
int DBCursor_UpdateInt64ByIdx(HDBCursor cursor, int columnIdx, __s64 value);
int DBCursor_UpdateDoubleByIdx(HDBCursor cursor, int columnIdx, double value);
int DBCursor_UpdateStringByIdx(HDBCursor cursor, int columnIdx, const char *string);
int DBCursor_UpdateBinaryByIdx(HDBCursor cursor, int columnIdx, void *binary, size_t size);

int DBCursor_UpdateInt(HDBCursor cursor, const char *columnName, int value);
int DBCursor_UpdateInt64(HDBCursor cursor, const char *columnName, __s64 value);
int DBCursor_UpdateDouble(HDBCursor cursor, const char *columnName, double value);
int DBCursor_UpdateString(HDBCursor cursor, const char *columnName, const char *string);
int DBCursor_UpdateBinary(HDBCursor cursor, const char *columnName, void *binary, size_t size);
int DBCursor_CommitUpdate(HDBCursor cursor);

#endif      /* __DBMS_H__ */
