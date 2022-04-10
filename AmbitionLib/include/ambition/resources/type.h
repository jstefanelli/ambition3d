#pragma once
#include <cstdint>

namespace ambition::resources {
	enum class Type : uint32_t {
		Unknown = 0,
		Model = 1,
		Texture = 2,
		Material = 3,
		Shader = 4,
		Sound = 5,
	};
}