#include "../../../include/Systems/CreationSystems/ULoadTextures.h"

namespace Pillar {

	//
	const std::string STextureNames::Water{ "Levels/Level_PlainsOfPassage_Water.png" };
	const std::string STextureNames::MainLevel{ "Levels/Level_PlainsOfPassage_MainLevel.png" };
	const std::string STextureNames::Tiles{ "Levels/Tiles_PlainsOfPassage_1.png" };
	const std::string STextureNames::ShovelKnight{"ShovelKnight_SpriteSheet.png"};
	const std::string STextureNames::DiveDrake{"Enemies/DiveDrake_SpriteSheet.png"};
	const std::string STextureNames::Beetle{"Enemies/Beetle_SpriteSheet.png"};
	const std::string STextureNames::Blorb{"Enemies/Blorb_SpriteSheet.png"};
	const std::string STextureNames::Level{"Levels/Level_01.png"};
	const std::string STextureNames::DirtBlock{ "Objects/dirtBlock.png" };
	const std::string STextureNames::CheckPoint{ "Objects/Checkpoint.png" };
	const std::string STextureNames::Treasure{"Objects/Treasures.png"};
	const std::string STextureNames::MoneyPoof{ "Objects/moneyPoof.png" };
	const std::string STextureNames::Shovel{ "Menu/MenuShovel.png" };
	const std::string STextureNames::AttackForward{"Objects/Effect_AttackForward.png"};
	const std::string STextureNames::AttackDown{"Objects/Effect_AttackDown.png"};

	FTexture_Registry* STextureRegistry::s_Texture_Registry{nullptr};

	STextureRegistry::~STextureRegistry()
	{
		if (s_Texture_Registry)
		{
			delete s_Texture_Registry;
		}
	}

	UTexture& STextureRegistry::GetTexture(const std::string& textureName)
	{
		if (!s_Texture_Registry)
		{
			s_Texture_Registry = new FTexture_Registry();
		}

		if (!s_Texture_Registry->s_TextureName_To_Texture.contains(textureName))
		{
			s_Texture_Registry->s_TextureName_To_Texture.emplace(std::pair(textureName, UTexture(textureName)));
		}

		return s_Texture_Registry->s_TextureName_To_Texture.at(textureName);
	}
	const UTexture& STextureRegistry::GetConstTexture(const std::string& textureName)
	{
		return GetTexture(textureName);
	}
	UTextureRef STextureRegistry::GetTextureRef(const std::string& textureName)
	{
		return UTextureRef(GetTexture(textureName));
	}
	void STextureRegistry::Delete()
	{
		delete s_Texture_Registry;
	}
	const UTexture& STextureRegistry::GetConstFont(const std::string& text, const std::string& fontFile, int pointSize, const Color4f& color)
	{
		return GetFont(text, fontFile, pointSize, color);
	}
	UTexture& STextureRegistry::GetFont(const std::string& text, const std::string& fontFile, int pointSize, const Color4f& color)
	{
		if (!s_Texture_Registry)
		{
			s_Texture_Registry = new FTexture_Registry();
		}

		if (!s_Texture_Registry->s_TextureName_To_Texture.contains(text))
		{
			s_Texture_Registry->s_TextureName_To_Texture.emplace(std::pair(text, UTexture(fontFile, fontFile, pointSize, color)));
		}

		return s_Texture_Registry->s_TextureName_To_Texture.at(text);
	}
}