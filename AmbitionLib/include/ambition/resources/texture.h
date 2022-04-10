#pragma once
#include "resource.h"
#include <vector>
#include <glm/glm.hpp>

namespace ambition::resources {
	enum class PixelFormat {
		RGB,
		RGBA,
		BGR,
		BGRA,
		RED,
		DEPTH,
		LUMINANCE
	};

	enum class PixelType {
		BYTE,
		SHORT,
		INT,
		FLOAT,
		DOUBLE
	};

	class BaseTexture : public Resource {
	protected:
		std::vector<unsigned char> data;
		PixelFormat pixelFormat;
		PixelType pixelType;

		~BaseTexture() override = 0;
	public:
		BaseTexture(const std::vector<unsigned char>& data, PixelFormat format, PixelType type);

		const std::vector<unsigned char>& Data() const;
		PixelFormat PixelFormat() const;
		PixelType PixelType() const;

		static constexpr size_t BytesPerChannel(enum PixelType type) {
			switch(type) {
				case PixelType::BYTE:
					return 1;
				case PixelType::SHORT:
					return 2;
				case PixelType::INT:
					return 3;
				case PixelType::FLOAT:
					return 4;
				case PixelType::DOUBLE:
					return 8;
				default:
					return 0;
			}
		}

		static constexpr size_t BytesPerPixel(enum PixelFormat format, enum PixelType type) {
			size_t bytesPerChannel = BytesPerChannel(type);
			size_t numChannels = 0;
			switch(format) {
				case PixelFormat::RGB:
					numChannels = 3;
					break;
				case PixelFormat::RGBA:
					numChannels = 4;
					break;
				case PixelFormat::BGR:
					numChannels = 3;
					break;
				case PixelFormat::BGRA:
					numChannels = 4;
					break;
				case PixelFormat::RED:
				case PixelFormat::DEPTH:
				case PixelFormat::LUMINANCE:
					numChannels = 1;
					break;
			}

			return bytesPerChannel * numChannels;
		}
	};

	class Texture2D : public BaseTexture {
	protected:
		glm::ivec2 size;
	public:
		Texture2D(std::vector<unsigned char> data, enum PixelFormat format, enum PixelType type, glm::ivec2 size);
		glm::ivec2 Size() const;
	};

	class Texture3D : public BaseTexture {
	protected:
		glm::ivec3 size;
	public:
		Texture3D(std::vector<unsigned char> data, enum PixelFormat format, enum PixelType type, glm::ivec3 size);
		glm::ivec3 Size() const;
	};
}