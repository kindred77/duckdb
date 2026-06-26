//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CWStringBase.h
//
//	@doc:
//		Abstract wide character string class
//---------------------------------------------------------------------------
#ifndef GPOS_CWStringBase_H
#define GPOS_CWStringBase_H

#include "gpos/common/clibwrapper.h"
#include "gpos/types.h"

#define GPOS_WSZ_LENGTH(x) gpos::clib::Wcslen(x)
#define GPOS_WSZ_STR_LENGTH(x) GPOS_WSZ_LIT(x), GPOS_WSZ_LENGTH(GPOS_WSZ_LIT(x))

#define WCHAR_EOS GPOS_WSZ_LIT('\0')


namespace gpos
{
class CWStringConst;
class CMemoryPool;

//---------------------------------------------------------------------------
//	@class:
//		CWStringBase
//
//	@doc:
//		Abstract wide character string class.
//		Currently, the class has two derived classes: CWStringConst and CWString.
//		CWString is used to represent constant strings that once initialized are never
//		modified. This class is not responsible for any memory management, rather
//		its users are in charge for allocating and releasing the necessary memory.
//		In contrast, CWString can be used to store strings that are modified after
//		they are created. CWString is in charge of dynamically allocating and deallocating
//		memory for storing the characters of the string.
//
//---------------------------------------------------------------------------
class CWStringBase
{
private:
	// private copy ctor
	CWStringBase(const CWStringBase &);

protected:
	// represents end-of-wide-string character
	static const WCHAR m_empty_wcstr;

	// size of the string in number of WCHAR units (not counting the terminating '\0')
	GP_ULONG m_length;

	// whether string owns its memory and should take care of deallocating it at destruction time
	GP_BOOL m_owns_memory;

	// checks whether the string is byte-wise equal to a given string literal
	virtual GP_BOOL Equals(const WCHAR *w_str_buffer) const;

public:
	// ctor
	CWStringBase(GP_ULONG length, GP_BOOL owns_memory)
		: m_length(length), m_owns_memory(owns_memory)
	{
	}

	// dtor
	virtual ~CWStringBase()
	{
	}

	// deep copy of the string
	virtual CWStringConst *Copy(CMemoryPool *mp) const;

	// accessors
	virtual GP_ULONG Length() const;

	// checks whether the string is byte-wise equal to another string
	virtual GP_BOOL Equals(const CWStringBase *str) const;

	// checks whether the string contains any characters
	virtual GP_BOOL IsEmpty() const;

	// checks whether a string is properly null-terminated
	bool IsValid() const;

	// equality operator
	GP_BOOL operator==(const CWStringBase &str) const;

	// returns the wide character buffer storing the string
	virtual const WCHAR *GetBuffer() const = 0;

	// returns the index of the first occurrence of a character, -1 if not found
	INT Find(WCHAR wc) const;

	// checks if a character is escaped
	GP_BOOL HasEscapedCharAt(GP_ULONG offset) const;

	// count how many times the character appears in string
	GP_ULONG CountOccurrencesOf(const WCHAR wc) const;
};

}  // namespace gpos

#endif	// !GPOS_CWStringBase_H

// EOF
