
#include "../../include/Core/UInternalFilter.h"


namespace Pillar {
	UInternalFilter::~UInternalFilter()
	{
		DeleteExcludingFilter();
		DeleteIncludingFilter();
	}
	void UInternalFilter::SetIncludingFilter(const DArchetypeKey& key)
	{
		m_IncludingFilter = key;
	}

	void UInternalFilter::SetExcludingFilter(const DArchetypeKey& key)
	{
		m_ExcludingFilter = key;
	}

	const DArchetypeKey& UInternalFilter::GetConstIncludingFilter()const
	{
		return m_IncludingFilter;
	}

	const DArchetypeKey& UInternalFilter::GetConstExcludingFilter()const
	{
		return m_ExcludingFilter;
	}

	void UInternalFilter::DeleteIncludingFilter()
	{
		m_IncludingFilter.clear();
	}

	void UInternalFilter::DeleteExcludingFilter()
	{
		m_ExcludingFilter.clear();
	}

}
