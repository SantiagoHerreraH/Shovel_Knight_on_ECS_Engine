#pragma once

#include <vector>

namespace Pillar {

	template<typename Type>
	class TReference {

	public:
		bool IsValid()const;
		Type* const Get();
		const Type* const GetConst()const;

	private:
		Type* m_TypePtr{ nullptr };
	};

	template<typename Type>
	class TVectorReference {

	public:
		bool IsValid()const;
		TReferencedVector<Type>* const Get();
		const TReferencedVector<Type>* const GetConst()const;

	private:
		TReferencedVector<Type>* m_ReferencedVectorPtr{ nullptr };
	};

	template<typename Type>
	class TReferencedVector {

	public:
		Type& At(size_t index);

	private:
		std::vector<Type>

	};
}