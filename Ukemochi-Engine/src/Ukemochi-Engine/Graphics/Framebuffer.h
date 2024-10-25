#pragma once
#include "Ukemochi-Engine/Core.h"

namespace Ukemochi {

	struct FramebufferSpecs
	{

	};

	class Framebuffer
	{
	public:
		//virtual FramebufferSpecs& GetSpecs() = 0;
		virtual const FramebufferSpecs& GetSpecs() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecs& specs);
	};
}