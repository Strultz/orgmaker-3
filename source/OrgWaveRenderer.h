// Written by CKDEV

#pragma once

#include <cstddef>
#include <memory>

namespace OrgWaveRenderer
{

struct RGB
{
	unsigned char r, g, b;
};

struct Image
{
	size_t width;
	size_t height;
	std::unique_ptr<RGB[]> data;
};

struct RenderParameters
{
	size_t width;
	size_t height;

	size_t antiAliasingFactor;

	RGB backgroundColor;

	RGB lineColor;
	float lineWidth;

	RGB baselineColor;
	float baselineWidth;
};

Image RenderWave(int8_t(&waveData)[256], const RenderParameters &params);

}
