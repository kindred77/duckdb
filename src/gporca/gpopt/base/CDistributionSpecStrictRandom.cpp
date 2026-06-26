//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "gpopt/base/CDistributionSpecStrictRandom.h"

using namespace gpopt;

CDistributionSpecStrictRandom::CDistributionSpecStrictRandom()
{
}

GP_BOOL
CDistributionSpecStrictRandom::Matches(const CDistributionSpec *pds) const
{
	return pds->Edt() == Edt();
}

GP_BOOL
CDistributionSpecStrictRandom::FSatisfies(const CDistributionSpec *pds) const
{
	return Matches(pds) || EdtAny == pds->Edt() || EdtRandom == pds->Edt() ||
		   EdtNonSingleton == pds->Edt();
}
