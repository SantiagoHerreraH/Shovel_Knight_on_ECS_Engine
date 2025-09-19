#pragma once

#include <unordered_set>
#include <unordered_map>

#include "../../../../../Engine/Transform.h"
#include "base.h"

#include "../../../include/Structs/FColor.h"
#include "../../../include/Structs/FVector.h"
#include "../../../include/Systems/TemplateSystems/TActivatableVector.h"
#include "../../../include/Systems/TemplateSystems/TActivatableFlatMap.h"

namespace Pillar {

	using DTextureTransform = Transform;
	using DTexturePixelData = std::vector<std::vector<std::array<GLubyte, 4>>>;

	class UTextureRef;
	class UTexture;
	class UBaseTexture;

	class UTextureData {

	public:
		UTextureData() {}
		explicit UTextureData(const UBaseTexture& texture);
		~UTextureData();
		UTextureData(const UTextureData& other);
		UTextureData(UTextureData&& other)noexcept;

		UTextureData& operator=(const UTextureData& other);
		UTextureData& operator=(UTextureData&& other)noexcept;

		size_t NumberOfRows()const;
		size_t NumberOfColumns()const;
		size_t NumberOfPixels()const;

		FColor4i PixelAt(size_t column, size_t row)const;
		FColor4i PixelAt(size_t pixelIndex)const;//count increases horizontally

		bool PixelIsInBounds(int x, int y)const;

		std::vector<Rectf> MakeRectsFromPixelsOfColor(const FColor4i& color, float xSize = 1.f,  float ySize = 1.f)const;

		bool IsValid()const;

	private:
		bool m_IsValid{ false };
		const int m_RGBA_Num{ 4 };
		GLubyte* m_Pixels{nullptr};
		size_t m_TextureWidth{0};
		size_t m_TextureHeight{0};

	};

	//This class is not instantiable
	class UBaseTexture {

	public:
		void SetScreenPosition(const Point2f& positionInScreen);
		void SetScale(const Point2f& scaleInScreen);
		void SetTextureView(const Rectf& textureView);

		const Point2f& GetPositionInScreen()const;
		const Point2f& GetScaleInScreen()const;
		const Rectf& GetTextureView()const;

		float GetTextureOriginalWidth()const;
		float GetTextureOriginalHeight()const;

		float GetTextureScaledWidth()const;
		float GetTextureScaledHeight()const;

		void Draw()const;

		bool IsValid()const;

	protected:

		friend class UTextureData;

		void DrawFilledRect(const Rectf& rect) const;

		GLuint m_Id{ 0 };
		float m_TextureWidth{ 0 }; //values changed on initialization
		float m_TextureHeight{ 0 };
		Point2f m_TexturePositionInScreen{};
		Point2f m_TextureScaleInScreen{1,1};
		Rectf m_TextureView = Rectf(0, 0, 10, 10);
		bool m_IsValid{ false };
	};

	class UTexture final : public UBaseTexture {
	
	public:
		
		UTexture(const std::string& imagePath);
		UTexture(const std::string& text, TTF_Font* pFont, const Color4f& textColor);
		UTexture(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor);
		
		UTexture(const UTexture& other);
		UTexture(UTexture&& other)noexcept;

		UTexture& operator=(UTexture&& other)noexcept;
		UTexture& operator=(const UTexture& other);
		
		~UTexture();

		void GetReference(UTextureRef& outReference);
		void RemoveReference(UTextureRef& outReference);
		
	private:

		void RegisterTextureInstance();
		void DeregisterTextureInstance();
		static std::unordered_map<GLuint, std::unordered_set<UTexture*>> s_TextureId_To_InstancePtrs;

		std::unordered_set<UTextureRef*> m_TextureReferencePointers;

		void RefreshTextureReferencePointers();
		void CreateFromImage(const std::string& path);
		void CreateFromString(const std::string& text, TTF_Font* pFont, const Color4f& textColor);
		void CreateFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor);
		void CreateFromSurface(SDL_Surface* pSurface);
	
	};

	class UTextureRef final : public UBaseTexture {

	public:
		UTextureRef() {}
		UTextureRef(UTexture& texture, const Rectf& textureView, const Point2f& positionInScreen, const Point2f& scaleInScreen);
		UTextureRef(UTexture& texture);
		UTextureRef(const UTextureRef& other);
		UTextureRef(UTextureRef&& other)noexcept;
		~UTextureRef();

		UTextureRef& operator=(const UTextureRef& other);
		UTextureRef& operator=(UTextureRef&& other)noexcept;

	private:

		UTexture* m_ReferencedTexture{ nullptr };
		friend class UTexture;
	};

	class USpriteSheet final{
	public:
		USpriteSheet(const UTexture& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixelsHorizontal, unsigned int numberOfEmptyPixelsVertical, const Point2f& scaleInScreen = Point2f{1,1});
	
		UTextureRef GetFrameCopy(size_t rowIndex, size_t columnIndex);
		const UTexture& GetFrame(size_t rowIndex, size_t columnIndex);

		void SetPositionInScreen(const Point2f& position);
		void SetScaleInScreen(const Point2f& scaleInScreen);
		
		const Point2f& GetPositionInScreen()const;
		const Point2f& GetScaleInScreen()const;

		int GetOriginalCellWidth()const;
		int GetOriginalCellHeight()const;

		float GetScaledCellWidth()const;
		float GetScaledCellHeight()const;
		
		bool IsValid()const;
	
	private:

		void FrameTexture(size_t rowIndex, size_t columnIndex);
		UTexture m_Texture;
		size_t m_NumberOfRows; 
		size_t m_NumberOfColumns; 
		size_t m_NumberOfEmptyPixelsBetweenColumns;
		size_t m_NumberOfEmptyPixelsBetweenRows;
		size_t m_CellWidth{};
		size_t m_CellHeight{};
		
	};

	struct FTextureMapRule {
		FColor4i Color;

		bool Check_R{ true };
		bool Check_G{ true };
		bool Check_B{ true };
		bool Check_A{ true };

		int RelativeX{};
		int RelativeY{};

		bool Equals{true};
	};

	class UTextureMapRuleBook final{

	public:

		const FColor4i& GetCenterColor()const;
		void SetCenterColor(const FColor4i& color);

		void AddRule(const FTextureMapRule& rule);
		void AddColorRule(const FColor4i& color, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals);
		void AddRedRule(size_t value,	int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals);
		void AddGreenRule(size_t value, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals);
		void AddBlueRule(size_t value,	int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals);
		void AddAlphaRule(size_t value, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals);
		

		size_t NumberOfRules()const;
		void ClearRules();

		bool RuleBookIsFulfilled(int columnIndex, int rowIndex, const UTextureData& textureData)const;

	private:
		FColor4i m_Color;
		std::vector<FTextureMapRule> m_Rules;
	};

	struct FTileInfo {

		FTileInfo() {};
		explicit FTileInfo(size_t spriteSheetColIndex, size_t spriteSheetRowIndex, int lvlColIndex, int lvlRowIndex) :
			SpriteSheetColumnIndex(spriteSheetColIndex),
			SpriteSheetRowIndex(spriteSheetRowIndex),
			LevelColumnIndex(lvlColIndex),
			LevelRowIndex(lvlRowIndex){}

		size_t SpriteSheetColumnIndex{};
		size_t SpriteSheetRowIndex{};
		int LevelColumnIndex{};
		int LevelRowIndex{};
	};

	struct  FTileTypeInfo {

		explicit FTileTypeInfo(const UTextureMapRuleBook& ruleBook, size_t spriteSheetRowIndex, size_t spriteSheetColIndex) :
			Rules(ruleBook),
			SpriteSheetRowIndex(spriteSheetRowIndex),
			SpriteSheetColumnIndex(spriteSheetColIndex) {}

		UTextureMapRuleBook Rules{};
		size_t SpriteSheetRowIndex{};
		size_t SpriteSheetColumnIndex{};
	};

	inline bool operator<(const FTileTypeInfo& one, const FTileTypeInfo& two) {
		return one.Rules.NumberOfRules() < two.Rules.NumberOfRules();
	}

	struct FTileGroupInfo {
		std::vector<FTileTypeInfo> TileTypeInfo;
		bool Sorted{false};
	};

	class UTextureMap final{

	public:
		UTextureMap(const USpriteSheet& spriteSheet);

		void SetScale(const Point2f& scale);
		const Point2f& GetScale()const;
		const USpriteSheet& GetSpriteSheet()const;
		const UTextureData& GetTextureData()const;

		//input offsets in absolute values
		void SetRulesForTileBlock(int topRowIndex, int leftColumnIndex, int bottomRowIndex, int rightColumnIndex, const FColor4i& color, bool takeIntoAccountBorders = true, int topBorderOffset = 0, int bottomBorderOffset = 0, int leftBorderOffset = 0, int rightBorderOffset = 0);
		void SetRuleForTile(size_t rowIndexForFrameInSpriteSheet, size_t columnIndexForFrameInSpriteSheet, const UTextureMapRuleBook& textureMapRule);

		void SetBorderTile(size_t rowIndexForFrameInSpriteSheet, size_t columnIndexForFrameInSpriteSheet, const FColor4i& mainColor, bool right, bool left, bool top, bool bottom, const std::vector<FColor4i>& colorsToOmit = {});
		void SetDiagonalBorderTile(size_t rowIndexForFrameInSpriteSheet, size_t columnIndexForFrameInSpriteSheet, const FColor4i& color, bool right, bool left, bool top, bool bottom, const std::vector<FColor4i>& colorsToOmit = {});

		void SetMap(const UTextureData& textureData);
		std::vector<Rectf> GetDrawnMapBoundsData()const;
		std::vector<Rectf> GetDrawnMapBoundsData(const FColor4i& color)const;
		std::vector<Rectf> GetRawMapBoundsData(const FColor4i& color)const;

		const std::vector<Rectf> ExtractRectsOfColor(const FColor4i& color)const;
		void Draw(const Rectf& cameraWorldRect)const;

	private:
		void SetRuleForTileInTileBlock(int currentRowIndex, int currentColumnIndex, int topRowIndex, int leftColumnIndex, int bottomRowIndex, int rightColumnIndex, const FColor4i& colorbool, bool takeIntoAccountBorders = true, int topBorderOffset = 0, int bottomBorderOffset = 0, int leftBorderOffset = 0, int rightBorderOffset = 0);
		void Implementation_SetMap(const UTextureData& textureData)const;

		//I know mutable is not the best, but it's either this or more memory usage + slower draw (each tile would have to be an independent entity)
		mutable TActivatableFlatMap<FColor4i, FTileGroupInfo> m_Color_To_TileGroup;
		mutable std::vector<FTileInfo> m_MapData;
		mutable std::vector<FColor4i> m_MapColorData;

		mutable bool m_HaveToCalculateMapData{ true };

		mutable USpriteSheet m_SpriteSheet;
		mutable UTextureData m_TextureData;
	};

	class UAnimatedTextureMap final {


	public:
		UAnimatedTextureMap(int framesPerSecond = 12);

		void AddTextureMap(const UTextureMap& textureMap);

		void Update(float deltaSeconds);
		void Draw(const Rectf& cameraWorldRect)const;

	private:

		struct FInternalTextureMapInfo {
			UTextureMap TextureMap;
		};

		std::vector<FInternalTextureMapInfo> m_TextureMapInfo;

		float m_Counter{0.f};
		float m_TimeToSwitchFrame{0};
		int m_CurrentIndex{ 0 };

	};

	struct FSceneTexture {
		UTexture Texture;
	};
}