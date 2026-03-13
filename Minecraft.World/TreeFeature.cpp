#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "TreeFeature.h"

TreeFeature::TreeFeature(bool doUpdate) : Feature(doUpdate), baseHeight(4), trunkType(0), leafType(0), addJungleFeatures(false)
{
}

TreeFeature::TreeFeature(bool doUpdate, int baseHeight, int trunkType, int leafType, bool addJungleFeatures) : Feature(doUpdate), baseHeight(baseHeight), trunkType(trunkType), leafType(leafType), addJungleFeatures(addJungleFeatures)
{
}

bool TreeFeature::place(Level *level, Random *random, int x, int y, int z)
{	
	int treeHeight = random->nextInt(3) + baseHeight;

	bool free = true;
	if (y < 1 || y + treeHeight + 1 > Level::maxBuildHeight) return false;

	// 4J Stu Added to stop tree features generating areas previously place by game rule generation
	if(app.getLevelGenerationOptions() != nullptr)
	{
		PIXBeginNamedEvent(0,"TreeFeature checking intersects");
		LevelGenerationOptions *levelGenOptions = app.getLevelGenerationOptions();
		bool intersects = levelGenOptions->checkIntersects(x - 2, y - 1, z - 2, x + 2, y + treeHeight, z + 2);
		PIXEndNamedEvent();
		if(intersects)
		{
			//app.DebugPrintf("Skipping reeds feature generation as it overlaps a game rule structure\n");
			return false;
		}
	}

	for (int yy = y; yy <= y + 1 + treeHeight; yy++)
	{
		int r = 1;
		if (yy == y) r = 0;
		if (yy >= y + 1 + treeHeight - 2) r = 2;
		for (int xx = x - r; xx <= x + r && free; xx++)
		{
			for (int zz = z - r; zz <= z + r && free; zz++)
			{
				if (yy >= 0 && yy < Level::maxBuildHeight)
				{
					int tt = level->getTile(xx, yy, zz);
					if (tt != 0 && tt != Tile::leaves_Id && tt != Tile::grass_Id && tt != Tile::dirt_Id && tt != Tile::treeTrunk_Id) free = false;
				}
				else
				{
					free = false;
				}
			}
		}
	}

	if (!free) return false;

	int belowTile = level->getTile(x, y - 1, z);
	if ((belowTile != Tile::grass_Id && belowTile != Tile::dirt_Id) || y >= Level::maxBuildHeight - treeHeight - 1) return false;

	placeBlock(level, x, y - 1, z, Tile::dirt_Id, 0);
	
	PIXBeginNamedEvent(0,"Placing TreeFeature leaves");
	int grassHeight = 3;
	int extraWidth = 0;
	// 4J Stu - Generate leaves from the top down to stop having to recalc heightmaps
	for (int yy = y + treeHeight; yy >= y - grassHeight + treeHeight; yy--)
	{
		int yo = yy - (y + treeHeight);
		int offs = extraWidth + 1 - yo / 2;
		for (int xx = x - offs; xx <= x + offs; xx++)
		{
			int xo = xx - (x);
			for (int zz = z - offs; zz <= z + offs; zz++)
			{
				int zo = zz - (z);
				if (abs(xo) == offs && abs(zo) == offs && (random->nextInt(2) == 0 || yo == 0)) continue;
				int t = level->getTile(xx, yy, zz);
				if (t == 0 || t == Tile::leaves_Id) placeBlock(level, xx, yy, zz, Tile::leaves_Id, leafType);
			}
		}
	}
	PIXEndNamedEvent();
	PIXBeginNamedEvent(0,"Placing TreeFeature trunks");
	for (int hh = 0; hh < treeHeight; hh++)
	{
		int t = level->getTile(x, y + hh, z);
		if (t == 0 || t == Tile::leaves_Id)
		{
			placeBlock(level, x, y + hh, z, Tile::treeTrunk_Id, trunkType);
		}
	}
	PIXEndNamedEvent();

	return true;
}

void TreeFeature::addVine(Level *level, int xx, int yy, int zz, int dir)
{
	placeBlock(level, xx, yy, zz, Tile::vine_Id, dir);
	int maxDir = 4;
	while (level->getTile(xx, --yy, zz) == 0 && maxDir > 0)
	{
		placeBlock(level, xx, yy, zz, Tile::vine_Id, dir);
		maxDir--;
	}
}