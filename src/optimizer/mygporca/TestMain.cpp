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
	InitDXL();
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
		std::cout << "---execute-----" << std::endl;
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
	std::cout << "optimize----111----" << std::endl;

	AUTO_MEM_POOL(amp);
	std::cout << "optimize----222----" << std::endl;
	CMemoryPool *mp = amp.Pmp();
	gpopt::CommonException::Init(mp);
	//(void) gpopt::EresExceptionInit(mp);
	std::cout << "optimize----333----" << std::endl;
	CTranslatorDuckDBOperatorToExpr *translator = GPOS_NEW(mp) CTranslatorDuckDBOperatorToExpr(mp);
	std::cout << "optimize----444----" << std::endl;
	CExpression *expr = translator->PexprTranslateQuery(opt);
	std::cout << "optimize----555----" << std::endl;
	return NULL;
}

void testBit(uint64_t value_in)
{
	uint64_t value = value_in;

	constexpr uint64_t index64lsb[] = {63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,
	                                   61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,
	                                   62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21,
	                                   56, 45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5};
	constexpr uint64_t debruijn64lsb = 0x07EDD5E59A4E28C2ULL;
	auto result = index64lsb[((value & -value) * debruijn64lsb) >> 58];

	std::cout << "----" << result << "------" << static_cast<uint64_t>(__builtin_ctzl(value_in)) << std::endl;
}

int
main(int iArgs, const char **rgszArgs)
{
//	testBit(12345678919837);
	std::cout << "main----000----" << std::endl;
	DuckDB db(nullptr);
	duckdb::Connection con(db);
	con.Query("create table t(id int)");
	con.Query("insert into t select * from generate_series(10000)");
	std::cout << "main----111----" << std::endl;
	const auto &result = con.Query("SELECT sum(id) from t where id%2=0");
	std::cout << "main----222----" << std::endl;
	std::cout << result->ToString() << std::endl;
	std::cout << "main----333----" << std::endl;
	const auto query_plan = con.ExtractPlan("SELECT sum(id) from t");
	std::cout << "main----444----" << std::endl;
	//optimize(query_plan.get());

	execute(&optimize, query_plan.get());


	std::cout << "main----555----" << query_plan->types.size() << std::endl;
	//REQUIRE((query_plan->type == LogicalOperatorType::LOGICAL_EXECUTE));
	//REQUIRE((query_plan->types.size() == 1));
	//REQUIRE((query_plan->types[0].id() == LogicalTypeId::INTEGER));

	duckdb::DuckDB duckdb;
	duckdb::ClientContext context(duckdb.instance);
	//auto binder = duckdb::Binder::CreateBinder(context);
	std::cout << "main----666" << std::endl;
	duckdb::Parser parser;
	std::cout << "main----777" << std::endl;
	parser.ParseQuery("select version()");
	for (const auto & stmt : parser.statements) {
		std::cout << stmt->ToString() << std::endl;
		std::cout << "main----888" << std::endl;
		//auto plan = context.ExtractPlan(stmt->ToString());
	}
	std::cout << "main----999" << std::endl;
}