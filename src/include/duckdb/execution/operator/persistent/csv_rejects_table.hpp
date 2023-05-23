#pragma once

#include "duckdb.hpp"
#ifndef DUCKDB_AMALGAMATION
#include "duckdb/storage/object_cache.hpp"
#endif

namespace duckdb {

struct ReadCSVData;

class CSVRejectsTable : public ObjectCacheEntry {
public:
	CSVRejectsTable() {
	}
	~CSVRejectsTable() override = default;
	mutex write_lock;
	idx_t count = 0;

	static shared_ptr<CSVRejectsTable> GetOrCreate(ClientContext &context);

	void InitializeTable(ClientContext &context, const ReadCSVData &options, const string &name);
	TableCatalogEntry &GetTable(ClientContext &context, const string &name);

public:
	static string ObjectType() {
		return "csv_rejects_table_cache";
	}

	string GetObjectType() override {
		return ObjectType();
	}
};

} // namespace duckdb
