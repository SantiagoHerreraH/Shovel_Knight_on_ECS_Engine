#pragma once

#include <vector>


namespace Pillar {

	template<typename Type, typename Comparer>
	class TSortedVector final {

	public:
		TSortedVector();
		TSortedVector();
		TSortedVector();

		

	private:

		std::vector<Type> m_Vector;
		Comparer m_Comparer;
	};

	
}