#include "ambition/resources/texture.h"

namespace ambition::resources {
	BaseTexture::BaseTexture(std::vector<unsigned char> data, enum PixelFormat format, enum PixelType type) :
			data(std::move(data)),
			pixelFormat(format),
			pixelType(type) {

	}

	const std::vector<unsigned char>& BaseTexture::Data() const {
		return data;
	}

	PixelFormat BaseTexture::PixelFormat() const {
		return pixelFormat;
	}

	PixelType BaseTexture::PixelType() const {
		return pixelType;
	}

	Texture2D::Texture2D(std::vector<unsigned char> data, enum PixelFormat format, enum PixelType type,
						 glm::ivec2 size) :
						 BaseTexture(std::move(data), format, type),
						 size(size) {

	}

	glm::ivec2 Texture2D::Size() const {
		return size;
	}

	Texture3D::Texture3D(std::vector<unsigned char> data, enum PixelFormat format, enum PixelType type,
						 glm::ivec3 size) :
						 BaseTexture(std::move(data), format, type),
						 size(size) {

	}

	glm::ivec3 Texture3D::Size() const {
		return size;
	}
}