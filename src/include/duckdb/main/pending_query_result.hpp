//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/main/pending_query_result.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/main/query_result.hpp"
#include "duckdb/execution/executor.hpp"

namespace duckdb {
class ClientContext;
class ClientContextLock;
class PreparedStatementData;

enum class PendingExecutionResult : uint8_t {
	RESULT_READY,
	RESULT_NOT_READY,
	EXECUTION_ERROR
};

class PendingQueryResult : public BaseQueryResult {
	friend class ClientContext;

public:
	DUCKDB_API PendingQueryResult(shared_ptr<ClientContext> context, shared_ptr<PreparedStatementData> statement, unique_ptr<Executor> executor, vector<LogicalType> types);
	DUCKDB_API explicit PendingQueryResult(string error_message);
	DUCKDB_API ~PendingQueryResult();

public:
	//! Executes a single task within the query, returning whether or not the query is ready.
	//! If this returns RESULT_READY, the Execute function can be called to obtain a pointer to the result.
	//! If this returns RESULT_NOT_READY, the ExecuteTask function should be called again.
	//! If this returns EXECUTION_ERROR, an error occurred during execution.
	//! The error message can be obtained by calling GetError() on the PendingQueryResult.
	DUCKDB_API PendingExecutionResult ExecuteTask();

	//! Returns the result of the query as an actual query result.
	//! This returns (mostly) instantly if ExecuteTask has been called until RESULT_READY was returned.
	DUCKDB_API unique_ptr<QueryResult> Execute(bool allow_streaming_result = false);

	DUCKDB_API void Close();

	DUCKDB_API void MarkAsClosed() override;

private:
	shared_ptr<ClientContext> context;
	//! Prepared statement data
	shared_ptr<PreparedStatementData> statement;
	//! Whether or not the PendingQueryResult is still open
	bool is_open;
	//! The current query being executed
	string query;
	//! The query executor
	unique_ptr<Executor> executor;

private:
	void CheckExecutable();
	unique_ptr<QueryResult> ExecuteInternal(ClientContextLock &lock, bool allow_streaming_result = false);
};

} // namespace duckdb
