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

#include "gpopt/init.h"
#include "gpopt/CommonException.h"
#include "gpopt/translate/CTranslatorDuckDBOperatorToExpr.h"

#include "naucrates/init.h"

#define AUTO_MEM_POOL(amp) CAutoMemoryPool amp(CAutoMemoryPool::ElcExc)

using namespace duckdb;
using namespace gpos;
using namespace gpopt;

bool
IsAbortRequested(void)
{
	// No GP_WRAP_START/END needed here. We just check these global flags,
	// it cannot throw an ereport().
	return false;
}

#define GPOPT_ERROR_BUFFER_SIZE 10 * 1024 * 1024

void execute(void *(*func)(void *), void *func_arg)
{
	struct gpos_init_params params = {IsAbortRequested};

	gpos_init(&params);
	gpdxl_init();
	gpopt_init();
	CHAR *err_buf = (CHAR *) malloc(GPOPT_ERROR_BUFFER_SIZE);
	err_buf[0] = '\0';
	// initialize DXL support
	//InitDXL();
	bool abort_flag = false;

	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	gpos_exec_params params_exe;
	params_exe.func = func;
	params_exe.arg = func_arg;
	params_exe.stack_start = &params_exe;
	params_exe.error_buffer = err_buf;
	params_exe.error_buffer_size = GPOPT_ERROR_BUFFER_SIZE;
	params_exe.abort_requested = &abort_flag;

	// execute task and send log message to server log
	GPOS_TRY
	{
		(void) gpos_exec(&params_exe);
	}
	GPOS_CATCH_EX(ex)
	{
		//LogExceptionMessageAndDelete(err_buf, ex.SeverityLevel());
		GPOS_RETHROW(ex);
	}
	GPOS_CATCH_END;
}

void *optimize(void *ptr)
{
	LogicalOperator *opt = (LogicalOperator *) ptr;
	std::cout << "111----" << std::endl;

	AUTO_MEM_POOL(amp);
	std::cout << "222----" << std::endl;
	CMemoryPool *mp = amp.Pmp();
	gpopt::CommonException::Init(mp);
	//(void) gpopt::EresExceptionInit(mp);
	std::cout << "333----" << std::endl;
	CTranslatorDuckDBOperatorToExpr *translator = GPOS_NEW(mp) CTranslatorDuckDBOperatorToExpr(mp);
	std::cout << "444----" << std::endl;
	CExpression *expr = translator->PexprTranslateQuery(opt);
	std::cout << "555----" << std::endl;
}

int
main(int iArgs, const char **rgszArgs)
{

	std::cout << "000----" << std::endl;
	DuckDB db(nullptr);
	duckdb::Connection con(db);
	//con.Query("PREPARE test AS SELECT version();");
	const auto query_plan = con.ExtractPlan("SELECT version()");

	//optimize(query_plan.get());

	execute(&optimize, query_plan.get());


	std::cout << "555----" << query_plan->types.size() << std::endl;
	//REQUIRE((query_plan->type == LogicalOperatorType::LOGICAL_EXECUTE));
	//REQUIRE((query_plan->types.size() == 1));
	//REQUIRE((query_plan->types[0].id() == LogicalTypeId::INTEGER));

	duckdb::DuckDB duckdb;
	duckdb::ClientContext context(duckdb.instance);
	//auto binder = duckdb::Binder::CreateBinder(context);
	std::cout << "444" << std::endl;
	duckdb::Parser parser;
	std::cout << "555" << std::endl;
	parser.ParseQuery("select version()");
	for (const auto & stmt : parser.statements) {
		std::cout << stmt->ToString() << std::endl;
		std::cout << "666" << std::endl;
		//auto plan = context.ExtractPlan(stmt->ToString());
	}
	std::cout << "777" << std::endl;
}