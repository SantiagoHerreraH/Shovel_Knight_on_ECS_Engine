#pragma once

#include "../../../include/Systems/StateSystems/UStateController.h"
#include "../../../include/Systems/StateSystems/UStateControllerGraph.h"

namespace Pillar {

	/*
	
		DEPRECATED
	
	*/

	class IStateControllerManager {
	public:
		virtual void TriggerEventExecution(const std::string& eventName, const UEntityHandle& handle) = 0;
		virtual void TriggerEventRemoval(const std::string& eventName, const UEntityHandle& handle) = 0;

		virtual void TriggerEventExecution(const std::string& eventName) = 0;
		virtual void TriggerEventRemoval(const std::string& eventName) = 0;

		virtual bool IsValid()const = 0;
		virtual bool IsValid(const UEntityHandle& entityHandle)const = 0;

		virtual bool AreAllActive(const std::string& eventOrActionName)const = 0;
		virtual bool IsActive(const UEntityHandle& entityHandle, const std::string& eventOrActionName)const = 0;
	};
	
	template<typename Type>
	class UStateControllerManager : public IStateControllerManager {

	public:

		void TriggerEventExecution(const std::string& eventName, const UEntityHandle& handle) override;
		void TriggerEventRemoval(const std::string& eventName, const UEntityHandle& handle) override;

		void TriggerEventExecution(const std::string& eventName) override;
		void TriggerEventRemoval(const std::string& eventName) override;

		bool IsValid()const override;
		bool IsValid(const UEntityHandle& entityHandle)const override;
		bool AreAllActive(const std::string& eventOrActionName)const override;
		bool IsActive(const UEntityHandle& entityHandle, const std::string& eventOrActionName)const override;
		

		size_t Size()const;

		Type& At(size_t index);
		Type& At(const UEntityHandle& handle);
		const Type& ConstAt(size_t index)const;
		const Type& ConstAt(const UEntityHandle& handle)const;

		bool AddStateController(const Type& stateController);
		void RemoveStateController(size_t index);//will change last index
		void RemoveStateController(const UEntityHandle& handle);//will change last index

	private:
		IStateController* StateControllerAt(const UEntityHandle& handle);
		IStateController* StateControllerAt(size_t index);
		const IStateController* StateControllerAt(const UEntityHandle& handle)const;
		const IStateController* StateControllerAt(size_t index)const;
		UEntityHandleMap<Type> m_Entities_To_StateControllers;
	};
	template<typename Type>
	inline void UStateControllerManager<Type>::TriggerEventExecution(const std::string& eventName, const UEntityHandle& handle)
	{
		if (m_Entities_To_StateControllers.Contains(handle))
		{
			StateControllerAt(handle)->TriggerStateExecution(eventName);
		}
	}
	template<typename Type>
	inline void UStateControllerManager<Type>::TriggerEventRemoval(const std::string& eventName, const UEntityHandle& handle)
	{
		if (m_Entities_To_StateControllers.Contains(handle))
		{
			StateControllerAt(handle)->TriggerStateRemoval(eventName);
		}
	}
	template<typename Type>
	inline void UStateControllerManager<Type>::TriggerEventExecution(const std::string& eventName)
	{
		for (size_t stateControllerIdx = 0; stateControllerIdx < m_Entities_To_StateControllers.NumOfEntities(); stateControllerIdx++)
		{
			StateControllerAt(stateControllerIdx)->TriggerStateExecution(eventName);
		}
	}
	template<typename Type>
	inline void UStateControllerManager<Type>::TriggerEventRemoval(const std::string& eventName)
	{
		for (size_t stateControllerIdx = 0; stateControllerIdx < m_Entities_To_StateControllers.NumOfEntities(); stateControllerIdx++)
		{
			StateControllerAt(stateControllerIdx)->TriggerStateRemoval(eventName);
		}
	}
	template<typename Type>
	inline bool UStateControllerManager<Type>::IsValid() const
	{
		for (size_t stateControllerIdx = 0; stateControllerIdx < m_Entities_To_StateControllers.NumOfEntities(); stateControllerIdx++)
		{
			if (!StateControllerAt(stateControllerIdx)->IsValid())
			{
				return false;
			}
		}

		return true;
	}
	template<typename Type>
	inline bool UStateControllerManager<Type>::IsValid(const UEntityHandle& entityHandle) const
	{
		return StateControllerAt(entityHandle)->IsValid();
	}
	template<typename Type>
	inline bool UStateControllerManager<Type>::AreAllActive(const std::string& eventOrActionName) const
	{
		for (size_t stateControllerIdx = 0; stateControllerIdx < m_Entities_To_StateControllers.NumOfEntities(); stateControllerIdx++)
		{
			if (!StateControllerAt(stateControllerIdx)->IsActive(eventOrActionName))
			{
				return false;
			}
		}

		return true;
	}
	template<typename Type>
	inline bool UStateControllerManager<Type>::IsActive(const UEntityHandle& entityHandle, const std::string& eventOrActionName) const
	{
		return StateControllerAt(entityHandle)->IsActive(eventOrActionName);
	}
	template<typename Type>
	inline size_t UStateControllerManager<Type>::Size() const
	{
		return m_Entities_To_StateControllers.NumOfEntities();
	}
	template<typename Type>
	inline Type& UStateControllerManager<Type>::At(size_t index)
	{
		return m_Entities_To_StateControllers.ValueAt(index);
	}
	template<typename Type>
	inline Type& UStateControllerManager<Type>::At(const UEntityHandle& handle)
	{
		return m_Entities_To_StateControllers.ValueAt(handle);
	}
	template<typename Type>
	inline const Type& UStateControllerManager<Type>::ConstAt(size_t index) const
	{
		return m_Entities_To_StateControllers.ConstValueAt(index);
	}
	template<typename Type>
	inline const Type& UStateControllerManager<Type>::ConstAt(const UEntityHandle& handle) const
	{
		return m_Entities_To_StateControllers.ConstValueAt(handle);
	}
	template<typename Type>
	inline bool UStateControllerManager<Type>::AddStateController(const Type& stateController)
	{
		IStateController* controller{ (IStateController*)&stateController};

		if (controller->IsValid() && !m_Entities_To_StateControllers.Contains(controller->GetEntityHandle()))
		{
			UEntityHandle handle{ controller->GetEntityHandle() };
			m_Entities_To_StateControllers.Add(handle, stateController);

			return true;
		}

		return false;
	}
	template<typename Type>
	inline void UStateControllerManager<Type>::RemoveStateController(size_t index)
	{
		m_Entities_To_StateControllers.SwapRemove(index);
	}
	template<typename Type>
	inline void UStateControllerManager<Type>::RemoveStateController(const UEntityHandle& handle)
	{
		m_Entities_To_StateControllers.SwapRemove(handle);
	}
	template<typename Type>
	inline IStateController* UStateControllerManager<Type>::StateControllerAt(const UEntityHandle& handle)
	{
		return (IStateController*)&m_Entities_To_StateControllers.ConstValueAt(handle);
	}
	template<typename Type>
	inline IStateController* UStateControllerManager<Type>::StateControllerAt(size_t index)
	{
		return (IStateController*)&m_Entities_To_StateControllers.ConstValueAt(index);
	}
	template<typename Type>
	inline const IStateController* UStateControllerManager<Type>::StateControllerAt(const UEntityHandle& handle) const
	{
		return (IStateController*)&m_Entities_To_StateControllers.ConstValueAt(handle);
	}
	template<typename Type>
	inline const IStateController* UStateControllerManager<Type>::StateControllerAt(size_t index) const
	{
		return (IStateController*)&m_Entities_To_StateControllers.ConstValueAt(index);
	}
}