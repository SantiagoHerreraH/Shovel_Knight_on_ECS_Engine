#pragma once

#include "../../../include/Core/URegistrySingleton.h"
#include "../../../include/Systems/DrawSystems/UTexture.h"


namespace Pillar {


	struct STextureNames {

		const static std::string Water;
		const static std::string MainLevel;
		const static std::string Tiles;
		const static std::string ShovelKnight;
		const static std::string DiveDrake;
		const static std::string Beetle;
		const static std::string Blorb;
		const static std::string Level;
		const static std::string DirtBlock;
		const static std::string CheckPoint;
		const static std::string Treasure;
		const static std::string MoneyPoof;
		const static std::string Shovel;
		const static std::string AttackForward;
		const static std::string AttackDown;
	};

	struct FTexture_Registry {
		std::unordered_map<std::string, UTexture> s_TextureName_To_Texture;
	};

	class STextureRegistry {

	public:
		~STextureRegistry();
		static UTexture& GetTexture(const std::string& textureName);
		static const UTexture& GetConstTexture(const std::string& textureName);
		static UTextureRef GetTextureRef(const std::string& textureName);
		static const UTexture& GetConstFont(const std::string& text, const std::string& fontFile, int pointSize, const Color4f& color);
		static UTexture& GetFont(const std::string& text, const std::string& fontFile, int pointSize, const Color4f& color);
		static void Delete();

	private:
		static FTexture_Registry* s_Texture_Registry;
	};

	
}