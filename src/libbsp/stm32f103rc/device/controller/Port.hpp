//
// Kernel Device
//
#pragma once

#include "../fdt/Fdt.hpp"
#include <noncopyable.hpp>

namespace Chino
{
	namespace Device
	{
		enum class PortPins
		{
			Pin0,
			Pin1,
			Pin2,
			Pin3,
			Pin4,
			Pin5,
			Pin6,
			Pin7,
			Pin8,
			Pin9,
			Pin10,
			Pin11,
			Pin12,
			Pin13,
			Pin14,
			Pin15
		};

		enum class PortInputMode
		{
			Analog = 0,
			Floating = 1,
			PullUpDown = 2
		};

		enum class PortOutputMode
		{
			PushPull = 0,
			OpenDrain = 1,
			AF_PushPull = 2,
			AF_OpenDrain = 3
		};

		enum class PortSpeed
		{
			PS_10MHz = 1,
			PS_2MHz = 2,
			PS_50MHz = 1
		};

		class PortPin : public Device, public ExclusiveObjectAccess
		{
		private:
			friend class PortDevice;

			PortPin(size_t portIndex, uintptr_t portRegAddr, PortPins pin);

			void SetMode(PortInputMode mode);
			void SetMode(PortOutputMode mode, PortSpeed speed);
		private:
			uintptr_t portRegAddr_;
			PortPins pin_;
		};
	}
}
