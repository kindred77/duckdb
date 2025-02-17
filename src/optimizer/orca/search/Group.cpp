#include "duckdb/optimizer/orca/search/Group.hpp"

namespace duckdb {

Group::Group(bool fScalar) {

}

OptimizationContextPtr
Group::PocInsert(OptimizationContextPtr poc)
{
	ShtAcc shta(Sht(), *poc);

	COptimizationContext *pocFound = shta.Find();
	if (nullptr == pocFound)
	{
		poc->SetId((ULONG) UlpIncOptCtxts());
		shta.Insert(poc);
		return poc;
	}

	return pocFound;
}

}
