
#include "../../include/Core/UComponentFilter.h"

namespace Pillar {

#pragma region ConstructorAndDestructor
	Pillar::UComponentFilter::UComponentFilter()
	{
	}

	UComponentFilter::UComponentFilter(const UComponentFilter& other)
	{
		m_IncludingFilter = other.m_IncludingFilter;
		m_ExcludingFilter = other.m_ExcludingFilter;
	}

	UComponentFilter::UComponentFilter(UComponentFilter&& other)noexcept
	{
		m_IncludingFilter.swap(other.m_IncludingFilter);
		m_ExcludingFilter.swap(other.m_ExcludingFilter);

		other.m_IncludingFilter.clear();
		other.m_ExcludingFilter.clear();
	}

	Pillar::UComponentFilter::~UComponentFilter()
	{
		DeleteIncludingFilter();
		DeleteExcludingFilter();
	}
#pragma endregion

#pragma region Deletion

	void Pillar::UComponentFilter::DeleteIncludingFilter()
	{
		m_IncludingFilter.clear();
	}

	void Pillar::UComponentFilter::DeleteExcludingFilter()
	{
		m_ExcludingFilter.clear();
	}

#pragma endregion

#pragma region GetFilter

	const DArchetype& Pillar::UComponentFilter::GetConstIncludingFilter()const 
	{
		return m_IncludingFilter;
	}

	const DArchetype& Pillar::UComponentFilter::GetConstExcludingFilter()const 
	{
		return m_ExcludingFilter;
	}

#pragma endregion

#pragma region SetFilter
	void UComponentFilter::SetIncludingFilter()
	{
		m_IncludingFilter.clear();
		m_IncludingFilter = m_FilterForOperations;
		m_FilterForOperations.clear();
	}
	void UComponentFilter::SetExcludingFilter()
	{
		m_ExcludingFilter.clear();
		m_ExcludingFilter = m_FilterForOperations;
		m_FilterForOperations.clear();
	}
#pragma endregion
}