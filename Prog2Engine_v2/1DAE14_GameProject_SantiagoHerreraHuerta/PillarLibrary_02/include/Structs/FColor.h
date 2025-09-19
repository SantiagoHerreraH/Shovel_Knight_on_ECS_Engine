#pragma once

#include <vector>

namespace Pillar{

	//range is 0 to 255
	struct FColor4i {
		FColor4i() {}
		explicit FColor4i(size_t r, size_t g, size_t b, size_t a)
		{
			SetR(r);
			SetG(g);
			SetB(b);
			SetA(a);
		}

		size_t GetR()const{return m_R;}
		size_t GetG()const{return m_G;}
		size_t GetB()const{return m_B;}
		size_t GetA()const{return m_A;}

		static const size_t NUMBER_OF_VARIABLES{ 4 };
		static const size_t MAXVALUE{ 255 };

		void SetR(size_t value) {  m_R = value > MAXVALUE ? MAXVALUE : value; }
		void SetG(size_t value) {  m_G = value > MAXVALUE ? MAXVALUE : value; }
		void SetB(size_t value) {  m_B = value > MAXVALUE ? MAXVALUE : value; }
		void SetA(size_t value) {  m_A = value > MAXVALUE ? MAXVALUE : value; }

	private:
		
		size_t m_R{0};
		size_t m_G{0};
		size_t m_B{0};
		size_t m_A{0};

	};

	inline bool operator==(const FColor4i& one, const FColor4i& two) {
		return 
			(one.GetR() == two.GetR()) &&
			(one.GetG() == two.GetG()) && 
			(one.GetB() == two.GetB()) && 
			(one.GetA() == two.GetA());
	}

}

template <>
struct std::hash<Pillar::FColor4i>
{
	std::size_t operator()(const Pillar::FColor4i& color) const
	{
		return color.GetR() + (color.GetG() * color.MAXVALUE) + (color.GetB() * color.MAXVALUE * 2) + (color.GetA() * color.MAXVALUE * 3);
	}
};



