#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>

namespace Pillar {

	/*
	
	Maybe add some activation features afterwards

	*/

	template<typename Type>
	class TUnorderedVector {

	public:
		TUnorderedVector();
		TUnorderedVector(const TUnorderedVector& other);
		TUnorderedVector(TUnorderedVector&& other)noexcept;

		TUnorderedVector<Type>& operator=(const TUnorderedVector<Type>& other) = default;
		TUnorderedVector<Type>& operator=(TUnorderedVector<Type>&& other) = default;

		bool Create(const Type& type);
		bool Create(Type&& type);

		bool Contains(Type& type)const;
		Type& At(size_t index);

		void Remove(const Type& type);
		void Remove(size_t index);

		size_t Size()const ;

	private:
		
		std::vector<Type> m_TypeVec;
		std::unordered_map<Type, size_t> m_TypeSet_To_Index;

	};

	template<typename Type>
	inline TUnorderedVector<Type>::TUnorderedVector()
	{
	}
	template<typename Type>
	inline TUnorderedVector<Type>::TUnorderedVector(const TUnorderedVector& other) :
		m_TypeVec(other.m_TypeVec),
		m_TypeSet_To_Index(other.m_TypeSet_To_Index){}
	template<typename Type>
	inline TUnorderedVector<Type>::TUnorderedVector(TUnorderedVector&& other) noexcept
	{
		m_TypeVec.swap(other.m_TypeVec);
		m_TypeSet_To_Index.swap(other.m_TypeSet_To_Index);
	}
	template<typename Type>
	inline bool TUnorderedVector<Type>::Create(const Type& type)
	{
		if (!m_TypeSet_To_Index.contains(type))
		{
			m_TypeSet_To_Index.emplace(std::pair(type, m_TypeVec.size()));
			m_TypeVec.push_back(type);

			return true;
		}
		return false;
	}
	template<typename Type>
	inline bool TUnorderedVector<Type>::Create(Type&& type)
	{
		if (!m_TypeSet_To_Index.contains(type))
		{
			m_TypeSet_To_Index.emplace(std::pair(type, m_TypeVec.size()));
			m_TypeVec.emplace_back(type);

			return true;
		}

		return false;
	}
	template<typename Type>
	inline bool TUnorderedVector<Type>::Contains(Type& type) const
	{
		return m_TypeSet_To_Index.contains(type);
	}
	template<typename Type>
	inline Type& TUnorderedVector<Type>::At(size_t index)
	{
		return m_TypeVec.at(index);
	}
	template<typename Type>
	inline void TUnorderedVector<Type>::Remove(const Type& type)
	{
		if (m_TypeSet_To_Index.contains(type))
		{
			size_t indexToRemove = m_TypeSet_To_Index.at(type);
			std::swap(m_TypeVec.at(indexToRemove), m_TypeVec.at(m_TypeVec.size() - 1));
			m_TypeSet_To_Index.at(m_TypeVec.at(indexToRemove)) = indexToRemove;
			m_TypeVec.pop_back();
			m_TypeSet_To_Index.erase(type);
		}
	}
	template<typename Type>
	inline void TUnorderedVector<Type>::Remove(size_t index)
	{
		if (m_TypeVec.size() > index)
		{
			std::swap(m_TypeVec.at(index), m_TypeVec.at(m_TypeVec.size() - 1));
			m_TypeSet_To_Index.at(m_TypeVec.at(index)) = index;
			m_TypeSet_To_Index.erase(m_TypeVec.at(m_TypeVec.size() - 1));
			m_TypeVec.pop_back();
			
		}
	}
	template<typename Type>
	inline size_t TUnorderedVector<Type>::Size() const
	{
		return m_TypeVec.size();
	}
}