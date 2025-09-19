#pragma once

#include <vector>
#include <algorithm>

namespace Pillar {

	template<typename ComponentType>
	class TActivatableVector {

	public:

		void Reserve(size_t number);

		void Add(const ComponentType& component);
		void Add(ComponentType&& component);
		void Add(size_t number);
		void Add(size_t number, const ComponentType& componentTemplate);

		void LockCreation();
		void UnlockCreation();
		const bool IsCreationLocked() const;

		void LockActivation();
		void UnlockActivation();
		const bool IsActivationLocked() const;

		const bool Activate(size_t& index);
		const bool Deactivate(size_t& index);
		const bool IsActivated(size_t index) const;

		const size_t NumOfComponents() const;
		const size_t NumOfActivatedComponents() const;

		ComponentType& At(size_t index);
		const ComponentType& ConstAt(size_t index) const;

	private:
		bool m_CreationLocked{false};
		bool m_ActivationLocked{false};
		std::vector<ComponentType> m_Components;
		size_t m_Activated{0};
	};

	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::Reserve(size_t number)
	{
		m_Components.reserve(number);
	}
	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::Add(const ComponentType& component)
	{
		if (m_CreationLocked)
		{
			//probably some debug message
			return;
		}

		m_Components.emplace_back(component);
		++m_Activated;
	}
	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::Add(ComponentType&& component)
	{
		if (m_CreationLocked)
		{
			//probably some debug message
			return;
		}

		m_Components.emplace_back(component);
		++m_Activated;
	}
	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::Add(size_t number)
	{
		if (m_CreationLocked)
		{
			//probably some debug message
			return;
		}

		m_Components.reserve(m_Components.size() + number);

		for (size_t i = 0; i < number; i++)
		{
			m_Components.emplace_back();
			++m_Activated;
		}
	}

	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::Add(size_t number, const ComponentType& component)
	{
		if (m_CreationLocked)
		{
			//probably some debug message
			return;
		}

		m_Components.reserve(m_Components.size() + number);

		for (size_t i = 0; i < number; i++)
		{
			m_Components.emplace_back(component);
			++m_Activated;
		}
	}

	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::LockCreation()
	{
		m_CreationLocked = true;
	}

	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::UnlockCreation()
	{
		m_CreationLocked = false;
	}

	template<typename ComponentType>
	inline const bool TActivatableVector<ComponentType>::IsCreationLocked() const
	{
		return m_CreationLocked;
	}

	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::LockActivation()
	{
		m_ActivationLocked = true;
	}

	template<typename ComponentType>
	inline void TActivatableVector<ComponentType>::UnlockActivation()
	{
		m_ActivationLocked = false;
	}

	template<typename ComponentType>
	inline const bool TActivatableVector<ComponentType>::IsActivationLocked() const
	{
		return m_ActivationLocked;
	}

	template<typename ComponentType>
	inline const bool TActivatableVector<ComponentType>::Activate(size_t& index)
	{
		if (m_Activated > index)
		{
			return;
		}

		std::swap(m_Components.at(index), m_Components.at(m_Activated));
		index = m_Activated;
		++m_Activated;
	}

	template<typename ComponentType>
	inline const bool TActivatableVector<ComponentType>::Deactivate(size_t& index)
	{
		if (m_Activated <= index)
		{
			return;
		}

		std::swap(m_Components.at(index), m_Components.at(m_Activated - 1));
		index = m_Activated - 1;
		--m_Activated;
	}

	template<typename ComponentType>
	inline const bool TActivatableVector<ComponentType>::IsActivated(size_t index) const
	{
		return index < m_Activated;
	}

	template<typename ComponentType>
	inline const size_t TActivatableVector<ComponentType>::NumOfComponents() const
	{
		return m_Components.size();
	}

	template<typename ComponentType>
	inline const size_t TActivatableVector<ComponentType>::NumOfActivatedComponents() const
	{
		return m_Activated;
	}

	template<typename ComponentType>
	inline ComponentType& TActivatableVector<ComponentType>::At(size_t index)
	{
		return m_Components.at(index);
	}

	template<typename ComponentType>
	inline const ComponentType& TActivatableVector<ComponentType>::ConstAt(size_t index) const
	{
		return m_Components.at(index);
	}
	
}