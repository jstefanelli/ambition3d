#include "ambition/resources/texture.h"

namespace ambition::resources {
	BaseTexture::BaseTexture(const std::vector<unsigned char>& data, enum PixelFormat format, enum PixelType type) : data(std::move(data)) {

	}
}