#pragma once

#include "../../include/Core/FEntity.h"

namespace Pillar {

	class UInternalFilter {


	public:
		UInternalFilter() {}
		UInternalFilter(const UInternalFilter& other) = default; //maybe incorporate move constructor
		~UInternalFilter();

		UInternalFilter& operator=(const UInternalFilter& other) = default;
		UInternalFilter& operator=(UInternalFilter&& other) = default;
		
		void SetIncludingFilter(const DArchetypeKey& key);
		void SetExcludingFilter(const DArchetypeKey& key);

		const DArchetypeKey& GetConstIncludingFilter()const ;
		const DArchetypeKey& GetConstExcludingFilter()const ;

		void DeleteIncludingFilter();
		void DeleteExcludingFilter();

	private:
		DArchetypeKey m_IncludingFilter;
		DArchetypeKey m_ExcludingFilter;

	};

	
}