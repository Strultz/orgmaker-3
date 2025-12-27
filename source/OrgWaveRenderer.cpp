// Written by CKDEV

#include "OrgWaveRenderer.h"

#include <cstdint>
#include <cstddef>
#include <memory>
#include <cmath>

namespace OrgWaveRenderer
{

static ptrdiff_t MapSampleToY(float sample, size_t imageHeight)
{
	return static_cast<ptrdiff_t>(imageHeight / 2) - (sample * (imageHeight / 2));
}

static void PlotDot(RGB *imageData, size_t imageWidth, size_t imageHeight, ptrdiff_t x, ptrdiff_t y, const RGB &color, size_t dotSize)
{
	for (ptrdiff_t dy = -static_cast<ptrdiff_t>(dotSize) / 2; dy <= static_cast<ptrdiff_t>(dotSize) / 2; dy++)
	{
		for (ptrdiff_t dx = -static_cast<ptrdiff_t>(dotSize) / 2; dx <= static_cast<ptrdiff_t>(dotSize) / 2; dx++)
		{
			ptrdiff_t px = x + dx;
			ptrdiff_t py = y + dy;
			if (px >= 0 && px < static_cast<ptrdiff_t>(imageWidth) && py >= 0 && py < static_cast<ptrdiff_t>(imageHeight))
			{
				imageData[py * imageWidth + px] = color;
			}
		}
	}
}

Image RenderWave(int8_t(&waveData)[256], const RenderParameters &params)
{
	size_t imageWidth = params.width;
	size_t imageHeight = params.height;
	std::unique_ptr<RGB[]> imageData = std::make_unique<RGB[]>(imageWidth * imageHeight);

	// First render at higher resolution for anti-aliasing the waveform line
	{
		size_t aaImageWidth = params.width * params.antiAliasingFactor;
		size_t aaImageHeight = params.height * params.antiAliasingFactor;
		std::unique_ptr<RGB[]> aaImageData = std::make_unique<RGB[]>(aaImageWidth * aaImageHeight);

		// Fill background
		size_t baselineWidthPixels = std::round(params.baselineWidth * params.antiAliasingFactor);
		
		ptrdiff_t baselineTopY = (aaImageHeight / 2) - (baselineWidthPixels / 2);
		ptrdiff_t baselineBottomY = baselineTopY + baselineWidthPixels;
		
		for (size_t y = 0; y < aaImageHeight; y++)
		{
			for (size_t x = 0; x < aaImageWidth; x++)
			{
				if (y >= baselineTopY && y < baselineBottomY)
					aaImageData[y * aaImageWidth + x] = params.baselineColor;
				else
					aaImageData[y * aaImageWidth + x] = params.backgroundColor;
			}
		}

		// Render waveform
		size_t lineWidthPixels = std::round(params.lineWidth * params.antiAliasingFactor);

		ptrdiff_t lastY = MapSampleToY(static_cast<float>(waveData[0]) / 100.0f, aaImageHeight);

		for (size_t x = 0; x < aaImageWidth; x++)
		{
			// Map x to waveData index
			size_t waveIndex = x * 256 / aaImageWidth;
			float t = (static_cast<float>(x) * 256.0f / static_cast<float>(aaImageWidth)) - static_cast<float>(waveIndex);

			float sampleFrom = static_cast<float>(waveData[waveIndex]) / 100.0f;
			float sampleTo = static_cast<float>(waveData[(waveIndex + 1) % 256]) / 100.0f;

			float sample = sampleFrom + t * (sampleTo - sampleFrom);

			ptrdiff_t y = MapSampleToY(sample, aaImageHeight);
			
			ptrdiff_t y0 = std::min(y, lastY);
			ptrdiff_t y1 = std::max(y, lastY);

			lastY = y;

			for (ptrdiff_t yy = y0; yy <= y1; yy++)
				PlotDot(aaImageData.get(), aaImageWidth, aaImageHeight, x, yy, params.lineColor, lineWidthPixels);
		}

		// Downsample to final image size
		for (size_t y = 0; y < imageHeight; y++)
		{
			for (size_t x = 0; x < imageWidth; x++)
			{
				size_t r = 0, g = 0, b = 0;
				for (size_t ay = 0; ay < params.antiAliasingFactor; ay++)
				{
					for (size_t ax = 0; ax < params.antiAliasingFactor; ax++)
					{
						size_t srcX = x * params.antiAliasingFactor + ax;
						size_t srcY = y * params.antiAliasingFactor + ay;
						RGB &pixel = aaImageData[srcY * aaImageWidth + srcX];
						r += pixel.r;
						g += pixel.g;
						b += pixel.b;
					}
				}
				size_t totalSamples = params.antiAliasingFactor * params.antiAliasingFactor;
				imageData[y * imageWidth + x] = {
					static_cast<unsigned char>(r / totalSamples),
					static_cast<unsigned char>(g / totalSamples),
					static_cast<unsigned char>(b / totalSamples)
				};
			}
		}
	}

	return Image{ imageWidth, imageHeight, std::move(imageData) };
}

}
