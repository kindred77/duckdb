#include "duckdb/parser/parser.hpp"
#include "duckdb/main/database.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/main/connection.hpp"

#include "gpos/base.h"
#include "gpos/common/CAutoP.h"
#include "gpos/memory/CAutoMemoryPool.h"
#include "gpos/task/CWorkerPoolManager.h"
#include "gpos/_api.h"
#include "gpos/io/COstreamFile.h"

#include "gpopt/translate/CTranslatorDuckDBOperatorToExpr.h"

#define AUTO_MEM_POOL(amp) CAutoMemoryPool amp(CAutoMemoryPool::ElcExc)

using namespace duckdb;
using namespace gpos;
using namespace gpopt;

int
main(int iArgs, const char **rgszArgs)
{
	DuckDB db(nullptr);
	duckdb::Connection con(db);
	//con.Query("PREPARE test AS SELECT version();");
	const auto query_plan = con.ExtractPlan("SELECT version()");

	AUTO_MEM_POOL(amp);
	CMemoryPool *mp = amp.Pmp();

	CTranslatorDuckDBOperatorToExpr *translator = GPOS_NEW(mp) CTranslatorDuckDBOperatorToExpr(mp);
	CExpression *expr = translator->PexprTranslateQuery(query_plan.get());

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