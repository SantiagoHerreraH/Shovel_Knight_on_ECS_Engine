
#include <queue>

#include "../../../include/Systems/SceneSystems/USceneGraph.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/SMatrix.h"


namespace Pillar {

	UApplySceneObjectTransformHierarchy::UApplySceneObjectTransformHierarchy(URegistry& registry) :
		m_Registry(registry),
		m_SceneObjectRootNodes(registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FWorldTransform2D{}, FMatrix4x4f{});

		registry.GetComponents(m_Matrices, filter);
		registry.GetComponents(m_WorldTransforms, filter);
	}

	void UApplySceneObjectTransformHierarchy::Update()
	{
		m_Registry.Lock<FMatrix4x4f>();
		m_Registry.Lock<FSceneNode>();
		m_Registry.Lock<FWorldTransform2D>();

		FSceneNode* currentParentSceneObject{nullptr};
		FSceneNode* currentChildSceneObject{nullptr};

		FWorldTransform2D* currentParentWorldTransform{nullptr};
		FMatrix4x4f* currentParentMatrix{ nullptr };
		FMatrix4x4f* currentChildMatrix{ nullptr };

		const UEntityHandle* currentParentEntity{ nullptr };

		for (size_t rootIdx = 0; rootIdx < m_SceneObjectRootNodes.Get().RootEntities.NumOfEntities(); rootIdx++)
		{
			currentParentEntity = &m_SceneObjectRootNodes.Get().RootEntities.ConstEntityAt(rootIdx);
			m_CurrentEntityQueue.push_back(*currentParentEntity);

			while (!m_CurrentEntityQueue.empty())
			{
				currentParentSceneObject = m_Registry.GetComponent<FSceneNode>(m_CurrentEntityQueue.front().GetConstEntity());
				currentParentMatrix = m_Registry.GetComponent<FMatrix4x4f>(m_CurrentEntityQueue.front().GetConstEntity());
				currentParentWorldTransform = m_Registry.GetComponent<FWorldTransform2D>(m_CurrentEntityQueue.front().GetConstEntity());
				
				if (!currentParentSceneObject ||
					!currentParentMatrix || 
					!currentParentWorldTransform ||
					!m_Registry.IsActivated(m_CurrentEntityQueue.front().GetConstEntity()))
				{
					m_CurrentEntityQueue.pop_front();
					continue;
				}
				
				SMatrix::ApplyTransform(*currentParentMatrix, currentParentWorldTransform->Transform);

				for (size_t childrenIdx = 0; childrenIdx < currentParentSceneObject->Children.NumOfEntities(); childrenIdx++)
				{
					currentChildSceneObject = m_Registry.GetComponent<FSceneNode>(currentParentSceneObject->Children.EntityAt(childrenIdx).GetConstEntity());
					currentChildMatrix = m_Registry.GetComponent<FMatrix4x4f>(currentParentSceneObject->Children.EntityAt(childrenIdx).GetConstEntity());

					if (!currentChildSceneObject->Traversed)
					{
						SMatrix::Multiply(*currentParentMatrix, *currentChildMatrix);
						

						currentChildSceneObject->Traversed = true;

						m_CurrentEntityQueue.push_back(currentParentSceneObject->Children.EntityAt(childrenIdx));
					}
				}

				m_CurrentEntityQueue.pop_front();
			}
		}

		/*
		
			the last transformed is the first inverse transform
			World Transform
		
		*/

		m_Registry.Unlock<FSceneNode>();
		m_Registry.Unlock<FMatrix4x4f>();
		m_Registry.Unlock<FWorldTransform2D>();
	}

	void UApplySceneObjectTransformHierarchy::Update_New()
	{
		for (size_t archIdx = 0; archIdx < m_WorldTransforms.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_WorldTransforms.NumOfComponents(archIdx); compIdx++)
			{
				SMatrix::ApplyTransform(m_Matrices.At(archIdx, compIdx), m_WorldTransforms.At(archIdx, compIdx).Transform);
			}
		}
	}

	
	
	USceneGraph::USceneGraph(URegistry& registry) :
		m_RegisterRootNodes(registry),
		m_ApplySceneObjectTransformHierarchy(registry),
		m_ApplyTransformToMatrix(registry),
		m_ApplyMatrixToShape(registry),
		m_ResetMatrices(registry),
		m_ApplyMatricesToWorldTransforms(registry),
		m_CalculateShapeCenters(registry),
		m_ResetSceneNodesTraversal(registry),
		m_ApplyCameraTransformToWorld(registry),
		m_ApplyMatricesToScreenTransform(registry)
	
	{}

	void USceneGraph::Initialize()
	{
		m_RegisterRootNodes.InitializeSceneGraph();
	}

	void USceneGraph::Update()
	{
		m_ApplyTransformToMatrix.Update();
		m_ApplySceneObjectTransformHierarchy.Update_New();

		m_ApplyMatricesToWorldTransforms.Update();
		m_ApplyCameraTransformToWorld.Update(); 
		m_ApplyMatricesToScreenTransform.Update();

		m_ApplyMatrixToShape.Update();
		m_CalculateShapeCenters.Update();				// so shape centers are in screen coordinates

		m_ResetMatrices.Reset();
		m_ResetSceneNodesTraversal.Reset();
	}

	UInitializeSceneObjects::UInitializeSceneObjects(URegistry& registry) :
		m_Registry(registry),
		m_SceneRegistry(registry),
		m_SceneManagementSystem(registry)
	{
		registry.GetComponents(m_SceneNodes);
		registry.GetComponents(m_SceneObjects);
	}

	void UInitializeSceneObjects::InitializeSceneGraph()
	{
		RegisterRootNodes();
		RegisterHierarchies();
		RegisterSceneObjects();
	}

	void UInitializeSceneObjects::InitializeSceneObjects()
	{
		m_SceneManagementSystem.Initialize(0);
	}

	void UInitializeSceneObjects::RegisterRootNodes()
	{
		UEntityHandle currentHandle{};
		size_t currentSceneIndex{};

		for (size_t archIdx = 0; archIdx < m_SceneNodes.NumOfArchetypes(); archIdx++)
		{

			for (size_t compIdx = 0; compIdx < m_SceneNodes.NumOfComponents(archIdx); compIdx++)
			{
				m_Registry.GetEntity<FSceneNode>(currentHandle, m_SceneNodes.GetArchetype(archIdx), compIdx);

				if (!m_SceneNodes.At(archIdx, compIdx).Parent.IsValid())
				{
					m_SceneRegistry.Get().RootEntities.Add(currentHandle);
				}
				else
				{
					m_SceneRegistry.Get().RootEntities.SwapRemove(currentHandle);

					m_Registry.GetComponent<FSceneNode>(
						m_SceneNodes.At(archIdx, compIdx).Parent.GetConstEntity())->Children.Add(currentHandle);
					
				}
			}
		}
	}

	void UInitializeSceneObjects::RegisterSceneObjects()
	{
		UEntityHandle currentHandle{};
		size_t currentSceneIndex{};

		m_SceneRegistry.Get().SceneEntities.reserve(m_EstimatedNumberOfScenes);

		for (size_t archIdx = 0; archIdx < m_SceneObjects.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_SceneObjects.NumOfComponents(archIdx); compIdx++)
			{
				m_Registry.GetEntity<FSceneObject>(currentHandle, m_SceneObjects.GetArchetype(archIdx), compIdx);

				currentSceneIndex = m_SceneObjects.At(archIdx, compIdx).SceneIndex;

				if (currentSceneIndex >= m_SceneRegistry.Get().SceneEntities.size())
				{
					m_SceneRegistry.Get().SceneEntities.resize(currentSceneIndex + 1);
				}

				if (m_Registry.IsActivated(currentHandle.GetConstEntity()))
				{
					m_SceneRegistry.Get().SceneEntities.at(currentSceneIndex).Add(currentHandle, EActivationStateForOptimization::ActiveBeforeDeactivating);
				}
				else
				{
					m_SceneRegistry.Get().SceneEntities.at(currentSceneIndex).Add(currentHandle, EActivationStateForOptimization::UnactiveBeforeDeactivating);
				}
			}
		}
	}

	void UInitializeSceneObjects::RegisterHierarchies()
	{
		m_SceneRegistry.Get().SceneHierarchies.resize(m_SceneRegistry.Get().RootEntities.NumOfEntities());
		m_SceneRegistry.Get().Entity_To_EntitySceneHierarchyIndex.Reserve(m_SceneNodes.TotalNumOfComponents());

		std::queue<FSceneNode> sceneObjectsToBeRegistered{};
		FSceneNode* currentSceneObject{ nullptr };

		for (size_t rootEnttIdx = 0; rootEnttIdx < m_SceneRegistry.Get().RootEntities.NumOfEntities(); rootEnttIdx++)
		{
			currentSceneObject = m_Registry.GetComponent<FSceneNode>(m_SceneRegistry.Get().RootEntities.EntityAt(rootEnttIdx).GetConstEntity());

			m_SceneRegistry.Get().SceneHierarchies.at(rootEnttIdx).Add(m_SceneRegistry.Get().RootEntities.EntityAt(rootEnttIdx));
			m_SceneRegistry.Get().Entity_To_EntitySceneHierarchyIndex.Add(m_SceneRegistry.Get().RootEntities.EntityAt(rootEnttIdx), rootEnttIdx);

			sceneObjectsToBeRegistered.push(*currentSceneObject);

			while (!sceneObjectsToBeRegistered.empty())
			{
				for (size_t childIdx = 0; childIdx < sceneObjectsToBeRegistered.front().Children.NumOfEntities(); childIdx++)
				{
					m_SceneRegistry.Get().SceneHierarchies.at(rootEnttIdx).Add(sceneObjectsToBeRegistered.front().Children.EntityAt(childIdx));
					m_SceneRegistry.Get().Entity_To_EntitySceneHierarchyIndex.Add(sceneObjectsToBeRegistered.front().Children.EntityAt(childIdx), rootEnttIdx);
					currentSceneObject = m_Registry.GetComponent<FSceneNode>(sceneObjectsToBeRegistered.front().Children.EntityAt(childIdx).GetConstEntity());
					sceneObjectsToBeRegistered.push(*currentSceneObject);
				}

				sceneObjectsToBeRegistered.pop();
				
			}
		}
	}

	UResetSceneNodesTraversal::UResetSceneNodesTraversal(URegistry& registry)
	{
		registry.GetComponents(m_SceneNodes);
	}

	void UResetSceneNodesTraversal::Reset()
	{
		for (size_t archIdx = 0; archIdx < m_SceneNodes.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_SceneNodes.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_SceneNodes.At(archIdx, compIdx).Traversed = false;
			}
		}
	}

	UApplyMatricesToRealWorldTransforms::UApplyMatricesToRealWorldTransforms(URegistry& registry)
	{
		URealTransform2D realTransform;
		FMatrix4x4f matrix{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(realTransform, matrix);

		registry.GetComponents(m_RealTransforms, filter);
		registry.GetComponents(m_Matrices, filter);
	}

	void UApplyMatricesToRealWorldTransforms::Update()
	{
		for (size_t archIdx = 0; archIdx < m_Matrices.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Matrices.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_RealTransformModifier.Reset(&m_RealTransforms.At(archIdx, compIdx));
				SMatrix::ToTransform(m_Matrices.At(archIdx, compIdx), m_RealTransformModifier.GetRealWorldTransform(&m_RealTransforms.At(archIdx, compIdx)));
			}
		}
	}

	UApplyHierarchyIndependentWorldTransformsToMatrix::UApplyHierarchyIndependentWorldTransformsToMatrix(URegistry& registry)
	{
		FMatrix4x4f matrix{};
		FWorldTransform2D independentWorldTransform{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(matrix, independentWorldTransform);

		registry.GetComponents(m_Matrices, filter);
		registry.GetComponents(m_IndependentWorldTransforms, filter);
	}

	void UApplyHierarchyIndependentWorldTransformsToMatrix::Update()
	{
		for (size_t archIdx = 0; archIdx < m_IndependentWorldTransforms.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_IndependentWorldTransforms.NumOfComponents(archIdx); compIdx++)
			{
				SMatrix::ApplyTransform(m_Matrices.At(archIdx, compIdx), m_IndependentWorldTransforms.At(archIdx, compIdx).Transform);
			}
		}
	}

	UApplyMatricesToScreenTransform::UApplyMatricesToScreenTransform(URegistry& registry)
	{
		URealTransform2D worldTransform;
		FMatrix4x4f matrix{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(worldTransform, matrix);

		registry.GetComponents(m_RealTransforms, filter);
		registry.GetComponents(m_Matrices, filter);
	}

	void UApplyMatricesToScreenTransform::Update()
	{
		for (size_t archIdx = 0; archIdx < m_Matrices.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Matrices.NumOfActivatedComponents(archIdx); compIdx++)
			{
				SMatrix::ToTransform(m_Matrices.At(archIdx, compIdx), m_RealTransformModifier.GetScreenTransform(&m_RealTransforms.At(archIdx, compIdx)));
			}
		}
	}

	UApplyRealTransformToRects::UApplyRealTransformToRects(URegistry& registry)
	{
		FRectCollider rectCollider{};
		URealTransform2D realTransform{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(rectCollider, realTransform);

		registry.GetComponents(m_RectColliders, filter);
		registry.GetComponents(m_RealTransforms, filter);
	}
	void UApplyRealTransformToRects::Update()
	{
		for (size_t archIdx = 0; archIdx < m_RealTransforms.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_RealTransforms.NumOfActivatedComponents(archIdx); compIdx++)
			{
				SMatrix::ApplyTransform(m_RectColliders.At(archIdx, compIdx), m_RealTransforms.At(archIdx, compIdx).GetConstScreenTransform());
			}
		}
	}

	URectSceneGraph::URectSceneGraph(URegistry& registry) : 
		m_InitializeSceneObjects(registry),
		m_ApplySceneObjectTransformHierarchy(registry),
		m_ResetSceneNodesTraversal(registry),
		m_ApplyLocalTransformsToMatrix(registry),
		m_ApplyMatricesToWorldTransforms(registry),
		m_ApplyMatricesToScreenTransform(registry),
		m_ApplyRealTransformToRects(registry),
		m_ApplyCameraTransformToWorld(registry),
		m_ResetMatrices(registry){}

	void URectSceneGraph::InitializeSceneGraph()
	{
		m_InitializeSceneObjects.InitializeSceneGraph();
		Update();
	}

	void URectSceneGraph::InitializeSceneObjects()
	{
		m_InitializeSceneObjects.InitializeSceneObjects();
	}

	void URectSceneGraph::Update()
	{
		m_ApplyLocalTransformsToMatrix.Update();
		m_ApplySceneObjectTransformHierarchy.Update();

		m_ApplyMatricesToWorldTransforms.Update();
		m_ApplyCameraTransformToWorld.Update();
		m_ApplyMatricesToScreenTransform.Update();

		m_ApplyRealTransformToRects.Update();

		m_ResetMatrices.Reset();
		m_ResetSceneNodesTraversal.Reset();
	}


	USceneManagementSystem::USceneManagementSystem(URegistry& registry) :
		m_SceneRegistry(registry),
		m_Registry(registry)
	{
	}

	void USceneManagementSystem::Initialize(size_t sceneIndex)
	{

		if (sceneIndex >= m_SceneRegistry.Get().SceneEntities.size())
		{
			return;
		}

		for (size_t sceneIdx = 0; sceneIdx < m_SceneRegistry.Get().SceneEntities.size(); sceneIdx++)
		{
			if (sceneIdx != sceneIndex)
			{
				for (int enttIdx = m_SceneRegistry.Get().SceneEntities.at(sceneIdx).NumOfEntities() - 1; enttIdx >= 0 ; --enttIdx)
				{
					if (enttIdx == 430)
					{
						bool yep = true;
					}
					m_Registry.DeactivateEntity(m_SceneRegistry.Get().SceneEntities.at(sceneIdx).ConstEntityAt(enttIdx).GetConstEntity());
				}
			}
			else
			{
				for (size_t enttIdx = 0; enttIdx < m_SceneRegistry.Get().SceneEntities.at(sceneIdx).NumOfEntities(); enttIdx++)
				{
					if (m_SceneRegistry.Get().SceneEntities.at(sceneIdx).ConstValueAt(enttIdx) == 
						EActivationStateForOptimization::ActiveBeforeDeactivating)
					{
						m_Registry.ActivateEntity(m_SceneRegistry.Get().SceneEntities.at(sceneIdx).ConstEntityAt(enttIdx).GetConstEntity());

					}
				}
			}
			
		}
		
	}

	void USceneManagementSystem::NextScene()
	{
		if (m_SceneRegistry.Get().CurrentSceneIndex < (m_SceneRegistry.Get().SceneEntities.size() - 1))
		{
			SetScene(m_SceneRegistry.Get().CurrentSceneIndex + 1);
		}
	}

	void USceneManagementSystem::PreviousScene()
	{
		if (m_SceneRegistry.Get().CurrentSceneIndex > 0)
		{
			SetScene(m_SceneRegistry.Get().CurrentSceneIndex - 1);
		}
	}

	void USceneManagementSystem::SetScene(size_t sceneIndex)
	{
		size_t currentSceneIndex = m_SceneRegistry.Get().CurrentSceneIndex;
		
		if (currentSceneIndex == sceneIndex || 
			sceneIndex >= m_SceneRegistry.Get().SceneEntities.size())
		{
			return;
		}

		int activationCount{ 0 };

		for (int enttIdx = m_SceneRegistry.Get().SceneEntities.at(currentSceneIndex).NumOfEntities() - 1; enttIdx >= 0 ; --enttIdx)
		{
			m_Registry.DeactivateEntity(m_SceneRegistry.Get().SceneEntities.at(currentSceneIndex).ConstEntityAt(enttIdx).GetConstEntity());
		}

		for (size_t enttIdx = 0; enttIdx < m_SceneRegistry.Get().SceneEntities.at(sceneIndex).NumOfEntities(); enttIdx++)
		{
			if (m_SceneRegistry.Get().SceneEntities.at(sceneIndex).ConstValueAt(enttIdx) ==
				EActivationStateForOptimization::ActiveBeforeDeactivating)
			{
				m_Registry.ActivateEntity(m_SceneRegistry.Get().SceneEntities.at(sceneIndex).ConstEntityAt(enttIdx).GetConstEntity());
				++activationCount;
			}
		}

		m_SceneRegistry.Get().CurrentSceneIndex = sceneIndex;
		activationCount;
	}

	void USceneManagementSystem::SkipToScene(int sceneDelta)
	{
		int currentScene = m_SceneRegistry.Get().CurrentSceneIndex + sceneDelta;
		if (currentScene >= 0 || currentScene <= (m_SceneRegistry.Get().SceneEntities.size() - 1))
		{
			SetScene(currentScene);
		}
	}

	//bool SSceneObject::MakeParent(const UEntityHandle& parent, const UEntityHandle& newChild, bool changeCurrentParentOfChild, URegistry& registry)
	//{
	//	URegistrySingleton<FRegistry_SceneObjectRoot> sceneObjectRootNodes{registry};

	//	FSceneObject* parentSceneObject = registry.GetComponent<FSceneObject>(parent.GetConstEntity());
	//	FSceneObject* childSceneObject = registry.GetComponent<FSceneObject>(newChild.GetConstEntity());

	//	if (parentSceneObject && childSceneObject)
	//	{
	//		if (parentSceneObject->Children.Contains(newChild) && 
	//			childSceneObject->Parent.GetConstEntity().Archetype_Id == parent.GetConstEntity().Archetype_Id &&
	//			childSceneObject->Parent.GetConstEntity().Entity_Id == parent.GetConstEntity().Entity_Id)
	//		{
	//			return true;
	//		}
	//		if ((childSceneObject->Parent.IsValid() && changeCurrentParentOfChild) || !childSceneObject->Parent.IsValid())
	//		{
	//			sceneObjectRootNodes.Get().RootEntities.Remove(newChild);
	//			const size_t& parentIndex = sceneObjectRootNodes.Get().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(parent);
	//			size_t& childIndex = sceneObjectRootNodes.Get().Entity_To_EntitySceneHierarchyIndex.ValueAt(newChild);

	//			if (parentIndex != childIndex)
	//			{
	//				sceneObjectRootNodes.Get().SceneHierarchies.at(childIndex).Remove(newChild);
	//				sceneObjectRootNodes.Get().SceneHierarchies.at(parentIndex).Add(newChild);
	//				childIndex = parentIndex;
	//			}

	//			FSceneObject* oldParentSceneObject = registry.GetComponent<FSceneObject>(childSceneObject->Parent.GetConstEntity());

	//			if (oldParentSceneObject)
	//			{
	//				oldParentSceneObject->Children.Remove(newChild);
	//			}

	//			childSceneObject->Parent = parent;
	//			parentSceneObject->Children.Add(newChild); 

	//			return true;
	//		}
	//	}

	//	return false;
	//}

	//bool SSceneObject::RemoveParent(const UEntityHandle& child, URegistry& registry)
	//{
	//	URegistrySingleton<FRegistry_SceneObjectRoot> sceneObjectRootNodes{ registry };

	//	FSceneObject* childSceneObject = registry.GetComponent<FSceneObject>(child.GetConstEntity());

	//	if (childSceneObject)
	//	{
	//		FSceneObject* parentSceneObject = registry.GetComponent<FSceneObject>(childSceneObject->Parent.GetConstEntity());

	//		if (parentSceneObject && parentSceneObject->Children.Contains(child))
	//		{
	//			sceneObjectRootNodes.Get().RootEntities.Add(child);
	//			const size_t& parentIndex = sceneObjectRootNodes.Get().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(childSceneObject->Parent);
	//			size_t& childIndex = sceneObjectRootNodes.Get().Entity_To_EntitySceneHierarchyIndex.ValueAt(child);

	//			if (parentIndex != childIndex)
	//			{
	//				sceneObjectRootNodes.Get().SceneHierarchies.at(childIndex).Remove(child);
	//				sceneObjectRootNodes.Get().SceneHierarchies.at(parentIndex).Add(child);
	//				childIndex = parentIndex;
	//			}

	//			FSceneObject* oldParentSceneObject = registry.GetComponent<FSceneObject>(childSceneObject->Parent.GetConstEntity());

	//			if (oldParentSceneObject)
	//			{
	//				oldParentSceneObject->Children.Remove(child);
	//			}

	//			parentSceneObject->Children.Add(child);

	//			return true;
	//		}
	//		
	//	}

	//	return false;
	//}

	

}
