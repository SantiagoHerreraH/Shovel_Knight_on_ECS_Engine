#include "../../../include/Systems/DrawSystems/UAnimation.h"
#include "../../../include/Systems/MathSystems/STools.h"
#include <cassert>

namespace Pillar {

#pragma region Animation

	void UAnimation::Play()
	{
		m_IsPlaying = true;
	}
	void UAnimation::Pause()
	{
		m_IsPlaying = false;
	}
	const UTextureRef& UAnimation::GetConstTextureRef(int frameNumber)const
	{
		return m_Frames.at(frameNumber).Texture;
	}
	void UAnimation::CanLoop(bool canLoop)
	{
		m_CanLoop = canLoop;
	}
	void UAnimation::SetFramesPerSecond(unsigned int numberOfFrames)
	{
		m_SecondsPerFrame = 1.f / float(numberOfFrames);

		float sumedTime{ 0 };
		for (size_t i = 0; i < m_Frames.size(); i++)
		{
			m_Frames.at(i).TimeToGoToTheNextFrame = sumedTime + (m_Frames.at(i).NumberOfFrames * m_SecondsPerFrame);
			sumedTime = m_Frames.at(i).TimeToGoToTheNextFrame;
		}

		Reset();
	}
	void UAnimation::AddFrame(const UTextureRef& frame, unsigned int numberOfFrames)
	{
		float sumedTime = m_Frames.empty() ? 0 : m_Frames.at(m_Frames.size() - 1).TimeToGoToTheNextFrame;

		m_Frames.emplace_back(FAnimationFrameInfo{ frame, numberOfFrames , 0});

		m_Frames.at(m_Frames.size() - 1).TimeToGoToTheNextFrame = sumedTime + (m_Frames.at(m_Frames.size() - 1).NumberOfFrames * m_SecondsPerFrame);
	}

	void UAnimation::AddFrames(UTexture& texture, const Rectf& startFrameBound, int numberOfFrames, int xSpaceBetweenFrames)
	{
		if (numberOfFrames <= 0)
		{
			return;
		}

		std::vector<UTextureRef> refs{};
		refs.reserve(numberOfFrames);

		Rectf currentFrameBounds{ startFrameBound };

		for (size_t i = 0; i < numberOfFrames; i++)
		{
			refs.push_back(UTextureRef(texture));
			refs.at(i).SetTextureView(currentFrameBounds);
			AddFrame(refs.at(i), 1);

			currentFrameBounds.left += currentFrameBounds.width + xSpaceBetweenFrames + 1;
		}
	}

	//void UAnimation::AddFrames(USpriteSheet& spriteSheet, unsigned int startRowIndex, unsigned int startColumnIndex, unsigned int endRowIndex, unsigned int endColumnIndex)
	//{
	//	assert(endRowIndex >= startRowIndex);
	//	assert(endColumnIndex >= startColumnIndex);
	//	assert(spriteSheet.IsValid());
	//
	//	unsigned int deltaRow{ endRowIndex - startRowIndex};
	//	unsigned int deltaColumn{ endColumnIndex  - startColumnIndex };
	//
	//	for (size_t rowIdx = 0; rowIdx <= deltaRow; rowIdx++)
	//	{
	//		for (size_t colIdx = 0; colIdx <= deltaColumn; colIdx++)
	//		{
	//			AddFrame(spriteSheet.GetFrame(startRowIndex + rowIdx, startColumnIndex + colIdx), 1);
	//		}
	//	}
	//}


	void UAnimation::SetFrameNumberInFrame(size_t frameIndex, int numberOfFrames)
	{
		if (frameIndex < m_Frames.size())
		{
			m_Frames.at(frameIndex).NumberOfFrames = numberOfFrames;

			float sumedTime = frameIndex > 0 ? m_Frames.at(frameIndex - 1).TimeToGoToTheNextFrame : 0.f;

			for (size_t i = frameIndex; i < m_Frames.size(); i++)
			{
				m_Frames.at(i).TimeToGoToTheNextFrame = sumedTime + (m_Frames.at(i).NumberOfFrames * m_SecondsPerFrame);
				sumedTime = m_Frames.at(i).TimeToGoToTheNextFrame;
			}

			Reset();
		}
	}

	void UAnimation::ClearFrames()
	{
		m_NumberOfLoops = 0;
		m_IsPlaying = false;
		m_CurrentTime = 0 ;
		m_CurrentFrameIndex = 0;
		m_SecondsPerFrame = 1;
		m_Frames.clear();
	}

	int UAnimation::LoopNumber()
	{
		return m_NumberOfLoops;
	}

	void UAnimation::Reset()
	{
		m_CurrentTime = 0;
		m_NumberOfLoops = 0;
		m_CurrentFrameIndex = 0;
	}

	void UAnimation::Draw() const
	{
		m_Frames.at(m_CurrentFrameIndex).Texture.Draw();
	}
	void UAnimation::Update(float deltaSeconds)
	{
		if (m_IsPlaying)
		{
			m_CurrentTime += deltaSeconds;
			//desired time to change to next frame

			if (m_CurrentTime >= m_Frames.at(m_CurrentFrameIndex).TimeToGoToTheNextFrame)
			{
				

				if (m_CanLoop)
				{
					m_NumberOfLoops = m_CurrentFrameIndex >= m_Frames.size() - 1 ? m_NumberOfLoops + 1 : m_NumberOfLoops;

					m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_Frames.size();
				}
				else if(!m_CanLoop && !m_Frames.empty())
				{

					m_NumberOfLoops = (m_CurrentFrameIndex >= m_Frames.size() - 1) && m_NumberOfLoops < 1 ? m_NumberOfLoops + 1 : m_NumberOfLoops;

					m_CurrentFrameIndex = STools::Clamp((m_CurrentFrameIndex + 1), 0, m_Frames.size() - 1);
				}

				m_CurrentTime = m_CurrentFrameIndex == 0 ? 0 : m_CurrentTime;

			}
		}
	}

#pragma endregion



}

