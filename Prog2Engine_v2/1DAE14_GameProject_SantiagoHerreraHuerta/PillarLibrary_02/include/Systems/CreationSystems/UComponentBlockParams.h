#pragma once

#include <unordered_map>

#include "../../../include/Core/UEntityCreation.h"

namespace Pillar {

	class UComponentBlockParamCreator {

	public:

		UComponentBlockParamCreator(const DComponentBlockName& componentBlockName);

		const DComponentBlockName& GetComponentName()const;

		void AddFloatVariable(const std::string& name);
		void AddBoolVariable(const std::string& name);

		void RemoveFloatVariable(const std::string& name);
		void RemoveBoolVariable(const std::string& name);

		void AddEntityHandle(UEntityHandle& entityHandle);
		void RemoveEntityHandle(UEntityHandle& entityHandle);

		float GetConstFloat(const std::string& name, const UEntityHandle& entityHandle)const;
		bool  GetConstBool(const std::string& name, const UEntityHandle& entityHandle)const;

		bool SetConstFloat(const std::string& name, float value, const UEntityHandle& entityHandle); //doesn't set if doesn't have entity handle
		bool SetConstBool(const std::string& name, bool value, const UEntityHandle& entityHandle); //doesn't set if doesn't have entity handle


	private:

		const DComponentBlockName m_ComponentBlockName;
		std::unordered_map<std::string, UEntityHandleMap<float>> m_VariableName_To_EntityHandleFloatValues;
		std::unordered_map<std::string, UEntityHandleMap<bool>> m_VariableName_To_EntityHandleBoolValues;

	};

}