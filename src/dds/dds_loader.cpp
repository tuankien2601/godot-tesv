#include "gli/gli.hpp"
#include "dds_loader.h"
#include <godot_cpp/classes/project_settings.hpp>
#define BCDEC_IMPLEMENTATION
extern "C"
{
#include "bcdec.h"
}

using namespace godot;

bool DDSLoader::_enable_compression = false;
Image::CompressMode DDSLoader::_compression_mode = Image::CompressMode::COMPRESS_ETC2;

void DDSLoader::_bind_methods()
{
	ClassDB::bind_static_method("DDSLoader", D_METHOD("load", "input"), &DDSLoader::load);
	ClassDB::bind_static_method("DDSLoader", D_METHOD("enable_compression", "enabled"), &DDSLoader::enable_compression);
}

Ref<ImageTexture> DDSLoader::load(PackedByteArray input)
{
	gli::texture tex = gli::load(reinterpret_cast<const char *>(input.ptr()), input.size());

	if (tex.empty())
	{
		UtilityFunctions::printerr("Invalid dds file");
		return Ref<ImageTexture>();
	}

	int w = tex.extent().x;
	int h = tex.extent().y;

	gli::format fmt = tex.format();

	size_t byte_per_pixel = gli::component_count(fmt);
	if (fmt == gli::FORMAT_RGB_DXT1_UNORM_BLOCK8 || fmt == gli::FORMAT_RGB_DXT1_SRGB_BLOCK8)
	{
		byte_per_pixel = 4;
	}
	Image::Format gd_format = (Image::Format)(byte_per_pixel + 1);
	bool compressed = gli::is_compressed(fmt);

	const void *data = tex.data(0, 0, 0);
	size_t size = tex.size();
	size_t output_size = w * h * byte_per_pixel;

	PackedByteArray uncompressed_data;

	if (compressed)
	{
		size_t num_blocks_x = w / 4;
		size_t num_blocks_y = h / 4;

		uncompressed_data.resize(output_size);
		unsigned char *dest = uncompressed_data.ptrw();

		// Decompress each block
		for (size_t by = 0; by < num_blocks_y; by++)
		{
			for (size_t bx = 0; bx < num_blocks_x; bx++)
			{
				size_t block_idx = by * num_blocks_x + bx;
				unsigned char *block_dest = dest + (by * 4 * w + bx * 4) * byte_per_pixel;
				const char *block_src = (const char *)data;
				switch (fmt)
				{
				case gli::FORMAT_RGB_DXT1_UNORM_BLOCK8:
				case gli::FORMAT_RGB_DXT1_SRGB_BLOCK8:
				case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
				case gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8:
					block_src += block_idx * 8;
					bcdec_bc1(block_src, block_dest, w * 4);
					break;

				case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
				case gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16:
					block_src += block_idx * 16;
					bcdec_bc2(block_src, block_dest, w * 4);
					break;

				case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
				case gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16:
					block_src += block_idx * 16;
					bcdec_bc3(block_src, block_dest, w * 4);
					break;

				case gli::FORMAT_R_ATI1N_UNORM_BLOCK8:
				case gli::FORMAT_R_ATI1N_SNORM_BLOCK8:
					block_src += block_idx * 8;
					bcdec_bc4(block_src, block_dest, w);
					break;

				case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16:
				case gli::FORMAT_RG_ATI2N_SNORM_BLOCK16:
					block_src += block_idx * 16;
					bcdec_bc5(block_src, block_dest, w * 2);
					break;
				default:
					UtilityFunctions::printerr("Unknown compression ", fmt);
					return Ref<ImageTexture>();
				}
			}
		}
	}
	else
	{
		uncompressed_data.resize(size);
		memcpy(uncompressed_data.ptrw(), data, size);
	}

	// Create Godot Image
	Ref<Image> image = Image::create_from_data(w, h, false, gd_format, uncompressed_data);
	if (fmt == gli::FORMAT_RGB_DXT1_UNORM_BLOCK8 || fmt == gli::FORMAT_RGB_DXT1_SRGB_BLOCK8)
	{
		image->convert(Image::FORMAT_RGB8);
	}
	image->generate_mipmaps();
	if (_enable_compression)
	{
		auto compression_source = gli::is_srgb(fmt) ? Image::COMPRESS_SOURCE_SRGB : Image::COMPRESS_SOURCE_GENERIC;
		image->compress(_compression_mode, compression_source);
	}
	auto im_tex = ImageTexture::create_from_image(image);
	return im_tex;
}

void DDSLoader::enable_compression(bool enabled)
{
	_enable_compression = enabled;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__) || defined(__APPLE__)
	_compression_mode = Image::CompressMode::COMPRESS_S3TC;
#elif defined(__ANDROID__)
	_compression_mode = Image::CompressMode::COMPRESS_ETC2;
	String renderer = ProjectSettings::get_singleton()->get_setting("rendering/rendering_device/driver.android");

    if (renderer.find("vulkan") != -1) {
        _compression_mode = Image::CompressMode::COMPRESS_ASTC;
    }
#elif defined(TARGET_OS_IPHONE)
	_compression_mode = Image::CompressMode::COMPRESS_ASTC;
#endif
}