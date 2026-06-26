//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2011 EMC Corp.
//
//	@filename:
//		CPredicateUtils.h
//
//	@doc:
//		Utility functions for normalizing scalar predicates
//---------------------------------------------------------------------------
#ifndef GPOPT_CPredicateUtils_H
#define GPOPT_CPredicateUtils_H

#include "gpos/base.h"

#include "gpopt/base/CUtils.h"
#include "gpopt/mdcache/CMDAccessor.h"
#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CScalarBoolOp.h"

namespace gpopt
{
using namespace gpos;

// fwd decl
class CConstraintInterval;

//---------------------------------------------------------------------------
//	@class:
//		CPredicateUtils
//
//	@doc:
//		Utility functions for normalizing scalar predicates
//
//---------------------------------------------------------------------------
class CPredicateUtils
{
private:
	// collect conjuncts recursively
	static void CollectConjuncts(CExpression *pexpr,
								 CExpressionArray *pdrgpexpr);

	// collect disjuncts recursively
	static void CollectDisjuncts(CExpression *pexpr,
								 CExpressionArray *pdrgpexpr);

	// check if a conjunct/disjunct can be skipped
	static GP_BOOL FSkippable(CExpression *pexpr, GP_BOOL fConjunction);

	// check if a conjunction/disjunction can be reduced to a constant True/False
	static GP_BOOL FReducible(CExpression *pexpr, GP_BOOL fConjunction);

	// check if given expression is a comparison over the given column
	static GP_BOOL FComparison(CExpression *pexpr, CColRef *colref,
							CColRefSet *pcrsAllowedRefs);

	// check whether the given expression contains references to only the given
	// columns. If pcrsAllowedRefs is NULL, then check whether the expression has
	// no column references and no volatile functions
	static GP_BOOL FValidRefsOnly(CExpression *pexprScalar,
							   CColRefSet *pcrsAllowedRefs);

	// determine which predicates we should test implication for
	static GP_BOOL FCheckPredicateImplication(CExpression *pexprPred);

	// check if predicate is implied by given equivalence classes
	static GP_BOOL FImpliedPredicate(CExpression *pexprPred,
								  CColRefSetArray *pdrgpcrsEquivClasses);

	// helper to create index lookup comparison predicate with index key on left side
	static CExpression *PexprIndexLookupKeyOnLeft(CMemoryPool *mp,
												  CMDAccessor *md_accessor,
												  CExpression *pexprScalar,
												  const IMDIndex *pmdindex,
												  CColRefArray *pdrgpcrIndex,
												  CColRefSet *outer_refs);

	// helper to create index lookup comparison predicate with index key on right side
	static CExpression *PexprIndexLookupKeyOnRight(CMemoryPool *mp,
												   CMDAccessor *md_accessor,
												   CExpression *pexprScalar,
												   const IMDIndex *pmdindex,
												   CColRefArray *pdrgpcrIndex,
												   CColRefSet *outer_refs);

	// for all columns that appear in the given expression and are members
	// of the given set, replace these columns with NULL constants
	static CExpression *PexprReplaceColsWithNulls(CMemoryPool *mp,
												  CExpression *pexprScalar,
												  CColRefSet *pcrs);

	// private ctor
	CPredicateUtils();

	// private dtor
	virtual ~CPredicateUtils();

	// private copy ctor
	CPredicateUtils(const CPredicateUtils &);

public:
	// reverse the comparison, for example "<" => ">", "<=" => "=>
	static IMDType::ECmpType EcmptReverse(IMDType::ECmpType cmp_type);

	// check if the expression is a boolean scalar identifier
	static GP_BOOL FBooleanScalarIdent(CExpression *pexprPred);

	// check if the expression is a negated boolean scalar identifier
	static GP_BOOL FNegatedBooleanScalarIdent(CExpression *pexprPred);

	// is the given expression an equality comparison
	static GP_BOOL IsEqualityOp(CExpression *pexpr);

	// is the given expression a scalar comparison
	static GP_BOOL FComparison(CExpression *pexpr);

	// is the given expression a disjunction of equality comparisons
	static GP_BOOL FDisjunctionOfIdentEqComparisons(CMemoryPool *mp,
												 CExpression *pexpr,
												 CColRef *colref);

	// is the given expression a comparison of the given type
	static GP_BOOL FComparison(CExpression *pexpr, IMDType::ECmpType cmp_type);

	// is the given array of expressions contain a volatile function like random()
	static GP_BOOL FContainsVolatileFunction(CExpressionArray *pdrgpexpr);

	// is the given expressions contain a volatile function like random()
	static GP_BOOL FContainsVolatileFunction(CExpression *pexpr);

	// is the given expression an equality comparison over two scalar identifiers
	static GP_BOOL FPlainEquality(CExpression *pexpr);

	// is the given expression a self comparison on some column
	static GP_BOOL FSelfComparison(CExpression *pexpr, IMDType::ECmpType *pecmpt);

	// eliminate self comparison if possible
	static CExpression *PexprEliminateSelfComparison(CMemoryPool *mp,
													 CExpression *pexpr);

	// is the given expression in the form (col1 Is NOT DISTINCT FROM col2)
	static GP_BOOL FINDFScalarIdents(CExpression *pexpr);

	// is the given expression in the form (col1 IS DISTINCT FROM col2)
	static GP_BOOL FIDFScalarIdents(CExpression *pexpr);

	// check if expression is an Is DISTINCT FROM FALSE expression
	static GP_BOOL FIDFFalse(CExpression *pexpr);

	static GP_BOOL FCompareScalarIdentToConstAndScalarIdentArray(
		CExpression *pexpr);

	// is the given expression in the form (expr IS DISTINCT FROM expr)
	static GP_BOOL FIDF(CExpression *pexpr);

	// is the given expression in the form (expr IS NOT DISTINCT FROM expr)
	static GP_BOOL FINDF(CExpression *pexpr);

	// generate a conjunction of INDF expressions between corresponding columns in the given arrays
	static CExpression *PexprINDFConjunction(CMemoryPool *mp,
											 CColRefArray *pdrgpcrFirst,
											 CColRefArray *pdrgpcrSecond);

	// is the given expression of the form (col CMP/IS DISTINCT/IS NOT DISTINCT FROM constant)
	// either the constant or the column can be casted
	static GP_BOOL FIdentCompareConstIgnoreCast(CExpression *pexpr,
											 COperator::EOperatorId);

	// check if a given expression is a comparison between a column and an expression
	// using only constants and one or more outer references from a given ColRefSet,
	// it optionally returns the colref of the local table
	static GP_BOOL FIdentCompareOuterRefExprIgnoreCast(
		CExpression *pexpr, CColRefSet *pcrsOuterRefs,
		CColRef **localColRef = NULL);

	// is the given expression a comparison between scalar ident and a const array
	// either the ident or constant array can be casted
	static GP_BOOL FArrayCompareIdentToConstIgnoreCast(CExpression *pexpr);

	// is the given expression of the form (col IS DISTINCT FROM constant)
	// either the constant or the column can be casted
	static GP_BOOL FIdentIDFConstIgnoreCast(CExpression *pexpr);

	// is the given expression of the form NOT (col IS DISTINCT FROM constant)
	// either the constant or the column can be casted
	static GP_BOOL FIdentINDFConstIgnoreCast(CExpression *pexpr);

	// is the given expression a comparison between a scalar ident and a constant
	static GP_BOOL FCompareIdentToConst(CExpression *pexpr);

	// is the given expression a comparison between a const and a const
	static GP_BOOL FCompareConstToConstIgnoreCast(CExpression *pexpr);

	// is the given expression of the form (col IS DISTINCT FROM const)
	static GP_BOOL FIdentIDFConst(CExpression *pexpr);

	static GP_BOOL FEqIdentsOfSameType(CExpression *pexpr);

	// checks if comparison is between two columns, or a column and a const
	static GP_BOOL FCompareColToConstOrCol(CExpression *pexprScalar);

	// is the given expression a comparison between a scalar ident under a scalar cast and a constant array
	static GP_BOOL FCompareCastIdentToConstArray(CExpression *pexpr);

	// is the given expression a comparison between a scalar ident and a constant array
	static GP_BOOL FCompareIdentToConstArray(CExpression *pexpr);

	// is the given expression an AND
	static GP_BOOL
	FAnd(CExpression *pexpr)
	{
		return CUtils::FScalarBoolOp(pexpr, CScalarBoolOp::EboolopAnd);
	}

	// is the given expression an OR
	static GP_BOOL
	FOr(CExpression *pexpr)
	{
		return CUtils::FScalarBoolOp(pexpr, CScalarBoolOp::EboolopOr);
	}

	// does the given expression have any NOT children?
	// is the given expression a NOT
	static GP_BOOL
	FNot(CExpression *pexpr)
	{
		return CUtils::FScalarBoolOp(pexpr, CScalarBoolOp::EboolopNot);
	}

	// returns true iff the given expression is a Not operator whose child is a
	// scalar identifier
	static GP_BOOL FNotIdent(CExpression *pexpr);

	// does the given expression have any NOT children?
	static GP_BOOL FHasNegatedChild(CExpression *pexpr);

	// is the given expression an inner join or NAry join
	static GP_BOOL
	FInnerOrNAryJoin(CExpression *pexpr)
	{
		return COperator::EopLogicalInnerJoin == pexpr->Pop()->Eopid() ||
			   COperator::EopLogicalNAryJoin == pexpr->Pop()->Eopid();
	}

	static GP_BOOL
	FLeftOuterJoin(CExpression *pexpr)
	{
		return COperator::EopLogicalLeftOuterJoin == pexpr->Pop()->Eopid();
	}

	// is the given expression either a union or union all operator
	static GP_BOOL
	FUnionOrUnionAll(CExpression *pexpr)
	{
		return COperator::EopLogicalUnion == pexpr->Pop()->Eopid() ||
			   COperator::EopLogicalUnionAll == pexpr->Pop()->Eopid();
	}

	// check if we can collapse the nth child of the given union / union all operator
	static GP_BOOL FCollapsibleChildUnionUnionAll(CExpression *pexprParent,
											   GP_ULONG child_index);

	// if the nth child of the given union/union all expression is also an union / union all expression,
	// then collect the later's children and set the input columns of the new n-ary union/unionall operator
	static void CollectGrandChildrenUnionUnionAll(
		CMemoryPool *mp, CExpression *pexpr, GP_ULONG child_index,
		CExpressionArray *pdrgpexprResult, CColRef2dArray *pdrgdrgpcrResult);

	// extract conjuncts from a scalar tree
	static CExpressionArray *PdrgpexprConjuncts(CMemoryPool *mp,
												CExpression *pexpr);

	// extract disjuncts from a scalar tree
	static CExpressionArray *PdrgpexprDisjuncts(CMemoryPool *mp,
												CExpression *pexpr);

	// extract equality predicates on scalar identifier from a list of scalar expressions
	static CExpressionArray *PdrgpexprPlainEqualities(
		CMemoryPool *mp, CExpressionArray *pdrgpexpr);

	// create conjunction/disjunction
	static CExpression *PexprConjDisj(CMemoryPool *mp,
									  CExpressionArray *Pdrgpexpr,
									  GP_BOOL fConjunction);

	// create conjunction/disjunction of two expressions
	static CExpression *PexprConjDisj(CMemoryPool *mp, CExpression *pexprOne,
									  CExpression *pexprTwo, GP_BOOL fConjunction);

	// create conjunction
	static CExpression *PexprConjunction(CMemoryPool *mp,
										 CExpressionArray *pdrgpexpr);

	// create conjunction of two expressions
	static CExpression *PexprConjunction(CMemoryPool *mp, CExpression *pexprOne,
										 CExpression *pexprTwo);

	// create disjunction of two expressions
	static CExpression *PexprDisjunction(CMemoryPool *mp, CExpression *pexprOne,
										 CExpression *pexprTwo);

	// expand disjuncts in the given array by converting ArrayComparison to AND/OR tree and deduplicating resulting disjuncts
	static CExpressionArray *PdrgpexprExpandDisjuncts(
		CMemoryPool *mp, CExpressionArray *pdrgpexprDisjuncts);

	// expand conjuncts in the given array by converting ArrayComparison to AND/OR tree and deduplicating resulting conjuncts
	static CExpressionArray *PdrgpexprExpandConjuncts(
		CMemoryPool *mp, CExpressionArray *pdrgpexprConjuncts);

	// check if the given expression is a boolean expression on the
	// given column, e.g. if its of the form "ScalarIdent(colref)" or
	// "Not(ScalarIdent(colref))"
	static GP_BOOL FBoolPredicateOnColumn(CExpression *pexpr, CColRef *colref);

	// check if the given expression is a scalar array cmp expression on the
	// given column
	static GP_BOOL FScArrayCmpOnColumn(CExpression *pexpr, CColRef *colref,
									GP_BOOL fConstOnly);

	// check if the given expression is a null check on the given column
	// i.e. "is null" or "is not null"
	static GP_BOOL FNullCheckOnColumn(CExpression *pexpr, CColRef *colref);

	// check if the given expression of the form "col is not null"
	static GP_BOOL FNotNullCheckOnColumn(CExpression *pexpr, CColRef *colref);

	// check if the given expression is a disjunction of scalar cmp
	// expressions on the given column
	static GP_BOOL IsDisjunctionOfRangeComparison(CMemoryPool *mp,
											   CExpression *pexpr,
											   CColRef *colref,
											   CColRefSet *pcrsAllowedRefs,
											   GP_BOOL allowNotEqualPreds);

	// check if the given comparison type is one of the range comparisons, i.e.
	// LT, GT, LEq, GEq, Eq
	static GP_BOOL FRangeComparison(CExpression *expr, CColRef *colref,
								 CColRefSet *pcrsAllowedRefs,
								 GP_BOOL allowNotEqualPreds);

	// create disjunction
	static CExpression *PexprDisjunction(CMemoryPool *mp,
										 CExpressionArray *Pdrgpexpr);

	// find a predicate that can be used for partition pruning with the given part key
	static CExpression *PexprPartPruningPredicate(
		CMemoryPool *mp, const CExpressionArray *pdrgpexpr, CColRef *pcrPartKey,
		CExpression *pexprCol, CColRefSet *pcrsAllowedRefs,
		GP_BOOL allowNotEqualPreds);

	// append the conjuncts from the given expression to the given array, removing
	// any duplicates, and return the resulting array
	static CExpressionArray *PdrgpexprAppendConjunctsDedup(
		CMemoryPool *mp, CExpressionArray *pdrgpexpr, CExpression *pexpr);

	// extract interesting conditions involving the partitioning keys
	static CExpression *PexprExtractPredicatesOnPartKeys(
		CMemoryPool *mp, CExpression *pexprScalar,
		CColRef2dArray *pdrgpdrgpcrPartKeys, CColRefSet *pcrsAllowedRefs,
		GP_BOOL fUseConstraints, const IMDRelation *pmdrel = NULL);

	// extract the constraint on the given column and return the corresponding
	// scalar expression
	static CExpression *PexprPredicateCol(CMemoryPool *mp, CConstraint *pcnstr,
										  CColRef *colref,
										  GP_BOOL fUseConstraints);

	// extract components of a comparison expression on the given key
	static void ExtractComponents(CExpression *pexprScCmp, CColRef *pcrKey,
								  CExpression **ppexprKey,
								  CExpression **ppexprOther,
								  IMDType::ECmpType *pecmpt);

	// Expression is a comparison with a simple identifer on at least one side
	static GP_BOOL FIdentCompare(CExpression *pexpr, IMDType::ECmpType pecmpt,
							  CColRef *colref);

	// check if given column is a constant
	static GP_BOOL FConstColumn(CConstraint *pcnstr, const CColRef *colref);

	// check if given column is a disjunction of constants
	static GP_BOOL FColumnDisjunctionOfConst(CConstraint *pcnstr,
										  const CColRef *colref);

	// check if given column is a disjunction of constants
	static GP_BOOL FColumnDisjunctionOfConst(CConstraintInterval *pcnstr,
										  const CColRef *colref);

	// split predicates into those that refer to an index key, and those that don't
	static void ExtractIndexPredicates(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpressionArray *pdrgpexprPredicate, const IMDIndex *pmdindex,
		CColRefArray *pdrgpcrIndex, CExpressionArray *pdrgpexprIndex,
		CExpressionArray *pdrgpexprResidual,
		CColRefSet *pcrsAcceptedOuterRefs =
			NULL,  // outer refs that are acceptable in an index predicate
		GP_BOOL allowArrayCmpForBTreeIndexes = false);

	// return the inverse of given comparison expression
	static CExpression *PexprInverseComparison(CMemoryPool *mp,
											   CExpression *pexprCmp);

	// prune unnecessary equality operations
	static CExpression *PexprPruneSuperfluosEquality(CMemoryPool *mp,
													 CExpression *pexpr);

	// remove conjuncts that are implied based on child columns equivalence classes
	static CExpression *PexprRemoveImpliedConjuncts(CMemoryPool *mp,
													CExpression *pexprScalar,
													CExpressionHandle &exprhdl);

	//	check if given correlations are valid for semi join operator;
	static GP_BOOL FValidSemiJoinCorrelations(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
		CExpressionArray *pdrgpexprCorrelations);

	// check if given expression is composed of simple column equalities that use columns from given column set
	static GP_BOOL FSimpleEqualityUsingCols(CMemoryPool *mp,
										 CExpression *pexprScalar,
										 CColRefSet *pcrs);

	// check if given expression is a valid index lookup predicate
	// return (modified) predicate suited for index lookup
	static CExpression *PexprIndexLookup(
		CMemoryPool *mp, CMDAccessor *md_accessor, CExpression *pexpPred,
		const IMDIndex *pmdindex, CColRefArray *pdrgpcrIndex,
		CColRefSet *outer_refs, GP_BOOL allowArrayCmpForBTreeIndexes);

	// split given scalar expression into two conjunctions; without and with outer references
	static void SeparateOuterRefs(CMemoryPool *mp, CExpression *pexprScalar,
								  CColRefSet *outer_refs,
								  CExpression **ppexprLocal,
								  CExpression **ppexprOuterRef);

	// is the condition a LIKE predicate
	static GP_BOOL FLikePredicate(IMDId *mdid);

	// is the condition a LIKE predicate
	static GP_BOOL FLikePredicate(CExpression *pexprPred);

	// extract the components of a LIKE predicate
	static void ExtractLikePredComponents(CExpression *pexprPred,
										  CExpression **ppexprScIdent,
										  CExpression **ppexprConst);

	// check if scalar expression is null-rejecting and uses columns from given column set
	static GP_BOOL FNullRejecting(CMemoryPool *mp, CExpression *pexprScalar,
							   CColRefSet *pcrs);

	// returns true iff the given predicate pexprPred is compatible with the given index pmdindex
	static GP_BOOL FCompatibleIndexPredicate(CExpression *pexprPred,
										  const IMDIndex *pmdindex,
										  CColRefArray *pdrgpcrIndex,
										  CMDAccessor *md_accessor);

	// returns true iff all predicates in the given array are compatible with the given index
	static GP_BOOL FCompatiblePredicates(CExpressionArray *pdrgpexprPred,
									  const IMDIndex *pmdindex,
									  CColRefArray *pdrgpcrIndex,
									  CMDAccessor *md_accessor);

	// check if the expensive CNF conversion is beneficial in finding predicate for hash join
	static GP_BOOL FConvertToCNF(CExpression *pexpr, CExpression *pexprOuter,
							  CExpression *pexprInner);

	// check if the predicate is a simple scalar cmp or a simple conjuct that can be used directly
	// for bitmap index looup without breaking it down.
	static GP_BOOL FBitmapLookupSupportedPredicateOrConjunct(
		CExpression *pexpr, CColRefSet *outer_refs);

	// Check if a given comparison operator is "very strict", meaning that it is strict
	// (NULL operands result in NULL result) and that it never produces a NULL result on
	// non-null operands. Note that this check may return false negatives (some operators
	// may have very strict behavior, yet this method returns false).
	static GP_BOOL FBuiltInComparisonIsVeryStrict(IMDId *mdid);

	// Check if the given expr only contains conjuncts of strict comparison operators
	// NB: This does NOT recurse into Boolean AND/OR operations
	static GP_BOOL ExprContainsOnlyStrictComparisons(CMemoryPool *mp,
												  CExpression *expr);

	// Check if the given exprs only contains conjuncts of strict comparison operators
	// NB: This does NOT recurse into Boolean AND/OR operations
	static GP_BOOL ExprsContainsOnlyStrictComparisons(CExpressionArray *conjuncts);

};	// class CPredicateUtils
}  // namespace gpopt


#endif	// !GPOPT_CPredicateUtils_H

// EOF
