#pragma once

#include "duckdb/common/types.hpp"
#include "duckdb/common/types/string_type.hpp"
#include "duckdb/common/types/string_heap.hpp"
#include "duckdb.hpp"

#include <memory>
#include <string>

#include "sqlite3.h"
#include "sqlite3_value_type.hpp"

// it was moved to here because the UDF API must know the structure members
struct sqlite3 {
	std::unique_ptr<duckdb::DuckDB> db;
	std::unique_ptr<duckdb::Connection> con;
	std::string last_error;
	int64_t last_changes = 0;
	int64_t total_changes = 0;
	int errCode; /* Most recent error code (SQLITE_*) */
};

struct sqlite3_value {
	union MemValue {
		double r;  /* Real value used when MEM_Real is set in flags */
		int64_t i; /* Integer value used when MEM_Int is set in flags */
		           // int nZero;          /* Extra zero bytes when MEM_Zero and MEM_Blob set */
		           // const char *zPType; /* Pointer type when MEM_Term|MEM_Subtype|MEM_Null */
	} u;
	duckdb::SQLiteTypeValue type;

	int n;                  /* Number of characters in string value, excluding '\0' */
	duckdb::string_t str_t; /* String or BLOB value */
	char *zMalloc;          /* Space to hold MEM_Str or MEM_Blob if szMalloc>0 */
	int szMalloc = 0;       /* Size of the zMalloc allocation */
	sqlite3 *db;            /* The associated database connection */
};

struct FuncDef {
	//   i8 nArg;             /* Number of arguments.  -1 means unlimited */
	//   u32 funcFlags;       /* Some combination of SQLITE_FUNC_* */
	void *pUserData; /* User data parameter */
	                 //   FuncDef *pNext;      /* Next function with same name */
	                 //   void (*xSFunc)(sqlite3_context*,int,sqlite3_value**); /* func or agg-step */
	                 //   void (*xFinalize)(sqlite3_context*);                  /* Agg finalizer */
	                 //   void (*xValue)(sqlite3_context*);                     /* Current agg value */
	                 //   void (*xInverse)(sqlite3_context*,int,sqlite3_value**); /* inverse agg-step */
	                 //   const char *zName;   /* SQL name of the function. */
	                 //   union {
	                 //     FuncDef *pHash;      /* Next with a different name but the same hash */
	                 //     FuncDestructor *pDestructor;   /* Reference counted destructor function */
	                 //   } u;
};

struct sqlite3_context {
	sqlite3_value result; // Mem *pOut;              /* The return value is stored here */
	FuncDef pFunc;        /* Pointer to function information */
	                      //   Mem *pMem;              /* Memory cell used to store aggregate context */
	                      //   Vdbe *pVdbe;            /* The VM that owns this context */
	                      //   int iOp;                /* Instruction number of OP_Function */
	int isError;          /* Error code returned by the function. */
	                      //   u8 skipFlag;            /* Skip accumulator loading if true */
	                      //   u8 argc;                /* Number of arguments */
	                      //   sqlite3_value *argv[1]; /* Argument set */
};
