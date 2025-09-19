
#include "../../../include/Systems/DrawSystems/UTexture.h"
#include "utils.h"
#include <iostream>

namespace Pillar {

#pragma region Base Texture


	
	void UBaseTexture::SetScreenPosition(const Point2f& positionInScreen)
	{
		m_TexturePositionInScreen = positionInScreen;
	}
	void UBaseTexture::SetScale(const Point2f& scaleInScreen)
	{
		m_TextureScaleInScreen = scaleInScreen;
	}
	void UBaseTexture::SetTextureView(const Rectf& textureView)
	{
		m_TextureView = textureView;
	}
	const Point2f& UBaseTexture::GetPositionInScreen() const
	{
		return m_TexturePositionInScreen;
	}
	const Point2f& UBaseTexture::GetScaleInScreen() const
	{
		return m_TextureScaleInScreen;
	}
	const Rectf& UBaseTexture::GetTextureView() const
	{
		return m_TextureView;
	}
	
	float UBaseTexture::GetTextureOriginalWidth()const
	{
		return m_TextureWidth;
	}
	float UBaseTexture::GetTextureOriginalHeight()const
	{
		return m_TextureHeight;
	}

	float UBaseTexture::GetTextureScaledWidth()const
	{
		return m_TextureScaleInScreen.x * m_TextureWidth;
	}

	float UBaseTexture::GetTextureScaledHeight()const
	{
		return m_TextureScaleInScreen.y * m_TextureHeight;
	}
	
	SDL_Texture* renderText(const std::string& message, const std::string& fontFile, SDL_Color color, int fontSize, SDL_Renderer* renderer) {
		// Load the font
		TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
		if (!font) {
			std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
			return nullptr;
		}

		// Render the message to an SDL_Surface
		SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
		if (!surface) {
			std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
			TTF_CloseFont(font);
			return nullptr;
		}

		// Convert the surface to a texture
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (!texture) {
			std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		}

		// Clean up
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);

		return texture;
	}

	void UBaseTexture::Draw() const
	{
		const float epsilon{ 0.001f };
		if (!m_IsValid)
		{
			DrawFilledRect(Rectf{ 0,0,100,100 });
			return;
		}

		// Determine texture coordinates using srcRect and default destination width and height
		float textLeft{};
		float textRight{};
		float textTop{};
		float textBottom{};

		float defaultDestWidth{};
		float defaultDestHeight{};
		if (!(m_TextureView.width > epsilon && m_TextureView.height > epsilon)) // No srcRect specified
		{
			// Use complete texture
			textLeft = 0.0f;
			textRight = 1.0f;
			textTop = 0.0f;
			textBottom = 1.0f;

			defaultDestHeight = m_TextureHeight;
			defaultDestWidth = m_TextureWidth;
		}
		else // srcRect specified
		{
			// Convert to the range [0.0, 1.0]
			textLeft = m_TextureView.left / m_TextureWidth;
			textRight = (m_TextureView.left + m_TextureView.width) / m_TextureWidth;
			textBottom = m_TextureView.bottom / m_TextureHeight;
			textTop = (m_TextureView.bottom - m_TextureView.height) / m_TextureHeight; //this is because the texture (0,0) coordinates are in the top left corner

			defaultDestHeight = m_TextureView.height;
			defaultDestWidth = m_TextureView.width;
		}

		Point2f actualTextureSizeInScreen{ m_TextureScaleInScreen.x * m_TextureView.width, m_TextureScaleInScreen.y * m_TextureView.height};
		// Determine vertex coordinates
		float vertexLeft{ m_TexturePositionInScreen.x };
		float vertexBottom{ m_TexturePositionInScreen.y };
		float vertexRight{};
		float vertexTop{};
		if (!(actualTextureSizeInScreen.x > 0.001f && actualTextureSizeInScreen.y > 0.001f)) // If no size specified use default size
		{
			vertexRight = vertexLeft + defaultDestWidth;
			vertexTop = vertexBottom + defaultDestHeight;
		}
		else
		{
			vertexRight = vertexLeft + actualTextureSizeInScreen.x;
			vertexTop = vertexBottom + actualTextureSizeInScreen.y;

		}

		// Tell opengl which texture we will use
		glBindTexture(GL_TEXTURE_2D, m_Id);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// Draw
		glEnable(GL_TEXTURE_2D);
		{
			glBegin(GL_QUADS);
			{
				glTexCoord2f(textLeft, textBottom);
				glVertex2f(vertexLeft, vertexBottom);

				glTexCoord2f(textLeft, textTop);
				glVertex2f(vertexLeft, vertexTop);

				glTexCoord2f(textRight, textTop);
				glVertex2f(vertexRight, vertexTop);

				glTexCoord2f(textRight, textBottom);
				glVertex2f(vertexRight, vertexBottom);
			}
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}

	bool UBaseTexture::IsValid() const
	{
		return m_IsValid;
	}

	void UBaseTexture::DrawFilledRect(const Rectf& rect) const
	{
		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		{
			glVertex2f(rect.left, rect.bottom);
			glVertex2f(rect.left + rect.width, rect.bottom);
			glVertex2f(rect.left + rect.width, rect.bottom + rect.height);
			glVertex2f(rect.left, rect.bottom + rect.height);
		}
		glEnd();
	}

#pragma endregion

#pragma region Texture

	std::unordered_map<GLuint, std::unordered_set<UTexture*>> UTexture::s_TextureId_To_InstancePtrs{};

	UTexture::UTexture(const std::string& imagePath)
	{
		CreateFromImage(imagePath);
		RegisterTextureInstance();
		m_TextureView.width = m_TextureWidth;
		m_TextureView.height = m_TextureHeight;
	}
	UTexture::UTexture(const std::string& text, TTF_Font* pFont, const Color4f& textColor)
	{
		CreateFromString(text, pFont, textColor);
		RegisterTextureInstance();
	}
	UTexture::UTexture(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor)
	{
		CreateFromString(text, fontPath, ptSize, textColor);
		RegisterTextureInstance();
	}
	UTexture::UTexture(const UTexture& other)
	{
		m_Id = other.m_Id;
		m_TextureWidth = other.m_TextureWidth;
		m_TextureHeight = other.m_TextureHeight;
		m_TexturePositionInScreen = other.m_TexturePositionInScreen;
		m_TextureScaleInScreen = other.m_TextureScaleInScreen;
		m_TextureView = other.m_TextureView;
		m_IsValid = other.m_IsValid;
		//you purposely don't pass this below because the texture refs only hold one UTexture pointer.
		// You pass these pointers when the destructor of this texture is called
		//m_TextureReferencePointers = other.m_TextureReferencePointers; 

		RegisterTextureInstance();
	}
	UTexture::UTexture(UTexture&& other)noexcept
	{
		m_Id = other.m_Id;
		m_TextureWidth = other.m_TextureWidth;
		m_TextureHeight = other.m_TextureHeight;
		m_TexturePositionInScreen = other.m_TexturePositionInScreen;
		m_TextureScaleInScreen = other.m_TextureScaleInScreen;
		m_TextureView = other.m_TextureView;
		m_IsValid = other.m_IsValid;
		m_TextureReferencePointers.swap(other.m_TextureReferencePointers);

		other.DeregisterTextureInstance();
		other.m_Id = 0;
		other.m_IsValid = false;

		RegisterTextureInstance();
		RefreshTextureReferencePointers();
	}

	UTexture::~UTexture()
	{
		DeregisterTextureInstance();

		if (m_IsValid)
		{
			if (s_TextureId_To_InstancePtrs.at(m_Id).empty())
			{
				glDeleteTextures(1, &m_Id);

				for (auto It = m_TextureReferencePointers.begin(); It != m_TextureReferencePointers.end(); It++)
				{
					(*It)->m_Id = 0;
					(*It)->m_IsValid = false;
					(*It)->m_ReferencedTexture = nullptr;
				}
			}
			else
			{
				auto otherInstanceOfSameTexture = (*s_TextureId_To_InstancePtrs.at(m_Id).begin());
				
				for (auto It = m_TextureReferencePointers.begin(); It != m_TextureReferencePointers.end(); It++)
				{
					if ((*It)->m_ReferencedTexture == this)
					{
						(*It)->m_ReferencedTexture = otherInstanceOfSameTexture;
						(*It)->m_Id = otherInstanceOfSameTexture->m_Id;
						(*It)->m_IsValid = otherInstanceOfSameTexture->m_IsValid;
					}
				}
			}
		}
		
	}

	UTexture& UTexture::operator=(UTexture&& other)noexcept
	{
		if (this != &other)// no self assignment
		{
			m_Id = other.m_Id;
			m_TextureWidth = other.m_TextureWidth;
			m_TextureHeight = other.m_TextureHeight;
			m_TexturePositionInScreen = other.m_TexturePositionInScreen;
			m_TextureScaleInScreen = other.m_TextureScaleInScreen;
			m_TextureView = other.m_TextureView;
			m_IsValid = other.m_IsValid;
			m_TextureReferencePointers.swap(other.m_TextureReferencePointers);

			other.DeregisterTextureInstance();
			other.m_Id = 0;
			other.m_IsValid = false;

			RegisterTextureInstance();
			RefreshTextureReferencePointers();
		}
		return *this;
	}

	UTexture& UTexture::operator=(const UTexture& other)
	{
		m_Id = other.m_Id;
		m_TextureWidth = other.m_TextureWidth;
		m_TextureHeight = other.m_TextureHeight;
		m_TexturePositionInScreen = other.m_TexturePositionInScreen;
		m_TextureScaleInScreen = other.m_TextureScaleInScreen;
		m_TextureView = other.m_TextureView;
		m_IsValid = other.m_IsValid;

		RegisterTextureInstance();

		return *this;
	}


	void UTexture::GetReference(UTextureRef& outReference)
	{
		if (m_IsValid)
		{
			if (outReference.m_IsValid && outReference.m_ReferencedTexture)
			{
				outReference.m_ReferencedTexture->RemoveReference(outReference);
			}
			outReference.m_Id = m_Id;
			outReference.m_IsValid = m_IsValid;
			outReference.m_ReferencedTexture = this;
			outReference.m_TextureWidth = m_TextureWidth;
			outReference.m_TextureHeight = m_TextureHeight;

			m_TextureReferencePointers.emplace(&outReference);
		}
	}

	void UTexture::RemoveReference(UTextureRef& outReference)
	{
		if (outReference.m_Id == m_Id)
		{
			outReference.m_Id = 0;
			outReference.m_IsValid = false;
			outReference.m_ReferencedTexture = nullptr;

			m_TextureReferencePointers.erase(&outReference);
		}
	}

	void UTexture::RegisterTextureInstance()
	{
		if (m_IsValid)
		{
			if (!s_TextureId_To_InstancePtrs.contains(m_Id))
			{
				s_TextureId_To_InstancePtrs.emplace(std::pair(m_Id, std::unordered_set<UTexture*>{}));
			}

			if (!s_TextureId_To_InstancePtrs.at(m_Id).contains(this))
			{
				s_TextureId_To_InstancePtrs.at(m_Id).emplace(this);
			}
			
		}
	}

	void UTexture::DeregisterTextureInstance()
	{
		if (s_TextureId_To_InstancePtrs.contains(m_Id))
		{
			if (s_TextureId_To_InstancePtrs.at(m_Id).contains(this))
			{
				s_TextureId_To_InstancePtrs.at(m_Id).erase(this);
			}
		}
	}

	void UTexture::RefreshTextureReferencePointers()
	{
		for (auto It = m_TextureReferencePointers.begin(); It !=  m_TextureReferencePointers.end(); It++)
		{
			(*It)->m_Id = m_Id;
			(*It)->m_ReferencedTexture = this;
			(*It)->m_IsValid = m_IsValid;
		}
	}

	void UTexture::CreateFromImage(const std::string& path)
	{
		m_IsValid = true;

		// Load image at specified path
		SDL_Surface* pLoadedSurface = IMG_Load(path.c_str());
		if (pLoadedSurface == nullptr)
		{
			std::cerr << "Texture::CreateFromImage, error when calling IMG_Load: " << SDL_GetError() << std::endl;
			m_IsValid = false;
			return;
		}
		CreateFromSurface(pLoadedSurface);

		// Free loaded surface
		SDL_FreeSurface(pLoadedSurface);

	}

	void UTexture::CreateFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor)
	{
		m_IsValid = true;

		// Create font
		TTF_Font* pFont{};
		pFont = TTF_OpenFont(fontPath.c_str(), ptSize);
		if (pFont == nullptr)
		{
			std::cerr << "Texture::CreateFromString, error when calling TTF_OpenFont: " << TTF_GetError() << std::endl;
			m_IsValid = false;
			return;
		}

		// Create texture using this font and close font afterwards
		CreateFromString(text, pFont, textColor);
		TTF_CloseFont(pFont);
	}

	void UTexture::CreateFromString(const std::string& text, TTF_Font* pFont, const Color4f& color)
	{
		m_IsValid = true;
		if (pFont == nullptr)
		{
			std::cerr << "Texture::CreateFromString, invalid TTF_Font pointer\n";
			m_IsValid = false;
			return;
		}

		// Render text surface
		SDL_Color textColor{};
		textColor.r = Uint8(color.r * 255);
		textColor.g = Uint8(color.g * 255);
		textColor.b = Uint8(color.b * 255);
		textColor.a = Uint8(color.a * 255);

		SDL_Surface* pLoadedSurface = TTF_RenderText_Blended(pFont, text.c_str(), textColor);
		if (pLoadedSurface == nullptr)
		{
			std::cerr << "Texture::CreateFromString, error when calling TTF_RenderText_Blended: " << TTF_GetError() << std::endl;
			m_IsValid = false;
			return;
		}

		// Copy to video memory
		CreateFromSurface(pLoadedSurface);

		// Free loaded surface
		SDL_FreeSurface(pLoadedSurface);
	}

	void UTexture::CreateFromSurface(SDL_Surface* pSurface)
	{
		m_IsValid = true;

		//Get image dimensions
		m_TextureWidth = float(pSurface->w);

		m_TextureHeight = float(pSurface->h);

		// Get pixel format information and translate to OpenGl format
		GLenum pixelFormat{ GL_RGB };
		switch (pSurface->format->BytesPerPixel)
		{
		case 3:
			if (pSurface->format->Rmask == 0x000000ff)
			{
				pixelFormat = GL_RGB;
			}
			else
			{
				pixelFormat = GL_BGR;
			}
			break;
		case 4:
			if (pSurface->format->Rmask == 0x000000ff)
			{
				pixelFormat = GL_RGBA;
			}
			else
			{
				pixelFormat = GL_BGRA;
			}
			break;
		default:
			std::cerr << "Texture::CreateFromSurface, unknow pixel format, BytesPerPixel: " << pSurface->format->BytesPerPixel << "\nUse 32 bit or 24 bit images.\n";
			m_IsValid = false;
			return;
		}

		//Generate an array of textures.  We only want one texture (one element array), so trick
		//it by treating "texture" as array of length one.
		glGenTextures(1, &m_Id);

		//Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
		//All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
		glBindTexture(GL_TEXTURE_2D, m_Id);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, pSurface->pitch / pSurface->format->BytesPerPixel);
		// check for errors. Can happen if a texture is created while a static pointer is being initialized, even before the call to the main function.
		GLenum e = glGetError();
		if (e != GL_NO_ERROR)
		{
			std::cerr << "Texture::CreateFromSurface, error binding textures, Error id = " << e << '\n';
			std::cerr << "Can happen if a texture is created before performing the initialization code (e.g. a static Texture object).\n";
			std::cerr << "There might be a white rectangle instead of the image.\n";
		}

		// Specify the texture's data.  
		// This function is a bit tricky, and it's hard to find helpful documentation. 
		// A summary:
		//    GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
		//                0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
		//                         not cached smaller copies)
		//          GL_RGBA:    Specifies the number of color components in the texture.
		//                     This is how OpenGL will store the texture internally (kinda)--
		//                     It's essentially the texture's type.
		//       surface->w:    The width of the texture
		//       surface->h:    The height of the texture
		//                0:    The border.  Don't worry about this if you're just starting.
		//      pixelFormat:    The format that the *data* is in--NOT the texture! 
		// GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
		//                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
		//                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
		//                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
		//  surface->pixels:    The actual data.  As above, SDL's array of bytes.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);

		// Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
		// *smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
		// each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
		// further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
		// them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

#pragma endregion

#pragma region TextureReference

	UTextureRef::UTextureRef(UTexture& texture, const Rectf& textureView, const Point2f& positionInScreen, const Point2f& scaleInScreen)
	{
		texture.GetReference(*this);
		SetTextureView(textureView);
		SetScreenPosition(positionInScreen);
		SetScale(scaleInScreen);
	}

	UTextureRef::UTextureRef(UTexture& texture)
	{
		texture.GetReference(*this);
		SetTextureView(texture.GetTextureView());
		SetScreenPosition(texture.GetPositionInScreen());
		SetScale(texture.GetScaleInScreen());
	}

	UTextureRef::UTextureRef(const UTextureRef& other)
	{
		if (other.m_ReferencedTexture)
		{
			m_TexturePositionInScreen = other.m_TexturePositionInScreen;
			m_TextureScaleInScreen = other.m_TextureScaleInScreen;
			m_TextureView = other.m_TextureView;

			other.m_ReferencedTexture->GetReference(*this);
		}
	}

	UTextureRef::UTextureRef(UTextureRef&& other) noexcept
	{
		if (other.m_ReferencedTexture)
		{
			m_TexturePositionInScreen = other.m_TexturePositionInScreen;
			m_TextureScaleInScreen = other.m_TextureScaleInScreen;
			m_TextureView = other.m_TextureView;

			other.m_ReferencedTexture->GetReference(*this);
			other.m_ReferencedTexture->RemoveReference(other);
			other.m_ReferencedTexture = nullptr;
		}
	}

	UTextureRef::~UTextureRef()
	{
		if (m_ReferencedTexture)
		{
			m_ReferencedTexture->RemoveReference(*this);
			m_ReferencedTexture = nullptr;
		}
		
	}

	UTextureRef& UTextureRef::operator=(const UTextureRef& other)
	{
		if (other.m_ReferencedTexture)
		{
			m_TexturePositionInScreen = other.m_TexturePositionInScreen;
			m_TextureScaleInScreen = other.m_TextureScaleInScreen;
			m_TextureView = other.m_TextureView;
			other.m_ReferencedTexture->GetReference(*this);
		}

		return *this;
	}

	UTextureRef& UTextureRef::operator=(UTextureRef&& other)noexcept
	{
		if (other.m_ReferencedTexture)
		{
			m_TexturePositionInScreen = other.m_TexturePositionInScreen;
			m_TextureScaleInScreen = other.m_TextureScaleInScreen;
			m_TextureView = other.m_TextureView;
			other.m_ReferencedTexture->GetReference(*this);
			other.m_ReferencedTexture->RemoveReference(other);
			other.m_ReferencedTexture = nullptr;
		}

		return *this;
	}
#pragma endregion
	USpriteSheet::USpriteSheet(const UTexture& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixeBetweenColumns, unsigned int numberOfEmptyPixeBetweenRows,  const Point2f& scaleInScreen) :
		m_Texture(texture),
		m_NumberOfRows(numberOfRows),
		m_NumberOfColumns(numberOfColumns),
		m_NumberOfEmptyPixelsBetweenColumns(numberOfEmptyPixeBetweenColumns),
		m_NumberOfEmptyPixelsBetweenRows(numberOfEmptyPixeBetweenRows)
	{
		m_Texture.SetScreenPosition(Point2f{ -m_Texture.GetTextureOriginalWidth() / 2.f, -m_Texture.GetTextureOriginalHeight() / 2.f });
		m_Texture.SetScale(scaleInScreen);
		m_CellWidth = m_Texture.GetTextureOriginalWidth() / m_NumberOfColumns;
		m_CellHeight = m_Texture.GetTextureOriginalHeight() / m_NumberOfRows ;
	}
	UTextureRef USpriteSheet::GetFrameCopy(size_t rowIndex, size_t columnIndex)
	{
		FrameTexture(rowIndex, columnIndex);
		return UTextureRef(m_Texture);
	}
	
	const UTexture& USpriteSheet::GetFrame(size_t rowIndex, size_t columnIndex)
	{
		FrameTexture(rowIndex, columnIndex);
		return m_Texture;
	}
	
	void USpriteSheet::SetPositionInScreen(const Point2f& position)
	{
		m_Texture.SetScreenPosition(position);
	}
	
	void USpriteSheet::SetScaleInScreen(const Point2f& scaleInScreen)
	{
		m_Texture.SetScale(scaleInScreen);
	}

	const Point2f& USpriteSheet::GetPositionInScreen()const
	{
		return m_Texture.GetPositionInScreen();
	}

	const Point2f& USpriteSheet::GetScaleInScreen() const
	{
		return m_Texture.GetScaleInScreen();
	}

	int USpriteSheet::GetOriginalCellWidth() const
	{
		return m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns;
	}

	int USpriteSheet::GetOriginalCellHeight() const
	{
		return m_CellHeight - m_NumberOfEmptyPixelsBetweenRows;
	}

	float USpriteSheet::GetScaledCellWidth() const
	{
		return (m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns) * m_Texture.GetScaleInScreen().x;
	}

	float USpriteSheet::GetScaledCellHeight() const
	{
		return (m_CellHeight - m_NumberOfEmptyPixelsBetweenRows) * m_Texture.GetScaleInScreen().y;
	}
	
	bool USpriteSheet::IsValid() const
	{
		return m_Texture.IsValid();
	}

	void USpriteSheet::FrameTexture(size_t rowIndex, size_t columnIndex)
	{
		++rowIndex;//because without the increment it would be top instead of bottom
		m_Texture.SetTextureView(Rectf{
			(columnIndex * m_CellWidth) + (m_NumberOfEmptyPixelsBetweenColumns / 2.f) ,
			(rowIndex * m_CellHeight) + (m_NumberOfEmptyPixelsBetweenRows / 2.f),
			float(m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns),
			float(m_CellHeight - m_NumberOfEmptyPixelsBetweenRows) });
	}
	
	UTextureData::UTextureData(const UBaseTexture& texture) :
		m_TextureWidth(texture.m_TextureWidth),
		m_TextureHeight(texture.m_TextureHeight)
	{
		if (texture.IsValid())
		{
			m_IsValid = true;
			// Bind the texture
			glBindTexture(GL_TEXTURE_2D, texture.m_Id);

			// Allocate memory to store the pixel data
			m_Pixels = new GLubyte[m_TextureWidth * m_TextureHeight * m_RGBA_Num]; // Assuming RGBA format

			// Retrieve the pixel data from the texture
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixels);
		}

	}
	UTextureData::~UTextureData()
	{
		delete[] m_Pixels;
	}
	UTextureData::UTextureData(const UTextureData& other) :
		m_TextureWidth(other.m_TextureWidth),
		m_TextureHeight(other.m_TextureHeight),
		m_IsValid(other.m_IsValid)
	{
		size_t totalArraySize{ other.m_TextureWidth * other.m_TextureHeight * m_RGBA_Num };// Assuming RGBA format

		if (totalArraySize > 0)
		{
			m_Pixels = new GLubyte[totalArraySize]; 

			for (size_t i = 0; i < totalArraySize; i++)
			{
				m_Pixels[i] = other.m_Pixels[i];
			}
		}

	}
	UTextureData::UTextureData(UTextureData&& other) noexcept :
		m_Pixels(other.m_Pixels),
		m_TextureWidth(other.m_TextureWidth),
		m_TextureHeight(other.m_TextureHeight),
		m_IsValid(other.m_IsValid)
	{
		other.m_Pixels = nullptr;
	}
	UTextureData& UTextureData::operator=(const UTextureData& other)
	{

		size_t totalArraySize{ other.m_TextureWidth * other.m_TextureHeight * m_RGBA_Num };// Assuming RGBA format

		if ((this != &other) && totalArraySize > 0)
		{
			delete[] m_Pixels;
			m_Pixels = new GLubyte[totalArraySize];

			for (size_t i = 0; i < totalArraySize; i++)
			{
				m_Pixels[i] = other.m_Pixels[i];
			}
			m_TextureWidth = other.m_TextureWidth;
			m_TextureHeight = other.m_TextureHeight;
			m_IsValid = other.m_IsValid;
		}

		return *this;
	}
	UTextureData& UTextureData::operator=(UTextureData&& other) noexcept
	{
		if ((this != &other) && 
			other.m_TextureWidth > 0 && 
			other.m_TextureHeight > 0) 
		{

			delete[] m_Pixels;
			m_Pixels = other.m_Pixels;
			m_TextureWidth = other.m_TextureWidth;
			m_TextureHeight = other.m_TextureHeight;
			m_IsValid = other.m_IsValid;

			other.m_Pixels = nullptr;
		}

		return *this;
	}
	size_t UTextureData::NumberOfRows() const
	{
		return m_TextureHeight;
	}
	size_t UTextureData::NumberOfColumns() const
	{
		return m_TextureWidth;
	}
	size_t UTextureData::NumberOfPixels() const
	{
		return m_TextureHeight * m_TextureWidth;
	}
	FColor4i UTextureData::PixelAt(size_t column, size_t row)const
	{
		FColor4i color{};
		color.SetR(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num)]	 );
		color.SetG(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num) + 1]);
		color.SetB(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num) + 2]);
		color.SetA(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num) + 3]);

		return color;
	}
	FColor4i UTextureData::PixelAt(size_t pixelIndex)const
	{
		FColor4i color{};
		color.SetR(m_Pixels[(pixelIndex * m_RGBA_Num)]	  );
		color.SetG(m_Pixels[(pixelIndex * m_RGBA_Num) + 1]);
		color.SetB(m_Pixels[(pixelIndex * m_RGBA_Num) + 2]);
		color.SetA(m_Pixels[(pixelIndex * m_RGBA_Num) + 3]);

		return color;
	}
	bool UTextureData::PixelIsInBounds(int x, int y) const
	{
		return x > 0 && x < m_TextureWidth && y > 0 && y < m_TextureHeight;
	}
	std::vector<Rectf> UTextureData::MakeRectsFromPixelsOfColor(const FColor4i& color, float xSize, float ySize)const
	{
		std::vector<Rectf> rects{};
		rects.reserve(NumberOfPixels()/4);//aprox

		Rectf rect{};
		rect.height = ySize;
		rect.width = xSize;

		for (size_t rowIdx = 0; rowIdx < NumberOfRows(); rowIdx++)
		{
			for (size_t colIdx = 0; colIdx < NumberOfColumns(); colIdx++)
			{
				if (PixelAt(colIdx, rowIdx) == color)
				{
					rect.bottom = rowIdx * ySize;
					rect.left = colIdx * xSize;
					
					rects.push_back(rect);
				}
			}
		}

		return rects;
	}
	bool UTextureData::IsValid() const
	{
		return m_IsValid;
	}
	const FColor4i& UTextureMapRuleBook::GetCenterColor() const
	{
		return m_Color;
	}
	void UTextureMapRuleBook::SetCenterColor(const FColor4i& color)
	{
		m_Color = color;
	}
	void UTextureMapRuleBook::AddColorRule(const FColor4i& color, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals)
	{
		FTextureMapRule newRule{};
		newRule.Color = color;
		newRule.Check_R = true;
		newRule.Check_G = true;
		newRule.Check_B = true;
		newRule.Check_A = true;
		newRule.RelativeX = xCoordinateFromCenter;
		newRule.RelativeY = yCoordinateFromCenter;
		newRule.Equals = equals;

		m_Rules.push_back(newRule);
	}
	void UTextureMapRuleBook::AddRedRule(size_t value, int xCoordinateFromCenter, int yCoordinateFromCenter,  bool equals)
	{
		FTextureMapRule newRule{};
		newRule.Color.SetR(value);
		newRule.Check_R = true;
		newRule.Check_G = false;
		newRule.Check_B = false;
		newRule.Check_A = false;
		newRule.RelativeX = xCoordinateFromCenter;
		newRule.RelativeY = yCoordinateFromCenter;
		newRule.Equals = equals;

		m_Rules.push_back(newRule);
	}
	void UTextureMapRuleBook::AddGreenRule(size_t value, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals)
	{
		FTextureMapRule newRule{};
		newRule.Color.SetG(value);
		newRule.Check_R = false;
		newRule.Check_G = true;
		newRule.Check_B = false;
		newRule.Check_A = false;
		newRule.RelativeX = xCoordinateFromCenter;
		newRule.RelativeY = yCoordinateFromCenter;
		newRule.Equals = equals;

		m_Rules.push_back(newRule);
	}
	void UTextureMapRuleBook::AddBlueRule(size_t value, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals)
	{
		FTextureMapRule newRule{};
		newRule.Color.SetB(value);
		newRule.Check_R = false;
		newRule.Check_G = false;
		newRule.Check_B = true;
		newRule.Check_A = false;
		newRule.RelativeX = xCoordinateFromCenter;
		newRule.RelativeY = yCoordinateFromCenter;
		newRule.Equals = equals;

		m_Rules.push_back(newRule);
	}
	void UTextureMapRuleBook::AddAlphaRule(size_t value, int xCoordinateFromCenter, int yCoordinateFromCenter, bool equals)
	{
		FTextureMapRule newRule{};
		newRule.Color.SetA(value);
		newRule.Check_R = false;
		newRule.Check_G = false;
		newRule.Check_B = false;
		newRule.Check_A = true;
		newRule.RelativeX = xCoordinateFromCenter;
		newRule.RelativeY = yCoordinateFromCenter;
		newRule.Equals = equals;

		m_Rules.push_back(newRule);
	}
	void UTextureMapRuleBook::AddRule(const FTextureMapRule& rule)
	{
		FTextureMapRule& ruleToChange{ const_cast<FTextureMapRule&>(rule) };
		m_Rules.push_back(ruleToChange);
	}
	size_t UTextureMapRuleBook::NumberOfRules() const
	{
		return m_Rules.size();
	}
	void UTextureMapRuleBook::ClearRules()
	{
		m_Rules.clear();
	}
	bool UTextureMapRuleBook::RuleBookIsFulfilled(int columnIndex, int rowIndex, const UTextureData& textureData) const
	{
		if (textureData.PixelIsInBounds(columnIndex, rowIndex))
		{
			FColor4i currentColor{ textureData.PixelAt(columnIndex, rowIndex) };
			int currentX{};
			int currentY{};

			if (currentColor == m_Color)
			{
				for (size_t i = 0; i < m_Rules.size(); i++)
				{
					currentX  = columnIndex + m_Rules.at(i).RelativeX;
					currentY  = rowIndex - m_Rules.at(i).RelativeY;//here you flip the y axis

					if (textureData.PixelIsInBounds(currentX, currentY))
					{
						currentColor = textureData.PixelAt(currentX, currentY);

						if (m_Rules[i].Equals)
						{
							if (m_Rules[i].Check_R)
							{
								if (m_Rules[i].Color.GetR() != currentColor.GetR())
								{
									return false;
								}
							}
							if (m_Rules[i].Check_G)
							{
								if (m_Rules[i].Color.GetG() != currentColor.GetG())
								{
									return false;
								}
							}
							if (m_Rules[i].Check_B)
							{
								if (m_Rules[i].Color.GetB() != currentColor.GetB())
								{
									return false;
								}
							}
							if (m_Rules[i].Check_A)
							{
								if (m_Rules[i].Color.GetA() != currentColor.GetA())
								{
									return false;
								}
							}
						}
						else
						{
							bool isTheSame{ true };

							if (m_Rules[i].Check_R)
							{
								if (m_Rules[i].Color.GetR() != currentColor.GetR())
								{
									isTheSame = false;
								}
							}
							if (m_Rules[i].Check_G)
							{
								if (m_Rules[i].Color.GetG() != currentColor.GetG())
								{
									isTheSame = false;
								}
							}
							if (m_Rules[i].Check_B)
							{
								if (m_Rules[i].Color.GetB() != currentColor.GetB())
								{
									isTheSame = false;
								}
							}
							if (m_Rules[i].Check_A)
							{
								if (m_Rules[i].Color.GetA() != currentColor.GetA())
								{
									isTheSame = false;
								}
							}

							if (isTheSame)
							{
								return false;
							}
						}
						
					}
				}

				return true;
			}
		}

		
		return false;
	}
	UTextureMap::UTextureMap(const USpriteSheet& spriteSheet) :
		m_SpriteSheet(spriteSheet)
	{
	}
	void UTextureMap::SetScale(const Point2f& scale)
	{
		m_SpriteSheet.SetScaleInScreen(scale);
	}
	const Point2f& UTextureMap::GetScale() const
	{
		return m_SpriteSheet.GetScaleInScreen();
	}
	const USpriteSheet& UTextureMap::GetSpriteSheet() const
	{
		return m_SpriteSheet;
	}
	const UTextureData& UTextureMap::GetTextureData() const
	{
		return m_TextureData;
	}
	void UTextureMap::SetRulesForTileBlock(
		int topRowIndex, int leftColumnIndex, int bottomRowIndex, int rightColumnIndex, const FColor4i& color,
		bool takeIntoAccountBorders, int topBorderOffset , int bottomBorderOffset, int leftBorderOffset , int rightpBorderOffset)
	{
		//row increases from up to bottom
		for (int rowIdx = bottomRowIndex; rowIdx >= topRowIndex; rowIdx--)
		{
			for (int colIdx = leftColumnIndex; colIdx <= rightColumnIndex; colIdx++)
			{
				SetRuleForTileInTileBlock(rowIdx, colIdx, topRowIndex, leftColumnIndex, bottomRowIndex, rightColumnIndex, color,
					takeIntoAccountBorders, topBorderOffset, bottomBorderOffset, leftBorderOffset, rightpBorderOffset);
			}
		}
	}
	void UTextureMap::SetRuleForTile(size_t rowIndexForFrameInSpriteSheet, size_t columnIndexForFrameInSpriteSheet, const UTextureMapRuleBook& textureMapRule)
	{
		m_HaveToCalculateMapData = true;

		if (!m_Color_To_TileGroup.Contains(textureMapRule.GetCenterColor()))
		{
			m_Color_To_TileGroup.Push(textureMapRule.GetCenterColor(), FTileGroupInfo{});
		}

		m_Color_To_TileGroup.ValueAt(textureMapRule.GetCenterColor()).Sorted = false;
		m_Color_To_TileGroup.ValueAt(textureMapRule.GetCenterColor()).TileTypeInfo.push_back(FTileTypeInfo{ textureMapRule, rowIndexForFrameInSpriteSheet , columnIndexForFrameInSpriteSheet });
	}
	void UTextureMap::SetBorderTile(size_t rowIndexForFrameInSpriteSheet, size_t columnIndexForFrameInSpriteSheet, const FColor4i& color, bool right, bool left, bool top, bool bottom, const std::vector<FColor4i>& colorsToOmit)
	{
		UTextureMapRuleBook rule{};

		rule.SetCenterColor(color);

		std::vector<FColor4i> localColorsToOmit{ colorsToOmit };

		localColorsToOmit.push_back(color);

		const int bottomDirection{-1};
		const int topDirection{1};
		const int leftDirection{-1};
		const int rightDirection{1};

		for (size_t i = 0; i < localColorsToOmit.size(); i++)
		{
			if (right && bottom)
			{
				rule.AddColorRule(localColorsToOmit.at(i), rightDirection, bottomDirection, false);
			}
			if (right && top)
			{
				rule.AddColorRule(localColorsToOmit.at(i), rightDirection, topDirection, false);
			}
			if (left && bottom)
			{
				rule.AddColorRule(localColorsToOmit.at(i), leftDirection, bottomDirection, false);
			}
			if (left && top)
			{
				rule.AddColorRule(localColorsToOmit.at(i), leftDirection, topDirection, false);
			}
			if (right)
			{
				rule.AddColorRule(localColorsToOmit.at(i), rightDirection, 0, false);
			}
			if (left)
			{
				rule.AddColorRule(localColorsToOmit.at(i), leftDirection, 0, false);
			}
			if (top)
			{
				rule.AddColorRule(localColorsToOmit.at(i), 0, topDirection, false);
			}
			if (bottom)
			{
				rule.AddColorRule(localColorsToOmit.at(i), 0, bottomDirection, false);
			}
		}
		

		SetRuleForTile(rowIndexForFrameInSpriteSheet, columnIndexForFrameInSpriteSheet, rule);
	}
	void UTextureMap::SetDiagonalBorderTile(size_t rowIndexForFrameInSpriteSheet, size_t columnIndexForFrameInSpriteSheet, const FColor4i& color, bool right, bool left, bool top, bool bottom, const std::vector<FColor4i>& colorsToOmit)
	{
		UTextureMapRuleBook rule_One{};
		UTextureMapRuleBook rule_Two{};

		rule_One.SetCenterColor(color);
		rule_Two.SetCenterColor(color);

		std::vector<FColor4i> localColorsToOmit{ colorsToOmit };
		localColorsToOmit.push_back(color);

		int bottomDirection{ -1 };
		int topDirection{ 1 };
		int leftDirection{ -1 };
		int rightDirection{ 1 };
		bool setRule{ false };

		for (size_t i = 0; i < localColorsToOmit.size(); i++)
		{
			if (right && bottom)
			{
				setRule = true;
				rule_One.AddColorRule(localColorsToOmit.at(i), rightDirection, bottomDirection, false);
				rule_One.AddColorRule(color, leftDirection, bottomDirection, true);

				rule_Two.AddColorRule(localColorsToOmit.at(i), rightDirection, bottomDirection, false);
				rule_Two.AddColorRule(color, rightDirection, topDirection, true);

			}
			if (right && top)
			{
				setRule = true;

				rule_One.AddColorRule(localColorsToOmit.at(i), rightDirection, topDirection, false);
				rule_One.AddColorRule(color, leftDirection, topDirection, true);

				rule_Two.AddColorRule(localColorsToOmit.at(i), rightDirection, topDirection, false);
				rule_Two.AddColorRule(color, rightDirection, bottomDirection, true);
			}
			if (left && bottom)
			{
				setRule = true;

				rule_One.AddColorRule(localColorsToOmit.at(i), leftDirection, bottomDirection, false);
				rule_One.AddColorRule(color, rightDirection, bottomDirection, true);

				rule_Two.AddColorRule(localColorsToOmit.at(i), leftDirection, bottomDirection, false);
				rule_Two.AddColorRule(color, leftDirection, topDirection, true);
			}
			if (left && top)
			{
				setRule = true;

				rule_One.AddColorRule(localColorsToOmit.at(i), leftDirection, topDirection, false);
				rule_One.AddColorRule(color, rightDirection, topDirection, true);

				rule_Two.AddColorRule(localColorsToOmit.at(i), leftDirection, topDirection, false);
				rule_Two.AddColorRule(color, leftDirection, bottomDirection, true);
			}
			if (right)
			{
				rule_One.AddColorRule(localColorsToOmit.at(i), rightDirection, 0, false);
				rule_Two.AddColorRule(localColorsToOmit.at(i), rightDirection, 0, false);
			}
			if (left)
			{
				rule_One.AddColorRule(localColorsToOmit.at(i), leftDirection, 0, false);
				rule_Two.AddColorRule(localColorsToOmit.at(i), leftDirection, 0, false);
			}
			if (top)
			{
				rule_One.AddColorRule(localColorsToOmit.at(i), 0, topDirection, false);
				rule_Two.AddColorRule(localColorsToOmit.at(i), 0, topDirection, false);
			}
			if (bottom)
			{
				rule_One.AddColorRule(localColorsToOmit.at(i), 0, bottomDirection, false);
				rule_Two.AddColorRule(localColorsToOmit.at(i), 0, bottomDirection, false);
			}
		}
		
		if (!setRule)
		{
			return;
		}

		SetRuleForTile(rowIndexForFrameInSpriteSheet, columnIndexForFrameInSpriteSheet, rule_One);
		SetRuleForTile(rowIndexForFrameInSpriteSheet, columnIndexForFrameInSpriteSheet, rule_Two);
	}
	void UTextureMap::SetMap(const UTextureData& textureData)
	{
		Implementation_SetMap(textureData);
	}
	std::vector<Rectf> UTextureMap::GetDrawnMapBoundsData() const
	{
		std::vector<Rectf> bounds{};
		bounds.reserve(m_MapData.size());

		Point2f currentPosition{};

		for (size_t i = 0; i < m_MapData.size(); i++)
		{
			currentPosition.x = m_SpriteSheet.GetScaledCellWidth() * m_MapData[i].LevelColumnIndex;
			currentPosition.y = m_SpriteSheet.GetScaledCellHeight() * m_MapData[i].LevelRowIndex;

			bounds.push_back(Rectf(currentPosition.x, currentPosition.y, m_SpriteSheet.GetScaledCellWidth(), m_SpriteSheet.GetScaledCellHeight()));
		}

		return bounds;
	}
	std::vector<Rectf> UTextureMap::GetDrawnMapBoundsData(const FColor4i& color) const
	{
		std::vector<Rectf> bounds{};
		bounds.reserve(m_MapData.size());

		Point2f currentPosition{};

		for (size_t i = 0; i < m_MapData.size(); i++)
		{
			if (m_MapColorData.at(i) == color)
			{
				currentPosition.x = m_SpriteSheet.GetScaledCellWidth() * m_MapData[i].LevelColumnIndex;
				currentPosition.y = m_SpriteSheet.GetScaledCellHeight() * m_MapData[i].LevelRowIndex;

				bounds.push_back(Rectf(currentPosition.x, currentPosition.y, m_SpriteSheet.GetScaledCellWidth(), m_SpriteSheet.GetScaledCellHeight()));

			}
		}

		return bounds;
	}
	std::vector<Rectf> UTextureMap::GetRawMapBoundsData(const FColor4i& color) const
	{
		std::vector<Rectf> bounds{};
		bounds.reserve(m_MapData.size());

		Rectf currentRect{};
		currentRect.height = m_SpriteSheet.GetScaledCellHeight();
		currentRect.width = m_SpriteSheet.GetScaledCellWidth();

		for (int rowIdx = 0; rowIdx < m_TextureData.NumberOfRows(); rowIdx++)
		{
			for (int colIdx = 0; colIdx < m_TextureData.NumberOfColumns(); colIdx++)
			{
				if (m_TextureData.PixelAt(colIdx, rowIdx) == color)
				{
					currentRect.bottom = std::ceil(m_SpriteSheet.GetScaledCellHeight() * (-rowIdx));
					currentRect.left = std::ceil(m_SpriteSheet.GetScaledCellWidth() * colIdx);
					bounds.push_back(currentRect);
				}
			}
		}

		return bounds;
	}
	const std::vector<Rectf> UTextureMap::ExtractRectsOfColor(const FColor4i& color) const
	{
		std::vector<Rectf> bounds{};
		bounds.reserve(m_MapData.size());

		UTextureMapRuleBook topLeft_EdgeRule{};
		topLeft_EdgeRule.SetCenterColor(color);
		topLeft_EdgeRule.AddColorRule(color, 1, 0,	true);
		topLeft_EdgeRule.AddColorRule(color, 0, -1, true);

		//topLeft_EdgeRule.AddColorRule(color,-1,	0,	false);
		//topLeft_EdgeRule.AddColorRule(color, 0,	1,  false);
		//topLeft_EdgeRule.AddColorRule(color, -1, 1, false);
		
		UTextureMapRuleBook topRight_EdgeRule{};
		topRight_EdgeRule.SetCenterColor(color);
		topRight_EdgeRule.AddColorRule(color, -1, 0, true);
		topRight_EdgeRule.AddColorRule(color, 0, -1, true);

		//topRight_EdgeRule.AddColorRule(color, 1, 0, false);
		//topRight_EdgeRule.AddColorRule(color, 0, 1, false);
		//topRight_EdgeRule.AddColorRule(color, 1, 1, false);

		//UTextureMapRuleBook bottomLeft_EdgeRule{};
		//bottomLeft_EdgeRule.SetCenterColor(color);
		//bottomLeft_EdgeRule.AddColorRule(color, 1, 0, true);
		//bottomLeft_EdgeRule.AddColorRule(color, 0, 1, true);
		//
		//bottomLeft_EdgeRule.AddColorRule(color, -1, 0, false);
		//bottomLeft_EdgeRule.AddColorRule(color, 0, -1, false);
		//bottomLeft_EdgeRule.AddColorRule(color, -1, -1, false);

		UTextureMapRuleBook bottomRight_EdgeRule{};
		bottomRight_EdgeRule.SetCenterColor(color);
		bottomRight_EdgeRule.AddColorRule(color, -1, 0, true);
		bottomRight_EdgeRule.AddColorRule(color, 0, 1, true);

		//bottomRight_EdgeRule.AddColorRule(color, 1, 0, false);
		//bottomRight_EdgeRule.AddColorRule(color, 0, -1, false);
		//bottomRight_EdgeRule.AddColorRule(color, 1, -1, false);

		Point2f currentPosition{};

		for (int rowIdx = 0; rowIdx < m_TextureData.NumberOfRows(); rowIdx++)
		{
			for (int colIdx = 0; colIdx < m_TextureData.NumberOfColumns(); colIdx++)
			{
				if (topLeft_EdgeRule.RuleBookIsFulfilled(colIdx, rowIdx, m_TextureData))
				{
					Rectf currentRect{};
					currentRect.left = 
						(m_SpriteSheet.GetScaledCellWidth() *
						colIdx);

					int currentColumn{ colIdx + 1 };
					int currentRow{ rowIdx};
					bool finishedCheckingHorizontal{false};
					bool finishedCheckingVertical{false};

					while (!finishedCheckingHorizontal || !finishedCheckingVertical)
					{
						if (!m_TextureData.PixelIsInBounds(currentColumn, currentRow))
						{
							break;
						}

						//Check Horizontal
						if (topRight_EdgeRule.RuleBookIsFulfilled(currentColumn, currentRow, m_TextureData))
						{
							currentRect.width = 
								(m_SpriteSheet.GetScaledCellWidth() *
								((currentColumn + 1) - colIdx));
							finishedCheckingHorizontal = true;
						}
						else if(!finishedCheckingHorizontal)
						{
							currentColumn++;
						}

						//CheckVertical

						if (finishedCheckingHorizontal)
						{
							if (bottomRight_EdgeRule.RuleBookIsFulfilled(currentColumn, currentRow, m_TextureData))
							{
								currentRect.bottom =
									(m_SpriteSheet.GetScaledCellHeight() *
									(-(currentRow)));

								currentRect.height =
									(m_SpriteSheet.GetScaledCellHeight() *
									(currentRow - rowIdx + 1));

								bounds.push_back(currentRect);
								finishedCheckingVertical = true;

								colIdx = currentColumn;
							}
							else
							{
								currentRow++;
							}
						}
						
					}
				}
			}
		}
		

		for (size_t i = 0; i < m_MapData.size(); i++)
		{
			if (m_MapColorData.at(i) == color)
			{
				currentPosition.x = m_SpriteSheet.GetScaledCellWidth() * m_MapData[i].LevelColumnIndex;
				currentPosition.y = m_SpriteSheet.GetScaledCellHeight() * m_MapData[i].LevelRowIndex;

				bounds.push_back(Rectf(currentPosition.x, currentPosition.y, m_SpriteSheet.GetScaledCellWidth(), m_SpriteSheet.GetScaledCellHeight()));

			}
		}

		return bounds;
	}
	void UTextureMap::Draw(const Rectf& cameraWorldRect) const
	{
		Point2f currentPosition{};

		if (m_HaveToCalculateMapData)
		{
			Implementation_SetMap(m_TextureData);
		}

		for (size_t i = 0; i < m_MapData.size(); i++)
		{
			currentPosition.x = m_SpriteSheet.GetScaledCellWidth() * m_MapData[i].LevelColumnIndex;
			currentPosition.y = m_SpriteSheet.GetScaledCellHeight() * m_MapData[i].LevelRowIndex;
			
			if (utils::IsPointInRect(currentPosition, cameraWorldRect))
			{
				m_SpriteSheet.SetPositionInScreen(currentPosition);
				m_SpriteSheet.GetFrame(m_MapData[i].SpriteSheetRowIndex, m_MapData[i].SpriteSheetColumnIndex).Draw();
			}
			
		}
	}
	void UTextureMap::SetRuleForTileInTileBlock(
		int currentRowIndex, int currentColumnIndex, int topRowIndex, int leftColumnIndex, int bottomRowIndex, int rightColumnIndex,
		const FColor4i& color,
		bool takeIntoAccountBorders, int topBorderOffset, int bottomBorderOffset, int leftBorderOffset, int rightBorderOffset)
	{
		//here there is still a normal x and y plane for the rules
		UTextureMapRuleBook currentRuleBook{};
		currentRuleBook.SetCenterColor(color);
		int rowOffset{};
		int colOffset{};

		//Set Color Rules
		for (int rowIdx = bottomRowIndex; rowIdx >= topRowIndex; rowIdx--)
		{
			for (int colIdx = leftColumnIndex; colIdx <= rightColumnIndex; colIdx++)
			{
				if (rowIdx != currentRowIndex || colIdx != currentColumnIndex)
				{
					rowOffset = rowIdx - currentRowIndex;
					rowOffset = -rowOffset;//because the y positive in a spritesheet goes downwards
					colOffset = colIdx - currentColumnIndex;

					currentRuleBook.AddColorRule(color, colOffset, rowOffset, true);

				}
			}
		}

		bool isTopBorder{ currentRowIndex <= topRowIndex + topBorderOffset };
		bool isBottomBorder{ currentRowIndex >= bottomRowIndex - bottomBorderOffset };
		bool isLeftBorder{ currentColumnIndex <= leftColumnIndex + leftBorderOffset };
		bool isRightBorder{ currentColumnIndex >= rightColumnIndex - rightBorderOffset };

		if (takeIntoAccountBorders && (isTopBorder || isBottomBorder || isLeftBorder || isRightBorder))
		{

			int currentTopBorderOffset{ -((topRowIndex - 1) - currentRowIndex)};
			int currentBottomBorderOffset{ -((bottomRowIndex + 1) - currentRowIndex) };
			int currentLeftBorderOffset{ (leftColumnIndex - 1) - currentColumnIndex };
			int currentRightBorderOffset{ (rightColumnIndex  + 1) - currentColumnIndex };

			/*rowOffset =
				currentRowIndex == bottomRowIndex ? -1 :
				currentRowIndex == topRowIndex ? 1 : 0;

			colOffset =
				currentColumnIndex == leftColumnIndex ? -1 :
				currentColumnIndex == rightColumnIndex ? 1 :
				0;*/

			if (isTopBorder && isLeftBorder)
			{
				currentRuleBook.AddColorRule(color, currentLeftBorderOffset, currentTopBorderOffset, false);
			}
			if (isTopBorder && isRightBorder)
			{
				currentRuleBook.AddColorRule(color, currentRightBorderOffset, currentTopBorderOffset, false);
			}
			if (isBottomBorder && isLeftBorder)
			{
				currentRuleBook.AddColorRule(color, currentLeftBorderOffset, currentBottomBorderOffset, false);
			}
			if (isBottomBorder && isRightBorder)
			{
				currentRuleBook.AddColorRule(color, currentRightBorderOffset, currentBottomBorderOffset, false);
			}

			if (isLeftBorder)
			{
				currentRuleBook.AddColorRule(color, currentLeftBorderOffset, 0, false);
			}
			if (isRightBorder)
			{
				currentRuleBook.AddColorRule(color, currentRightBorderOffset, 0, false);
			}
			if (isTopBorder)
			{
				currentRuleBook.AddColorRule(color, 0, currentTopBorderOffset, false);
			}
			if (isBottomBorder)
			{
				currentRuleBook.AddColorRule(color, 0, currentBottomBorderOffset, false);
			}
		}
		

		SetRuleForTile(currentRowIndex, currentColumnIndex, currentRuleBook);
	}
	void UTextureMap::Implementation_SetMap(const UTextureData& textureData) const
	{
		if (!textureData.IsValid())
		{
			return;
		}

		m_HaveToCalculateMapData = false;
		m_TextureData = textureData;

		FColor4i currentColor{};

		m_MapData.clear();
		m_MapData.reserve(textureData.NumberOfPixels());
		m_MapColorData.clear();
		m_MapColorData.reserve(textureData.NumberOfPixels());

		for (int rowIdx = 0; rowIdx < textureData.NumberOfRows(); rowIdx++)
		{
			for (int colIdx = 0; colIdx < textureData.NumberOfColumns(); colIdx++)
			{
				currentColor = textureData.PixelAt(colIdx, rowIdx);

				if (m_Color_To_TileGroup.Contains(currentColor))
				{
					if (!m_Color_To_TileGroup.ValueAt(currentColor).Sorted)
					{
						std::sort(
							m_Color_To_TileGroup.ValueAt(currentColor).TileTypeInfo.begin(),
							m_Color_To_TileGroup.ValueAt(currentColor).TileTypeInfo.end());

						m_Color_To_TileGroup.ValueAt(currentColor).Sorted = true;
					}

					for (int tileInfoIdx = m_Color_To_TileGroup.ValueAt(currentColor).TileTypeInfo.size() - 1; tileInfoIdx >= 0; tileInfoIdx--)
					{
						if (m_Color_To_TileGroup.ValueAt(currentColor).TileTypeInfo[tileInfoIdx].Rules.RuleBookIsFulfilled(colIdx, rowIdx, textureData))
						{

							m_MapColorData.push_back(currentColor);

							m_MapData.push_back(FTileInfo{
								m_Color_To_TileGroup.ValueAt(currentColor).TileTypeInfo[tileInfoIdx].SpriteSheetColumnIndex,
								m_Color_To_TileGroup.ValueAt(currentColor).TileTypeInfo[tileInfoIdx].SpriteSheetRowIndex,
								colIdx,
								rowIdx });

							break;
						}
					}
				}

			}
		}



		//flip level data vertically because (0,0) is top, left, and then put bottom in (0,0)
		for (size_t i = 0; i < m_MapData.size(); i++)
		{
			m_MapData.at(i).LevelRowIndex = -(m_MapData.at(i).LevelRowIndex);
		}
	}

	UAnimatedTextureMap::UAnimatedTextureMap(int framesPerSecond) 
	{
		m_TimeToSwitchFrame = 1.f / float(framesPerSecond);
	}
	void UAnimatedTextureMap::AddTextureMap(const UTextureMap& textureMap)
	{
		m_TextureMapInfo.push_back(FInternalTextureMapInfo{ textureMap });
	}
	void UAnimatedTextureMap::Update(float deltaSeconds)
	{
		m_Counter += deltaSeconds;

		if (m_Counter >= m_TimeToSwitchFrame)
		{
			m_Counter = 0;

			m_CurrentIndex = m_CurrentIndex >= m_TextureMapInfo.size() - 1 ? 0 : m_CurrentIndex + 1;
		}
	}
	void UAnimatedTextureMap::Draw(const Rectf& cameraWorldRect) const
	{
		m_TextureMapInfo.at(m_CurrentIndex).TextureMap.Draw(cameraWorldRect);
	}
}