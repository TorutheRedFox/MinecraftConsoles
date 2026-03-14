#include "stdafx.h"
#include "Color.h"
#include "..\Minecraft.Client\Minecraft.h"
#include "net.minecraft.world.level.levelgen.feature.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.entity.animal.h"
#include "net.minecraft.world.entity.monster.h"
#include "net.minecraft.world.entity.h"
#include "Biome.h"
#include "net.minecraft.world.level.biome.h"

//public static final Biome[] biomes = new Biome[256];
Biome *Biome::biomes[256];

Biome *Biome::rainForest = nullptr;
Biome *Biome::swampland = nullptr;
Biome *Biome::seasonalForest = nullptr;
Biome *Biome::forest = nullptr;

Biome *Biome::savanna = nullptr;
Biome *Biome::shrubland = nullptr;
Biome *Biome::taiga = nullptr;

Biome *Biome::desert = nullptr;
Biome *Biome::plains = nullptr;
Biome *Biome::iceDesert = nullptr;
Biome *Biome::tundra = nullptr;

Biome *Biome::hell = nullptr;
Biome *Biome::sky = nullptr;

Biome *Biome::map[4096];

void Biome::staticCtor()
{
	//public static final Biome[] biomes = new Biome[256];
	Biome::rainForest = (new RainforestBiome(0))->setColor(0x08fa36)->setName(L"Rainforest")->setLeafColor(0x1ff458);
	Biome::swampland = (new SwampBiome(1))->setColor(0x07f9b2)->setName(L"Swampland")->setLeafColor(0x8baf48);
	Biome::seasonalForest = (new Biome(2))->setColor(0x9be023)->setName(L"Seasonal Forest");
	Biome::forest = (new ForestBiome(3))->setColor(0x056621)->setName(L"Forest");

	Biome::savanna = (new FlatBiome(4))->setColor(0xd9e023)->setName(L"Savanna");
	Biome::shrubland = (new Biome(5))->setColor(0xa1ad20)->setName(L"Shrubland");
	Biome::taiga = (new TaigaBiome(6))->setColor(0x0b6659)->setName(L"Taiga")->setSnowCovered()->setLeafColor(0x7bb731);
	Biome::desert = (new FlatBiome(7))->setColor(0xfa9418)->setName(L"Desert");
	Biome::plains = (new FlatBiome(8))->setColor(0xffd910)->setName(L"Plains");
	Biome::iceDesert = (new FlatBiome(9))->setColor(0xffed93)->setName(L"Ice Desert")->setSnowCovered()->setLeafColor(0xc4d339);
	Biome::tundra = (new Biome(10))->setColor(0x57ebf9)->setName(L"Tundra")->setSnowCovered()->setLeafColor(0xc4d339);

	Biome::hell = (new HellBiome(11))->setColor(0xff0000)->setName(L"Hell")->setNoRain()->setTemperatureAndDownfall(2, 0)->setLeafFoliageWaterSkyColor(eMinecraftColour_Grass_Hell, eMinecraftColour_Foliage_Hell, eMinecraftColour_Water_Hell,eMinecraftColour_Sky_Hell);
	Biome::sky = (new TheEndBiome(12))->setColor(0x8080ff)->setName(L"Sky")->setNoRain()->setLeafFoliageWaterSkyColor(eMinecraftColour_Grass_Sky, eMinecraftColour_Foliage_Sky, eMinecraftColour_Water_Sky,eMinecraftColour_Sky_Sky);

	// recalc
	for (int a = 0; a < 64; a++) {
		for (int b = 0; b < 64; b++) {
			map[a + b * 64] = _getBiome(a / 63.0f, b / 63.0f);
		}
	}

	Biome::desert->topMaterial = Biome::desert->material = (byte)Tile::sand->id;
	Biome::iceDesert->topMaterial = Biome::iceDesert->material = (byte)Tile::sand->id;
}

Biome *Biome::getBiome(double temperature, double downfall) {
	int a = (int)(temperature * 63.0);
	int b = (int)(downfall * 63.0);
	return Biome::map[a + b * 64];
}

Biome *Biome::_getBiome(float temperature, float downfall)
{
	downfall *= (temperature);
	if (temperature < 0.10f) {
		return Biome::tundra;
	} else if (downfall < 0.20f) {
		if (temperature < 0.50f) {
			return Biome::tundra;
		} else if (temperature < 0.95f) {
			return Biome::savanna;
		} else {
			return Biome::desert;
		}
	} else if (downfall > 0.5f && temperature < 0.7f) {
		return Biome::swampland;
	} else if (temperature < 0.50f) {
		return Biome::taiga;
	} else if (temperature < 0.97f) {
		if (downfall < 0.35f) {
			return Biome::shrubland;
		} else {
			return Biome::forest;
		}
	} else {
		if (downfall < 0.45f) {
			return Biome::plains;
		} else if (downfall < 0.90f) {
			return Biome::seasonalForest;
		} else {
			return Biome::rainForest;
		}
	}
}

Biome::Biome(int id) : id(id)
{
	// 4J Stu Default inits
	color = 0;
	snowCovered = false;	// 4J - this isn't set by the java game any more so removing to save confusion

	topMaterial = static_cast<byte>(Tile::grass_Id);
	material = static_cast<byte>(Tile::dirt_Id);
	leafColor = 0x4EE031;
	_hasRain = true;
	depth = 0.1f;
	scale = 0.3f;
	temperature = 0.5f;
	downfall = 0.5f;
	//waterColor = 0xffffff; // 4J Stu - Not used
	decorator = nullptr;

	m_grassColor = eMinecraftColour_NOT_SET;
	m_foliageColor = eMinecraftColour_NOT_SET;
	m_waterColor = eMinecraftColour_NOT_SET;

	/*	4J - removing these so that we can consistently return newly created trees via getTreeFeature, and let the calling function be resposible for deleting the returned tree
	normalTree = new TreeFeature();
	fancyTree = new BasicTree();
	birchTree = new BirchFeature();
	swampTree = new SwampTreeFeature();
	*/

	biomes[id] = this;
	decorator = createDecorator();

	friendlies.push_back(new MobSpawnerData(eTYPE_SHEEP, 12, 4, 4));
	friendlies.push_back(new MobSpawnerData(eTYPE_PIG, 10, 4, 4));
	friendlies_chicken.push_back(new MobSpawnerData(eTYPE_CHICKEN, 10, 4, 4));		// 4J - moved chickens to their own category
	friendlies.push_back(new MobSpawnerData(eTYPE_COW, 8, 4, 4));

	enemies.push_back(new MobSpawnerData(eTYPE_SPIDER, 10, 4, 4));
	enemies.push_back(new MobSpawnerData(eTYPE_ZOMBIE, 10, 4, 4));
	enemies.push_back(new MobSpawnerData(eTYPE_SKELETON, 10, 4, 4));
	enemies.push_back(new MobSpawnerData(eTYPE_CREEPER, 10, 4, 4));
	//enemies.push_back(new MobSpawnerData(eTYPE_SLIME, 10, 4, 4));
	//enemies.push_back(new MobSpawnerData(eTYPE_ENDERMAN, 1, 1, 4));

	// wolves are added to forests and taigas

	waterFriendlies.push_back(new MobSpawnerData(eTYPE_SQUID, 10, 4, 4));

	//ambientFriendlies.push_back(new MobSpawnerData(eTYPE_BAT, 10, 8, 8));
}

Biome::~Biome()
{
	if(decorator != nullptr) delete decorator;
}

BiomeDecorator *Biome::createDecorator()
{
	return new BiomeDecorator(this);
}

// 4J Added
Biome *Biome::setLeafFoliageWaterSkyColor(eMinecraftColour grassColor, eMinecraftColour foliageColor, eMinecraftColour waterColour, eMinecraftColour skyColour)
{
	m_grassColor = grassColor;
	m_foliageColor = foliageColor;
	m_waterColor = waterColour;
	m_skyColor = skyColour;
	return this;
}

Biome *Biome::setTemperatureAndDownfall(float temp, float downfall)
{
	temperature = temp;
	this->downfall = downfall;
	return this;
}

Biome *Biome::setDepthAndScale(float depth, float scale)
{
	this->depth = depth;
	this->scale = scale;
	return this;
}

Biome *Biome::setNoRain()
{
	_hasRain = false;
	return this;
}

Feature *Biome::getTreeFeature(Random *random)
{
	if (random->nextInt(10) == 0)
	{
		return new BasicTree(false); // 4J used to return member fancyTree, now returning newly created object so that caller can be consistently resposible for cleanup
	}
	return new TreeFeature(false); // 4J used to return member normalTree, now returning newly created object so that caller can be consistently resposible for cleanup
}

Feature *Biome::getGrassFeature(Random *random)
{
	return new TallGrassFeature(Tile::tallgrass_Id, TallGrass::TALL_GRASS);
}

Biome *Biome::setSnowCovered()
{
	snowCovered = true;
	return this;
}

Biome *Biome::setName(const wstring &name)
{
	this->m_name = name;
	return this;
}

Biome *Biome::setLeafColor(int leafColor)
{
	this->leafColor = leafColor;
	return this;
}

Biome *Biome::setColor(int color)
{
	this->color = color;
	return this;
}

int Biome::getSkyColor(float temp)
{
	temp /= 3.0f;
	if (temp < -1) temp = -1;
	if (temp > 1) temp = 1;
	return Color::getHSBColor(224 / 360.0f - temp * 0.05f, 0.50f + temp * 0.1f, 1.0f).getRGB();

	// 4J Stu - Load colour from texture pack
	//return Minecraft::GetInstance()->getColourTable()->getColor( m_skyColor );
}

vector<Biome::MobSpawnerData *> *Biome::getMobs(MobCategory *category)
{
	if (category == MobCategory::monster) return &enemies;
	if (category == MobCategory::creature) return &friendlies;
	if (category == MobCategory::waterCreature) return &waterFriendlies;
	if (category == MobCategory::creature_chicken) return &friendlies_chicken;
	if (category == MobCategory::creature_wolf) return &friendlies_wolf;
	if (category == MobCategory::creature_mushroomcow) return &friendlies_mushroomcow;
	if (category == MobCategory::ambient) return &ambientFriendlies;
	return nullptr;
}

bool Biome::hasSnow()
{
	// 4J - snowCovered flag removed as it wasn't being set by the game anymore - snow is now temperature dependent to match code in rain rendering, shouldFreeze functions etc.
	if( !_hasRain ) return false;

	//if( getTemperature() >= 0.15f ) return false;

	return this->snowCovered;
}

bool Biome::hasRain()
{
	// 4J - snowCovered flag removed as it wasn't being set by the game anymore, replaced by call to hasSnow()
	if( hasSnow() ) return false;
	//    if (snowCovered) return false;
	return _hasRain;
}

bool Biome::isHumid()
{
	return downfall > .85f;
}

float Biome::getCreatureProbability()
{
	return 0.1f;
}

int Biome::getDownfallInt()
{
	return static_cast<int>(downfall * 65536);
}

int Biome::getTemperatureInt()
{
	return static_cast<int>(temperature * 65536);
}

// 4J - brought forward from 1.2.3
float Biome::getDownfall()
{
	return downfall;
}

// 4J - brought forward from 1.2.3
float Biome::getTemperature()
{
	return temperature;
}

void Biome::decorate(Level *level, Random *random, int xo, int zo)
{
	decorator->decorate(level, random, xo, zo);
}

int Biome::getGrassColor()
{
	double temp = Mth::clamp(getTemperature(), 0.0f, 1.0f);
	double rain = Mth::clamp(getDownfall(), 0.0f, 1.0f);

	return GrassColor::get(temp, rain);
	//return Minecraft::GetInstance()->getColourTable()->getColor( m_grassColor );
}

int Biome::getFolageColor()
{
	double temp = Mth::clamp(getTemperature(), 0.0f, 1.0f);
	double rain = Mth::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::get(temp, rain);
	//return Minecraft::GetInstance()->getColourTable()->getColor( m_foliageColor );
}

// 4J Added
int Biome::getWaterColor()
{
	return Minecraft::GetInstance()->getColourTable()->getColor( m_waterColor );
}