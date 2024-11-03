#pragma once
#include "Ukemochi-Engine/Graphics/Framebuffer.h"

namespace Ukemochi {

	class OpenGlFrameBuffer : public Framebuffer
	{
	public:
		OpenGlFrameBuffer();
		virtual ~OpenGlFrameBuffer();
	private:
		FramebufferSpecs m_Specs;
	};
}