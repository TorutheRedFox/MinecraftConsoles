#include "stdafx.h"
#include "UI.h"
#include "UIComponent_Panorama.h"
#include "Minecraft.h"
#include "MultiPlayerLevel.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.level.dimension.h"
#include "..\..\..\Minecraft.World\net.minecraft.world.level.storage.h"
#include <Tesselator.h>

UIComponent_Panorama::UIComponent_Panorama(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bShowingDay = true;

	while(!m_hasTickedOnce) tick();
}

wstring UIComponent_Panorama::getMoviePath()
{
	switch( m_parentLayer->getViewport() )
	{
	case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
	case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
	case C4JRender::VIEWPORT_TYPE_SPLIT_LEFT:
	case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_LEFT:
	case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT:
		m_bSplitscreen = true;
		return L"PanoramaSplit";
		break;
	case C4JRender::VIEWPORT_TYPE_FULLSCREEN:
	default:
		m_bSplitscreen = false;
		return L"Panorama";
		break;
	}
}

void UIComponent_Panorama::tick()
{
	if(!hasMovie()) return;

	Minecraft *pMinecraft = Minecraft::GetInstance();
	EnterCriticalSection(&pMinecraft->m_setLevelCS);
	if(pMinecraft->level!=NULL)
	{
		int64_t i64TimeOfDay =0;
		// are we in the Nether? - Leave the time as 0 if we are, so we show daylight
		if(pMinecraft->level->dimension->id==0)
		{
			i64TimeOfDay = pMinecraft->level->getLevelData()->getGameTime() % 24000;
		}

		if(i64TimeOfDay>14000)
		{
			setPanorama(false);
		}
		else
		{
			setPanorama(true);
		}
	}
	else
	{
		setPanorama(true);
	}
	LeaveCriticalSection(&pMinecraft->m_setLevelCS);

	UIScene::tick();
}


void fillGradient(int x0, int y0, int x1, int y1, int col1, int col2)
{
	float a1 = ((col1 >> 24) & 0xff) / 255.0f;
	float r1 = ((col1 >> 16) & 0xff) / 255.0f;
	float g1 = ((col1 >> 8) & 0xff) / 255.0f;
	float b1 = ((col1) & 0xff) / 255.0f;

	float a2 = ((col2 >> 24) & 0xff) / 255.0f;
	float r2 = ((col2 >> 16) & 0xff) / 255.0f;
	float g2 = ((col2 >> 8) & 0xff) / 255.0f;
	float b2 = ((col2) & 0xff) / 255.0f;
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	Tesselator* t = Tesselator::getInstance();
	t->begin();
	t->color(r1, g1, b1, a1);
	t->vertex(static_cast<float>(x1), static_cast<float>(y0), 0);
	t->vertex(static_cast<float>(x0), static_cast<float>(y0), 0);
	t->color(r2, g2, b2, a2);
	t->vertex(static_cast<float>(x0), static_cast<float>(y1),0);
	t->vertex(static_cast<float>(x1), static_cast<float>(y1),0);
	t->end();

	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
}

void renderPanoramas(int xm, int ym, float a)
{
	Tesselator* t = Tesselator::getInstance();

	ScreenSizeCalculator ssc(Minecraft::GetInstance()->options, Minecraft::GetInstance()->width, Minecraft::GetInstance()->height);
	int width = ssc.getWidth();
	int height = ssc.getHeight();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(120.0f, 1.0f, 0.05f, 10.0f);
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// CHANGE - performance issues getting these for every loop iteration, get them beforehand
	int panoramas[6];
	for (int panorama = 0; panorama < 6; panorama++)
		panoramas[panorama] = Minecraft::GetInstance()->textures->loadTexture(TN_TITLE_BG_PANORAMA0 + panorama);

	static uint64_t prevTime = System::currentTimeMillis();
	uint64_t time = System::currentTimeMillis() - prevTime;
	prevTime = System::currentTimeMillis();

	static float vo = 0;
	vo += time / 60.0f;

	int antialias = 8; // 8 originally, runs terrible here for no visual improvement
	for (int l = 0; l < antialias * antialias; l++) {
		glPushMatrix();
		float x = ((float)(l % antialias) / (float)antialias - 0.5F) / 64.0f;
		float y = ((float)(l / antialias) / (float)antialias - 0.5F) / 64.0f;
		float z = 0.0F;
		glTranslatef(x, y, z);
		glRotatef(Mth::sin((vo + a) / 400.0f) * 25.0f + 20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-(vo + a) * 0.1f, 0.0f, 1.0f, 0.0f);
		for (int panorama = 0; panorama < 6; panorama++) {
			glPushMatrix();
			if (panorama == 1) {
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			}
			if (panorama == 2) {
				glRotatef(180.f, 0.0f, 1.0f, 0.0f);
			}
			if (panorama == 3) {
				glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			}
			if (panorama == 4) {
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			}
			if (panorama == 5) {
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			}
			//GL11.glBindTexture(GL11.GL_TEXTURE_2D, minecraft.textures
			//    .loadTexture((new StringBuilder()).append("/title/bg/panorama").append(i1).append(
			//        ".png").toString()));
			glBindTexture(GL_TEXTURE_2D, panoramas[panorama]);
			t->begin();
			t->color(0xffffff, 255 / (l + 1));
			float f4 = 0.0F;
			t->vertexUV(-1.0, -1.0, 1.0, 0.0f + f4, 0.0f + f4);
			t->vertexUV(1.0, -1.0, 1.0, 1.0f - f4, 0.0f + f4);
			t->vertexUV(1.0, 1.0, 1.0, 1.0f - f4, 1.0f - f4);
			t->vertexUV(-1.0, 1.0, 1.0, 0.0f + f4, 1.0f - f4);
			t->end();
			glPopMatrix();
		}

		glPopMatrix();
		glColorMask(true, true, true, false);
	}
	t->offset(0, 0, 0);
	glColorMask(true, true, true, true);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPopMatrix();
	glDepthMask(true);
	glEnable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);

	fillGradient(0, 0, width, height, 0xAAFFFFFF, 0x00FFFFFF); // white
	fillGradient(0, 0, width, height, 0x00000000, 0xAA000000); // black
}

void renderDirtBackground()
{
	// 4J Unused
//#if 0
	ScreenSizeCalculator ssc(Minecraft::GetInstance()->options, Minecraft::GetInstance()->width, Minecraft::GetInstance()->height);
	int width = ssc.getWidth();
	int height = ssc.getHeight();
	float vo = 0;

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	Tesselator* t = Tesselator::getInstance();
	//glBindTexture(GL_TEXTURE_2D, minecraft->textures->loadTexture(L"/gui/background.png"));
	glBindTexture(GL_TEXTURE_2D, Minecraft::GetInstance()->textures->loadTexture(TN_GUI_BACKGROUND));
	glColor4f(1, 1, 1, 1);
	float s = 32;
	t->begin();
	t->color(0x404040);
	t->vertexUV(static_cast<float>(0), static_cast<float>(height), static_cast<float>(0), static_cast<float>(0), static_cast<float>(height / s + vo));
	t->vertexUV(static_cast<float>(width), static_cast<float>(height), static_cast<float>(0), static_cast<float>(width / s), static_cast<float>(height / s + vo));
	t->vertexUV(static_cast<float>(width), static_cast<float>(0), static_cast<float>(0), static_cast<float>(width / s), static_cast<float>(0 + vo));
	t->vertexUV(static_cast<float>(0), static_cast<float>(0), static_cast<float>(0), static_cast<float>(0), static_cast<float>(0 + vo));
	t->end();
	//#endif
}

void UIComponent_Panorama::render(S32 width, S32 height, C4JRender::eViewportType viewport)
{
	//renderPanoramas(width, height, 0.137655854f);
	renderDirtBackground();
	return;
	bool specialViewport =	(viewport == C4JRender::VIEWPORT_TYPE_SPLIT_TOP) ||
		(viewport == C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM) ||
		(viewport == C4JRender::VIEWPORT_TYPE_SPLIT_LEFT) ||
		(viewport == C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT);
	if(m_bSplitscreen && specialViewport)
	{
		S32 xPos = 0;
		S32 yPos = 0;
		switch( viewport )
		{
		case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
			yPos = (S32)(ui.getScreenHeight() / 2);
			break;
		case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
			xPos = (S32)(ui.getScreenWidth() / 2);
			break;
		}
		ui.setupRenderPosition(xPos, yPos);

		if((viewport == C4JRender::VIEWPORT_TYPE_SPLIT_LEFT) ||	(viewport == C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT))
		{
			// Need to render at full height, but only the left side of the scene
			S32 tileXStart = 0;
			S32 tileYStart = 0;
			S32 tileWidth = width;
			S32 tileHeight = (S32)(ui.getScreenHeight());

			IggyPlayerSetDisplaySize( getMovie(), m_movieWidth, m_movieHeight );

			IggyPlayerDrawTilesStart ( getMovie() );

			m_renderWidth = tileWidth;
			m_renderHeight = tileHeight;
			IggyPlayerDrawTile ( getMovie() ,
				tileXStart ,
				tileYStart ,
				tileXStart + tileWidth ,
				tileYStart + tileHeight ,
				0 );
			IggyPlayerDrawTilesEnd ( getMovie() );
		}
		else
		{
			// Need to render at full height, and full width. But compressed into the viewport
			IggyPlayerSetDisplaySize( getMovie(), ui.getScreenWidth(), ui.getScreenHeight()/2 );
			IggyPlayerDraw( getMovie() );
		}
	}
	else
	{
		UIScene::render(width, height, viewport);
	}
}

void UIComponent_Panorama::setPanorama(bool isDay)
{
	if(isDay != m_bShowingDay)
	{
		m_bShowingDay = isDay;

		IggyDataValue result;
		IggyDataValue value[1];
		value[0].type = IGGY_DATATYPE_boolean;
		value[0].boolval = isDay;

		IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcShowPanoramaDay , 1 , value );
	}
}
