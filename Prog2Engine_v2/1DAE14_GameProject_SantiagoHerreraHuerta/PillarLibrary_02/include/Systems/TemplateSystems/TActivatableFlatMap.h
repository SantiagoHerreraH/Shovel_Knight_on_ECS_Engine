#pragma once

#include <unordered_map>
#include <vector>

namespace Pillar {

	template<typename KeyType, typename ValueType>
	class TActivatableFlatMap {

	public:

		void Reserve(size_t number);

		bool Erase(const KeyType& key);
		bool Erase(size_t index);

		bool Push(const KeyType& key, const ValueType& value, bool activateOnCreation = true); //default creation is deactivated
		bool Emplace(KeyType&& key, ValueType&& value, bool activateOnCreation = true);

		void LockCreation();
		void UnlockCreation();
		bool IsCreationLocked() const;

		void LockActivation();
		void UnlockActivation();
		bool IsActivationLocked() const;

		bool Contains(const KeyType& key)const;

		bool Activate(const KeyType& key);
		bool Deactivate(const KeyType& key);
		bool IsActivated(const KeyType& key) const;

		void DeactivateAll();
		void ActivateAll();

		bool ActivateAt(size_t& index);
		bool DeactivateAt(size_t& index);
		bool IsActivatedAt(size_t index) const;

		size_t Size() const;
		size_t ActivatedSize() const;

		const KeyType& ConstKeyAt(size_t index)const;

		ValueType& ValueAt(const KeyType& key);
		const ValueType& ConstValueAt(const KeyType& key) const;

		ValueType& At(size_t index);
		const ValueType& ConstAt(size_t index) const;

	private:
		bool m_CreationLocked{ false };
		bool m_ActivationLocked{ false };
		std::unordered_map<KeyType, size_t> m_Keys_To_Indexes;
		std::vector<KeyType> m_Keys; //_Index_To_Keys
		std::vector<ValueType> m_Values;
		size_t m_NumberOfActivated{ 0 };
	};

#pragma region Reservation and Creation

	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::Reserve(size_t number)
	{
		m_Keys_To_Indexes.reserve(number);
		m_Keys.reserve(number);
		m_Values.reserve(number);
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Erase(const KeyType& key)
	{
		if (m_Keys_To_Indexes.contains(key))
		{
			return Erase(m_Keys_To_Indexes.at(key));
		}
		
		return false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Erase(size_t index)
	{
		if (!m_ActivationLocked && !m_CreationLocked && !m_Values.empty())
		{
			DeactivateAt(index);

			size_t lastIndex{ m_Values.size() - 1};
			KeyType& keyToRemove{ m_Keys.at(index) };
			KeyType& lastKey{ m_Keys.at(lastIndex) };

			if (lastIndex != index)
			{
				

				std::swap(m_Keys_To_Indexes.at(keyToRemove), m_Keys_To_Indexes.at(lastKey));
				std::swap(m_Values.at(index), m_Values.at(lastIndex));
				std::swap(m_Keys.at(index), m_Keys.at(lastIndex));
			}

			m_Keys_To_Indexes.erase(keyToRemove);
			m_Keys.pop_back();
			m_Values.pop_back();

			return true;
		}
		return false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Push(const KeyType& key, const ValueType& value, bool activateOnCreation)
	{
		if (!m_CreationLocked && !m_Keys_To_Indexes.contains(key))
		{
			if ((m_ActivationLocked && !activateOnCreation) || !m_ActivationLocked)
			{
				m_Keys_To_Indexes.emplace(std::pair(key, m_Values.size()));
				m_Values.push_back(value);
				m_Keys.push_back(key);

				if (activateOnCreation)
				{
					size_t lastIdx{ m_Values.size() - 1 };
					ActivateAt(lastIdx);
				}

				return true;
			}
		}

		return false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Emplace(KeyType&& key, ValueType&& value, bool activateOnCreation)
	{
		if (!m_CreationLocked && !m_ActivationLocked && !m_Keys_To_Indexes.contains(key))
		{
			if ((m_ActivationLocked && !activateOnCreation) || !m_ActivationLocked) 
			{
				m_Keys_To_Indexes.emplace(std::pair(key, m_Values.size()));
				m_Values.emplace_back(value);
				m_Keys.emplace_back(key);

				if (activateOnCreation)
				{
					ActivateAt(m_Values.size() - 1);
				}

				return true;
			}
		}

		return false;
	}

#pragma endregion

#pragma region Lock and Unlock Creation and Activation

	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::LockCreation()
	{
		m_CreationLocked = true;
	}
	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::UnlockCreation()
	{
		m_CreationLocked = false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::IsCreationLocked() const
	{
		return m_CreationLocked;
	}

	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::LockActivation()
	{
		m_ActivationLocked = true;
	}
	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::UnlockActivation()
	{
		m_ActivationLocked = false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::IsActivationLocked() const
	{
		return m_ActivationLocked;
	}

#pragma endregion

#pragma region Activation and Deactivation

	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Activate(const KeyType& key)
	{
		if (Contains(key))
		{
			size_t indexToActivate{ m_Keys_To_Indexes.at(key) };

			return ActivateAt(indexToActivate);
		}
		
		return false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Deactivate(const KeyType& key)
	{
		if (Contains(key))
		{
			size_t indexToDeactivate{ m_Keys_To_Indexes.at(key) };

			return DeactivateAt(indexToDeactivate);
		}

		return false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::IsActivated(const KeyType& key) const
	{
		if (Contains(key))
		{
			return m_Keys_To_Indexes.at(key) < m_NumberOfActivated;
		}
		
		return false;
	}
	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::DeactivateAll()
	{
		m_NumberOfActivated = 0;
	}
	template<typename KeyType, typename ValueType>
	inline void TActivatableFlatMap<KeyType, ValueType>::ActivateAll()
	{
		m_NumberOfActivated = m_Values.size();
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::ActivateAt(size_t& indexToActivate)
	{
		if (m_NumberOfActivated == m_Values.size())
		{
			return true;
		}
		if (indexToActivate < m_Values.size())
		{
			size_t firstDeactivatedIndex{ m_NumberOfActivated };

			if (indexToActivate > firstDeactivatedIndex)
			{
				KeyType& keyToActivate{ m_Keys.at(indexToActivate) };
				KeyType& keyToSwitchPlaceWithActivated{ m_Keys.at(firstDeactivatedIndex) };

				std::swap(m_Keys_To_Indexes.at(keyToActivate), m_Keys_To_Indexes.at(keyToSwitchPlaceWithActivated));
				std::swap(m_Keys.at(indexToActivate), m_Keys.at(firstDeactivatedIndex));
				std::swap(m_Values.at(indexToActivate), m_Values.at(firstDeactivatedIndex));

				indexToActivate = firstDeactivatedIndex;
				++m_NumberOfActivated;
			}
			else if (indexToActivate == firstDeactivatedIndex)
			{
				++m_NumberOfActivated;
			}

			return true;
		}

		return false;
		
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::DeactivateAt(size_t& indexToDeactivate)
	{
		if (m_NumberOfActivated == 0)
		{
			return true;
		}
		if (indexToDeactivate < m_Values.size())
		{
			size_t lastActivatedIndex{ m_NumberOfActivated - 1 };

			if (indexToDeactivate < lastActivatedIndex)
			{
				KeyType& keyToDeactivate{ m_Keys.at(indexToDeactivate) };
				KeyType& keyToSwitchPlaceWithDeactivated{ m_Keys.at(lastActivatedIndex) };

				std::swap(m_Keys_To_Indexes.at(keyToDeactivate), m_Keys_To_Indexes.at(keyToSwitchPlaceWithDeactivated));
				std::swap(m_Keys.at(indexToDeactivate), m_Keys.at(lastActivatedIndex));
				std::swap(m_Values.at(indexToDeactivate), m_Values.at(lastActivatedIndex));

				indexToDeactivate = lastActivatedIndex;
				--m_NumberOfActivated;
			}
			else if (indexToDeactivate == lastActivatedIndex)
			{
				--m_NumberOfActivated;
			}

			return true;
		}

		return false;
	}
	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::IsActivatedAt(size_t index) const
	{
		return index < m_NumberOfActivated;
	}

#pragma endregion

#pragma region Check Data

	template<typename KeyType, typename ValueType>
	inline bool TActivatableFlatMap<KeyType, ValueType>::Contains(const KeyType& key)const
	{
		return m_Keys_To_Indexes.contains(key);
	}

	template<typename KeyType, typename ValueType>
	inline size_t TActivatableFlatMap<KeyType, ValueType>::Size() const
	{
		return m_Values.size();
	}
	template<typename KeyType, typename ValueType>
	inline size_t TActivatableFlatMap<KeyType, ValueType>::ActivatedSize() const
	{
		return m_NumberOfActivated;
	}

	

#pragma endregion

#pragma region Get Key and Value

	template<typename KeyType, typename ValueType>
	inline const KeyType& TActivatableFlatMap<KeyType, ValueType>::ConstKeyAt(size_t index) const
	{
		return m_Keys.at(index);
	}

	template<typename KeyType, typename ValueType>
	inline ValueType& TActivatableFlatMap<KeyType, ValueType>::ValueAt(const KeyType& key)
	{
		size_t index = m_Keys_To_Indexes.at(key);
		ValueType& valueRef = m_Values.at(index);
		return valueRef;
	}
	template<typename KeyType, typename ValueType>
	inline const ValueType& TActivatableFlatMap<KeyType, ValueType>::ConstValueAt(const KeyType& key) const
	{
		size_t index = m_Keys_To_Indexes.at(key);
		const ValueType& valueRef = m_Values.at(index);
		return valueRef;
	}
	template<typename KeyType, typename ValueType>
	inline ValueType& TActivatableFlatMap<KeyType, ValueType>::At(size_t index)
	{
		return m_Values.at(index);
	}
	template<typename KeyType, typename ValueType>
	inline const ValueType& TActivatableFlatMap<KeyType, ValueType>::ConstAt(size_t index) const
	{
		return m_Values.at(index);
	}

#pragma endregion
}