#include "../../../include/Systems/CreationSystems/UComponentBlockParams.h"

namespace Pillar{
	UComponentBlockParamCreator::UComponentBlockParamCreator(const DComponentBlockName& componentBlockName) :
		m_ComponentBlockName(componentBlockName){}

	const DComponentBlockName& UComponentBlockParamCreator::GetComponentName() const
	{
		return m_ComponentBlockName;
	}
	void UComponentBlockParamCreator::AddFloatVariable(const std::string& name)
	{
		m_VariableName_To_EntityHandleFloatValues.emplace(std::pair(name, UEntityHandleMap<float>{}));
	}
	void UComponentBlockParamCreator::AddBoolVariable(const std::string& name)
	{
		m_VariableName_To_EntityHandleBoolValues.emplace(std::pair(name, UEntityHandleMap<bool>{}));
	}
	void UComponentBlockParamCreator::RemoveFloatVariable(const std::string& name)
	{
		m_VariableName_To_EntityHandleFloatValues.erase(name);
	}
	void UComponentBlockParamCreator::RemoveBoolVariable(const std::string& name)
	{
		m_VariableName_To_EntityHandleBoolValues.erase(name);
	}
	void UComponentBlockParamCreator::AddEntityHandle(UEntityHandle& entityHandle)
	{
		auto beginFloat = m_VariableName_To_EntityHandleFloatValues.begin();
		auto endFloat = m_VariableName_To_EntityHandleFloatValues.end();

		for (auto It = beginFloat; It != endFloat; It++)
		{
			It->second.Add(entityHandle, 0.f);
		}

		auto beginBool = m_VariableName_To_EntityHandleBoolValues.begin();
		auto endBool = m_VariableName_To_EntityHandleBoolValues.end();

		for (auto It = beginBool; It != endBool; It++)
		{
			It->second.Add(entityHandle, false);
		}
	}
	void UComponentBlockParamCreator::RemoveEntityHandle(UEntityHandle& entityHandle)
	{
		auto beginFloat = m_VariableName_To_EntityHandleFloatValues.begin();
		auto endFloat = m_VariableName_To_EntityHandleFloatValues.end();

		for (auto It = beginFloat; It != endFloat; It++)
		{
			It->second.SwapRemove(entityHandle);
		}

		auto beginBool = m_VariableName_To_EntityHandleBoolValues.begin();
		auto endBool = m_VariableName_To_EntityHandleBoolValues.end();

		for (auto It = beginBool; It != endBool; It++)
		{
			It->second.SwapRemove(entityHandle);
		}
	}
	float UComponentBlockParamCreator::GetConstFloat(const std::string& name, const UEntityHandle& entityHandle) const
	{
		if (m_VariableName_To_EntityHandleFloatValues.contains(name))
		{
			return m_VariableName_To_EntityHandleFloatValues.at(name).ConstValueAt(entityHandle);
		}
		return 0.0f;
	}
	bool UComponentBlockParamCreator::GetConstBool(const std::string& name, const UEntityHandle& entityHandle) const
	{
		if (m_VariableName_To_EntityHandleBoolValues.contains(name))
		{
			return m_VariableName_To_EntityHandleBoolValues.at(name).ConstValueAt(entityHandle);
		}
		return false;
	}
	bool UComponentBlockParamCreator::SetConstFloat(const std::string& name, float value, const UEntityHandle& entityHandle)
	{
		if (m_VariableName_To_EntityHandleFloatValues.contains(name))
		{
			m_VariableName_To_EntityHandleFloatValues.at(name).Set(entityHandle, value);

			return true;
		}

		return false;
	}
	bool UComponentBlockParamCreator::SetConstBool(const std::string& name, bool value, const UEntityHandle& entityHandle)
	{
		if (m_VariableName_To_EntityHandleBoolValues.contains(name))
		{
			m_VariableName_To_EntityHandleBoolValues.at(name).Set(entityHandle, value);

			return true;
		}

		return false;
	}

}