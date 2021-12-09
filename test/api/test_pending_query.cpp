#include "catch.hpp"
#include "test_helpers.hpp"

using namespace duckdb;
using namespace std;

TEST_CASE("Test Pending Query API", "[api]") {
	DuckDB db;
	Connection con(db);

	SECTION("Materialized result") {
		auto pending_query = con.PendingQuery("SELECT SUM(i) FROM range(1000000) tbl(i)");
		REQUIRE(pending_query->success);
		auto result = pending_query->Execute();
		REQUIRE(CHECK_COLUMN(result, 0, {Value::BIGINT(499999500000)}));

		// cannot fetch twice from the same pending query
		REQUIRE_THROWS(pending_query->Execute());
		REQUIRE_THROWS(pending_query->Execute(true));

		// query the connection as normal after
		result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
	SECTION("Streaming result") {
		auto pending_query = con.PendingQuery("SELECT SUM(i) FROM range(1000000) tbl(i)");
		REQUIRE(pending_query->success);
		auto result = pending_query->Execute(true);
		REQUIRE(CHECK_COLUMN(result, 0, {Value::BIGINT(499999500000)}));

		// cannot fetch twice from the same pending query
		REQUIRE_THROWS(pending_query->Execute(true));
		REQUIRE_THROWS(pending_query->Execute());

		// query the connection as normal after
		result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
	SECTION("Execute tasks") {
		auto pending_query = con.PendingQuery("SELECT SUM(i) FROM range(1000000) tbl(i)");
		while(pending_query->ExecuteTask() == PendingExecutionResult::RESULT_NOT_READY);
		REQUIRE(pending_query->success);
		auto result = pending_query->Execute(true);
		REQUIRE(CHECK_COLUMN(result, 0, {Value::BIGINT(499999500000)}));

		// cannot fetch twice from the same pending query
		REQUIRE_THROWS(pending_query->Execute(true));

		// query the connection as normal after
		result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
	SECTION("Create pending query while another pending query exists") {
		auto pending_query = con.PendingQuery("SELECT SUM(i) FROM range(1000000) tbl(i)");
		auto pending_query2 = con.PendingQuery("SELECT SUM(i) FROM range(1000000) tbl(i)");

		// first pending query is now closed
		REQUIRE_THROWS(pending_query->ExecuteTask());
		REQUIRE_THROWS(pending_query->Execute());

		// we can execute the second one
		auto result = pending_query2->Execute(true);
		REQUIRE(CHECK_COLUMN(result, 0, {Value::BIGINT(499999500000)}));

		// query the connection as normal after
		result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
	SECTION("Binding error in pending query") {
		auto pending_query = con.PendingQuery("SELECT XXXSUM(i) FROM range(1000000) tbl(i)");
		REQUIRE(!pending_query->success);
		REQUIRE_THROWS(pending_query->ExecuteTask());
		REQUIRE_THROWS(pending_query->Execute());

		// query the connection as normal after
		auto result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
	SECTION("Runtime error in pending query (materialized)") {
		// this succeeds initially
		auto pending_query = con.PendingQuery("SELECT concat(SUM(i)::varchar, 'hello')::INT FROM range(1000000) tbl(i)");
		REQUIRE(pending_query->success);
		// we only encounter the failure later on as we are executing the query
		auto result = pending_query->Execute();
		REQUIRE_FAIL(result);

		// query the connection as normal after
		result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
	SECTION("Runtime error in pending query (streaming)") {
		// this succeeds initially
		auto pending_query = con.PendingQuery("SELECT concat(SUM(i)::varchar, 'hello')::INT FROM range(1000000) tbl(i)");
		REQUIRE(pending_query->success);
		// still succeeds...
		auto result = pending_query->Execute(true);
		REQUIRE(result->success);
		auto chunk = result->Fetch();
		REQUIRE(!chunk);
		REQUIRE(!result->success);

		// query the connection as normal after
		result = con.Query("SELECT 42");
		REQUIRE(CHECK_COLUMN(result, 0, {42}));
	}
}
