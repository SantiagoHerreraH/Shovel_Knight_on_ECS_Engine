#pragma once

#include "../../include/Core/FEntity.h"

namespace Pillar {

	/*
	Later on you can create a function that takes in a library and directly converts the Archetypes to ArchetypeKeys

	*/

	class UComponentFilter {

	public:
		UComponentFilter();
		UComponentFilter(const UComponentFilter& other);
		UComponentFilter(UComponentFilter&& other)noexcept;
		~UComponentFilter();

		//check upon setting the filters 
		template<typename ComponentType, typename ... ListOfComponents>
		void SetIncludingFilter(const ComponentType& component, const ListOfComponents&... components);
		template<typename ComponentType, typename ... ListOfComponents>
		void SetExcludingFilter(const ComponentType& component, const ListOfComponents&... components);		

		void DeleteIncludingFilter();
		void DeleteExcludingFilter();

		const DArchetype& GetConstIncludingFilter()const;
		const DArchetype& GetConstExcludingFilter()const;

	private:
		void SetIncludingFilter();
		void SetExcludingFilter();

		DArchetype m_IncludingFilter; //maybe shoudl include collision check to see if including and excluding filters have the same types
		DArchetype m_ExcludingFilter;

		DArchetype m_FilterForOperations;

	};

	template<typename ComponentType, typename ...ListOfComponents>
	inline void UComponentFilter::SetIncludingFilter(const ComponentType& component, const ListOfComponents & ...components)
	{
		m_FilterForOperations.emplace(typeid(component));

		SetIncludingFilter(components...);
	}

	template<typename ComponentType, typename ...ListOfComponents>
	inline void UComponentFilter::SetExcludingFilter(const ComponentType& component, const ListOfComponents & ...components)
	{
		m_FilterForOperations.emplace(typeid(component));

		SetExcludingFilter(components...);
	}

}