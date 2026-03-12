#pragma once

#include "Biome.h"

class FlatBiome : public Biome
{
	friend class Biome;
protected:
	FlatBiome(int id);
};