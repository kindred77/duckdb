#include "duckdb/parser/parser.hpp"
#include "duckdb/main/database.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/main/connection.hpp"

using namespace duckdb;

int
main(int iArgs, const char **rgszArgs)
{
	DuckDB db(nullptr);
	duckdb::Connection con(db);
	//con.Query("PREPARE test AS SELECT version();");
	const auto query_plan = con.ExtractPlan("SELECT version()");
	std::cout << "000----" << (int)query_plan->type << std::endl;
	std::cout << "111----" << query_plan->types.size() << std::endl;
	//REQUIRE((query_plan->type == LogicalOperatorType::LOGICAL_EXECUTE));
	//REQUIRE((query_plan->types.size() == 1));
	//REQUIRE((query_plan->types[0].id() == LogicalTypeId::INTEGER));

	duckdb::DuckDB duckdb;
	duckdb::ClientContext context(duckdb.instance);
	//auto binder = duckdb::Binder::CreateBinder(context);
	std::cout << "000" << std::endl;
	duckdb::Parser parser;
	std::cout << "111" << std::endl;
	parser.ParseQuery("select version()");
	for (const auto & stmt : parser.statements) {
		std::cout << stmt->ToString() << std::endl;
		std::cout << "222" << std::endl;
		//auto plan = context.ExtractPlan(stmt->ToString());
	}
	std::cout << "333" << std::endl;
}