#pragma once

#include <stack>

#include "../../../include/Structs/FInput.h"
#include "../../../include/Systems/TemplateSystems/TActivatableFlatMap.h"
#include "../../../include/Systems/StateSystems/UStateCondition.h"
#include "../../../include/Systems/StateSystems/UStateController.h"
#include "../../../include/Systems/EventSystems/UEventRegistry.h"
#include "../../../include/Core/URegistrySingleton.h"


namespace Pillar {

	struct FEventReactingInformation {

		EEventState CurrentGeneralStateOfEvent{ EEventState::Inactive };

		std::unordered_set<std::type_index> StateControllerTypesWithEvent;

		TActivatableFlatMap<std::string, EEventStateToTrigger> EventsToReactOnExecution;
		TActivatableFlatMap<std::string, EEventStateToTrigger> EventsToReactOnRemoval;

		std::unordered_set<std::string> EventsToParticipateInExecution;
		std::unordered_set<std::string> EventsToParticipateInRemoval;

		std::vector<FEventTriggerCondition> ConditionsToTriggerExecution;
		std::vector<FEventTriggerCondition> ConditionsToTriggerRemoval;
	};

	/*
			
	YOU STILL HAVE TO REWORK ALL OF THE STATE CONTROLLER MANAGER LOGIC AND ADD SOME ENTITY LOCAL AND ENTITY GLOBAL FUNCTIONS
			
	*/
	/*
	
		I'm aware the design of this class is really messy, for now I am prioritizing the easiest and most time efficient solution.
		Doing it properly would take a few days of intensive refactoring, which is time that I don't have at the moment
	

		DOXYGEN
	*/

	class UStateMachine final{

	public:
		explicit UStateMachine(URegistry& registry, const UEntityHandle& handle);

		const UEntityHandle& GetEntityHandle() const;
		bool IsValid()const;

		//Check if event or actions are active
		EEventState EventState(const std::string& eventOrActionName)const;

		template<typename StateControllerType>
		EEventState StateControllerEventState(const std::string& eventOrActionName)const;
	
		void AddState(const std::string& stateName);

		template<typename StateControllerType>
		bool AddStateController(const StateControllerType& stateController);
		template<typename StateControllerType>
		void RemoveStateController();

		template<typename StateControllerType>
		void TriggerTypeSpecificEventExecution(const std::string& eventName);

		///
		/// Function description
		/// @param eventName name of the event
		/// @tparam stuff
		/// 
		template<typename StateControllerType>
		void TriggerTypeSpecificEventRemoval(const std::string& eventName);

		void TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger);

		void BindEventToInput(const std::string& stateNameToBeExecutedOnInput, EEventStateToTrigger eventStateToTrigger, SDL_Scancode input, EInputState inputState);
		void UnbindEventFromInput(const std::string& stateName, EEventStateToTrigger eventStateToTrigger, SDL_Scancode input, EInputState inputState);

		//this is react on start and on finish
		bool AddReactingState(const std::string& stateNameThatIsGoingToTrigger, EEventStateToTrigger eventStateToReactTo, const std::string& eventThatIsGoingToReact, EEventStateToTrigger eventStateToTrigger);
		void RemoveReactingState(const std::string& stateNameThatIsGoingToTrigger, EEventStateToTrigger eventStateToReactTo, const std::string& eventThatIsGoingToReact, EEventStateToTrigger eventStateToTrigger);

		//this is react if x conditions are met -> has to do with state more than execution time
		bool AddConditionsToExecute(const std::string& eventOrActionToExecute, const FEventTriggerCondition& condition);
		bool AddConditionsToRemove(const std::string& eventOrActionToRemove, const FEventTriggerCondition& condition);

		void AddConditionsToExecute(const std::string& eventOrActionToExecute, const UStateCondition& condition, EEventTriggerConditionType eventTriggerConditionType);
		void AddConditionsToRemove(const std::string& eventOrActionToRemove, const UStateCondition& condition, EEventTriggerConditionType eventTriggerConditionType);

		void CheckStateTriggersOfOtherStateMachines();

	private:

		/*
		Only Gate Conditions are checked here
		Gives the effect of only Let this happen if meets certain conditions
		*/
		bool GateConditions_CanExecute_Internal(const std::string& eventOrActionName);
		bool GateConditions_CanBeRemoved_Internal(const std::string& eventOrActionName);

		/*
		Only Trigger Conditions are checked here
		Gives the effect of, when this happens, trigger this
		*/
		bool TriggerConditions_CanExecute_Internal(const std::string& eventOrActionName);
		bool TriggerConditions_CanBeRemoved_Internal(const std::string& eventOrActionName);

		//just functions for the graph loop
		void PushEventsToReactOnExecution(const std::string& eventOrActionNameExecuting);
		void PushEventsToReactOnRemoval(const std::string& eventOrActionNameRemoval);

		void Implementation_TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger);

		URegistrySingleton<UEventRegistry> m_EventRegistry;

		URegistry* m_Registry;
		UEntityHandle m_EntityHandle;

		UStateConditionsController m_ConditionsToExecute;
		UStateConditionsController m_ConditionsToRemove;

		std::unordered_map<std::string, FEventReactingInformation> m_StateName_To_ReactingInformation;
		std::unordered_map<std::type_index, std::shared_ptr<IStateController>> m_Type_To_StateControllers;

		//For Loop implementation purposes (could be local function variables, but too big to allocate and deallocate every trigger call)
		std::queue<FEventNameAndEventStateToTrigger> m_EventsToRemoveOrExecute;
	};
	

	template<typename StateControllerType>
	inline EEventState UStateMachine::StateControllerEventState(const std::string& eventOrActionName) const
	{
		return m_Type_To_StateControllers.at(typeid(StateControllerType)).get()->EventState(eventOrActionName);
	}
	template<typename StateControllerType>
	inline bool UStateMachine::AddStateController(const StateControllerType& stateController)
	{
		const IStateController* controller = (IStateController*)&stateController; //ask this in class
		std::type_index typeId{ typeid(StateControllerType) };
		if (controller->IsValid() && !m_Type_To_StateControllers.contains(typeId))
		{
			m_Type_To_StateControllers.emplace(std::pair(typeId, std::make_shared<StateControllerType>(stateController)));

			for (size_t eventIdx = 0; eventIdx < controller->GetConstData().Size(); eventIdx++)
			{
				if (!m_StateName_To_ReactingInformation.contains(controller->GetConstData().ConstKeyAt(eventIdx)))
				{
					m_StateName_To_ReactingInformation.emplace(std::pair(controller->GetConstData().ConstKeyAt(eventIdx), FEventReactingInformation{}));
				}
				m_StateName_To_ReactingInformation.at(controller->GetConstData().ConstKeyAt(eventIdx)).StateControllerTypesWithEvent.emplace(typeId);
			}

			return true;
			
		}

		return false;
	}
	template<typename StateControllerType>
	inline void UStateMachine::RemoveStateController()
	{
		std::type_index typeId{ typeid(StateControllerType) };
		if (m_Type_To_StateControllers.contains(typeId))
		{
			IStateController* stateController{ m_Type_To_StateControllers.at(typeId).get() };

			for (size_t eventIdx = 0; eventIdx < stateController->GetConstData().Size(); eventIdx++)
			{
				m_StateName_To_ReactingInformation.at(stateController->GetConstData().ConstKeyAt(eventIdx)).StateControllerTypesWithEvent.erase(typeId);
			}

			m_Type_To_StateControllers.erase(typeId);
		}
	}
	template<typename StateControllerType>
	inline void UStateMachine::TriggerTypeSpecificEventExecution(const std::string& eventName)
	{
		std::type_index typeId{typeid(StateControllerType)};

		if (m_StateName_To_ReactingInformation.contains(eventName) && m_StateName_To_ReactingInformation.at(eventName).StateControllerTypesWithEvent.contains(typeId))
		{
			m_Type_To_StateControllers.at(typeId).get()->TriggerState(eventName, EEventStateToTrigger::ConditionalExecution);
		}
	}
	template<typename StateControllerType>
	inline void UStateMachine::TriggerTypeSpecificEventRemoval(const std::string& eventName)
	{
		std::type_index typeId{ typeid(StateControllerType) };

		if (m_StateName_To_ReactingInformation.contains(eventName) && m_StateName_To_ReactingInformation.at(eventName).StateControllerTypesWithEvent.contains(typeId))
		{
			m_Type_To_StateControllers.at(typeId).get()->TriggerState(eventName, EEventStateToTrigger::ConditionalRemoval);
		}
	}
}