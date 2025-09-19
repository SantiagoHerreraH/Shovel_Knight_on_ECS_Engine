#pragma once

#include <deque>

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/URegistrySingleton.h"

#include "../../../include/Structs/FSceneGraphStructure.h"
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FMatrix.h"

#include "../../../include/Systems/TransformSystems/URealTransform2D.h"
#include "../../../include/Systems/TransformSystems/URealTransform2DModifier.h"
#include "../../../include/Systems/TransformSystems/UShapeTransformer2D.h"
#include "../../../include/Systems/TransformSystems/UCameraTransformer.h"
#include "../../../include/Structs/FState.h"
#include "../../../include/Systems/OptimizationSystems/UOptimization.h"

namespace Pillar {

	/*class SSceneObject {
		bool MakeParent(const UEntityHandle& parent, const UEntityHandle& newChild, bool changeCurrentParentOfChild, URegistry& registry);
		bool RemoveParent(const UEntityHandle& child, URegistry& registry);
	};*/

	struct FSceneState {
		int SceneDeltaFromCurrentState{ 0 };
		EStateRemovalType StateRemovalType{};
	};

	class USceneManagementSystem {

	public:
		USceneManagementSystem(URegistry& registry);

		void Initialize(size_t sceneIndex);
		void NextScene();
		void PreviousScene();
		void SetScene(size_t sceneIndex);
		void SkipToScene(int sceneDelta);

	private:

		URegistrySingleton<FSceneRegistry> m_SceneRegistry;
		URegistry& m_Registry;
	};

	class UInitializeSceneObjects final{

	public:

		UInitializeSceneObjects(URegistry& registry);

		void InitializeSceneGraph();
		void InitializeSceneObjects();

	private:

		void RegisterRootNodes();
		void RegisterSceneObjects();
		void RegisterHierarchies();

		const size_t m_EstimatedNumberOfScenes{ 10 };

		URegistry& m_Registry;
		USceneManagementSystem m_SceneManagementSystem;
		URegistrySingleton<FSceneRegistry> m_SceneRegistry;
		UComponentIterator<FSceneNode> m_SceneNodes;
		UComponentIterator<FSceneObject> m_SceneObjects;

	};

	class UApplySceneObjectTransformHierarchy final{

	public:
		UApplySceneObjectTransformHierarchy(URegistry& registry);

		void Update();
		void Update_New();

	private:
		URegistry& m_Registry;

		URegistrySingleton<FSceneRegistry> m_SceneObjectRootNodes;

		UComponentIterator<FWorldTransform2D> m_WorldTransforms;
		UComponentIterator<FMatrix4x4f> m_Matrices;

		std::deque<UEntityHandle> m_CurrentEntityQueue;
	};

	class UApplyHierarchyIndependentWorldTransformsToMatrix {

	public:
		UApplyHierarchyIndependentWorldTransformsToMatrix(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FMatrix4x4f> m_Matrices;
		UComponentIterator<FWorldTransform2D> m_IndependentWorldTransforms;

	};

	class UResetSceneNodesTraversal final{

	public:
		UResetSceneNodesTraversal(URegistry& registry);
		~UResetSceneNodesTraversal(){}

		void Reset();

	private:
		UComponentIterator<FSceneNode> m_SceneNodes;
	};

	class UApplyMatricesToRealWorldTransforms {
		
	public:

		UApplyMatricesToRealWorldTransforms(URegistry& registry);

		void Update();

	private:
		UComponentIterator<FMatrix4x4f> m_Matrices;
		UComponentIterator<URealTransform2D> m_RealTransforms;

		URealTransform2DModifier m_RealTransformModifier;

	};

	class UApplyMatricesToScreenTransform {
		
	public:
		UApplyMatricesToScreenTransform(URegistry& registry);
		void Update();


	private:
		UComponentIterator<FMatrix4x4f>		m_Matrices;
		UComponentIterator<URealTransform2D>	m_RealTransforms;

		URealTransform2DModifier m_RealTransformModifier;
	};

	class UApplyRealTransformToRects {
	public:
		UApplyRealTransformToRects(URegistry& registry);
		void Update();
	private:
		UComponentIterator<FRectCollider> m_RectColliders;
		UComponentIterator<URealTransform2D> m_RealTransforms;
	};

	class USceneGraph final {
	
	public:
		USceneGraph(URegistry& registry);

		void Initialize();
		void Update();
	
	private:
		UInitializeSceneObjects						m_RegisterRootNodes;
		UApplySceneObjectTransformHierarchy		m_ApplySceneObjectTransformHierarchy;
		UResetSceneNodesTraversal				m_ResetSceneNodesTraversal;

		UApplyLocalTransform2DToMatrix				m_ApplyTransformToMatrix;
		UApplyMatrixToShape2D					m_ApplyMatrixToShape;
		UApplyMatricesToRealWorldTransforms			m_ApplyMatricesToWorldTransforms;
		UApplyMatricesToScreenTransform			m_ApplyMatricesToScreenTransform;

		UApplyCameraTransformToWorld			m_ApplyCameraTransformToWorld;

		UCalculateShapeCenters					m_CalculateShapeCenters;
		UResetMatrices							m_ResetMatrices;
	
	};

	class URectSceneGraph final{
	public:
		URectSceneGraph(URegistry& registry);

		void InitializeSceneGraph();
		void InitializeSceneObjects();
		void Update();

	private:
		UInitializeSceneObjects					m_InitializeSceneObjects;
		UApplySceneObjectTransformHierarchy		m_ApplySceneObjectTransformHierarchy;
		UResetSceneNodesTraversal				m_ResetSceneNodesTraversal;

		UApplyLocalTransform2DToMatrix			m_ApplyLocalTransformsToMatrix;
		UApplyMatricesToRealWorldTransforms			m_ApplyMatricesToWorldTransforms;
		UApplyMatricesToScreenTransform			m_ApplyMatricesToScreenTransform;

		UApplyRealTransformToRects				m_ApplyRealTransformToRects;

		UApplyCameraTransformToWorld			m_ApplyCameraTransformToWorld;

		UResetMatrices							m_ResetMatrices;
	};
}