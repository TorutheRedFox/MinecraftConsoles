#include "stdafx.h"
#include "Button.h"
#include "OptionsScreen.h"
#include "SelectWorldScreen.h"
#include "JoinMultiplayerScreen.h"
#include "Tesselator.h"
#include "Textures.h"
#include "..\Minecraft.World\StringHelpers.h"
#include "..\Minecraft.World\InputOutputStream.h"
#include "..\Minecraft.World\net.minecraft.locale.h"
#include "..\Minecraft.World\System.h"
#include "..\Minecraft.World\Random.h"
#include "TitleScreen.h"
#include "../Minecraft.World/Calendar.h"
#include "../Minecraft.World/Tile.h"
#include <TileRenderer.h>

Random *TitleScreen::random = new Random();

TitleScreen::TitleScreen()
{
	// 4J - added initialisers
	vo = 0;
	multiplayerButton = nullptr;

    splash = L"missingno";
//   try {	// 4J - removed try/catch
    vector<wstring> splashes;
    
	
    //BufferedReader *br = new BufferedReader(new InputStreamReader(InputStream::getResourceAsStream(L"Common\\res\\title\\splashes.txt"))); //, Charset.forName("UTF-8")
	//
    //if (br)
    //{
    //    wstring line = L"";
    //    while (!(line = br->readLine()).empty())
    //    {
    //        line = trimString(line);
    //        if (line.length() > 0)
    //        {
    //            splashes.push_back(line);
    //        }
    //    }
    //
    //    br->close();
    //    delete br;
    //}

    // use the main file for this
    wstring filename = L"splashes.txt";
    if (app.hasArchiveFile(filename))
    {
        byteArray splashesArray = app.getArchiveFile(filename);
        ByteArrayInputStream bais(splashesArray);
        InputStreamReader isr(&bais);
        BufferedReader br(&isr);

        wstring line = L"";
        while (!(line = br.readLine()).empty())
        {
            line = trimString(line);
            if (line.length() > 0)
            {
                splashes.push_back(line);
            }
        }

        br.close();
    }
    
    
    splash = splashes.at(random->nextInt(splashes.size()));

//    } catch (Exception e) {
//    }
}

void TitleScreen::tick()
{
	vo += 1.0f;

    if (initializedLetterBlocks) // Toru - was null check
    {
        for (int x = 0; x < 39; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                letterBlocks[x][y].tick();
            }
        }
    }

	//if( vo > 100.0f ) minecraft->setScreen(new SelectWorldScreen(this));		// 4J - temp testing
}

void TitleScreen::keyPressed(wchar_t eventCharacter, int eventKey)
{
}

void TitleScreen::init()
{
    // 4J - Implemented in main menu instead
    //Calendar c = Calendar.getInstance();
    //c.setTime(new Date());

    if (Calendar::GetMonth() + 1 == 11 && Calendar::GetDayOfMonth() == 9) {//if (c.get(Calendar.MONTH) + 1 == 11 && c.get(Calendar.DAY_OF_MONTH) == 9) {
        splash = L"Happy birthday, ez!";
    } else if (Calendar::GetMonth() + 1 == 6 && Calendar::GetDayOfMonth() == 1) {// else if (c.get(Calendar.MONTH) + 1 == 6 && c.get(Calendar.DAY_OF_MONTH) == 1) {
        splash = L"Happy birthday, Notch!";
    } else if (Calendar::GetMonth() + 1 == 12 && Calendar::GetDayOfMonth() == 24) {// else if (c.get(Calendar.MONTH) + 1 == 12 && c.get(Calendar.DAY_OF_MONTH) == 24) {
        splash = L"Merry X-mas!";
    } else if (Calendar::GetMonth() + 1 == 1 && Calendar::GetDayOfMonth() == 1) {// else if (c.get(Calendar.MONTH) + 1 == 1 && c.get(Calendar.DAY_OF_MONTH) == 1) {
        splash = L"Happy new year!";
    }

    Language *language = Language::getInstance();

    const int spacing = 24;
    const int topPos = height / 4 + spacing * 2;

    buttons.push_back(new Button(1, width / 2 - 100, topPos, language->getElement(L"menu.singleplayer")));
    buttons.push_back(multiplayerButton = new Button(2, width / 2 - 100, topPos + spacing * 1, language->getElement(L"menu.multiplayer")));
    //buttons.push_back(new Button(3, width / 2 - 100, topPos + spacing * 2, language->getElement(L"menu.mods")));

    if (minecraft->appletMode)
	{
        buttons.push_back(new Button(0, width / 2 - 100, topPos + spacing * 3, language->getElement(L"menu.options")));
    }
	else
	{
        buttons.push_back(new Button(0, width / 2 - 100, topPos + spacing * 3 + 12, 98, 20, language->getElement(L"menu.options")));
        buttons.push_back(new Button(4, width / 2 + 2, topPos + spacing * 3 + 12, 98, 20, language->getElement(L"menu.quit")));
    }

    if (minecraft->user == nullptr)
	{
        multiplayerButton->active = false;
    }

}

void TitleScreen::buttonClicked(Button *button)
{
    if (button->id == 0)
	{
        minecraft->setScreen(new OptionsScreen(this, minecraft->options));
    }
    if (button->id == 1)
	{
        minecraft->setScreen(new SelectWorldScreen(this));
        //ui.NavigateToScene(0, eUIScene_MainMenu);
    }
    if (button->id == 2)
	{
        minecraft->setScreen(new JoinMultiplayerScreen(this));
    }
    if (button->id == 3)
	{
 //       minecraft->setScreen(new TexturePackSelectScreen(this));		// 4J - TODO put back in
    }
    if (button->id == 4)
	{
        minecraft->stop();
    }
}

void TitleScreen::renderPanoramas(int xm, int ym, float a)
{
    Tesselator* t = Tesselator::getInstance();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(120.0f * (float)height / (float)width, (float)width / (float)height, 0.05f, 10.0f);
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
        panoramas[panorama] = minecraft->textures->loadTexture(TN_TITLE_BG_PANORAMA0 + panorama);

    int samples = 8; // 8 originally, runs terrible here for no visual improvement
    for (int l = 0; l < samples * samples; l++) {
        glPushMatrix();
        float x = (((float)(l % samples) / (float)samples) - 0.5F) / 16.0f;
        float y = (((float)(l / samples) / (float)samples) - 0.5F) / 16.0f;
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

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Toru - added these to compensate for lack of blur
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            t->begin();
            t->color(0xffffff, 255 / samples);
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
}

void TitleScreen::renderGaussianBlur(float f)
{
    return;
    // TODO - do something about glCopyTexSubImage2D
    glBindTexture(GL_TEXTURE_2D, minecraft->textures->loadTexture(TN_GUI_BACKGROUND));//blurBuffer);
    //glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 256, 256);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(true, true, true, false);
    Tesselator *t = Tesselator::getInstance();
    t->begin();
    byte blurSamples = 3;
    for (int i = 0; i < blurSamples; i++) {
        t->color(1.0f, 1.0f, 1.0f, 1.0f / (i + 1));
        int x = width;
        int y = height;
        float xOffset = (i - blurSamples / 2) / 256.0f;
        t->vertexUV(x, y, blitOffset, 0.0F + xOffset, 0.0);
        t->vertexUV(x, 0, blitOffset, 1.0F + xOffset, 0.0);
        t->vertexUV(0, 0, blitOffset, 1.0F + xOffset, 1.0);
        t->vertexUV(0, y, blitOffset, 0.0F + xOffset, 1.0);
    }
    
    t->end();
    glColorMask(true, true, true, true);
}

void TitleScreen::renderBackground(int xm, int ym, float a)
{
    //renderDirtBackground(0); // TODO - do panoramas
    glViewport(0, 0, 256, 256); // blur doesn't exist
    renderPanoramas(xm, ym, a);

    for (int i = 0; i < 8; i++)
        renderGaussianBlur(a);

    glViewport(0, 0, minecraft->width, minecraft->height);


    //Tesselator* t = Tesselator::getInstance();
}

void TitleScreen::render(int xm, int ym, float a)
{
	// 4J Unused
//#if 0
    renderBackground(xm, ym, a);

    Tesselator *t = Tesselator::getInstance();

    fillGradient(0, 0, width, height, 0xAAFFFFFF, 0x00FFFFFF); // white
    fillGradient(0, 0, width, height, 0x00000000, 0xAA000000); // black

    //renderMinecraftLogo(a);

    int logoWidth = 155 + 119;
    int logoX = width / 2 - logoWidth / 2;
    int logoY = 30;
    
    //glBindTexture(GL_TEXTURE_2D, minecraft->textures->loadTexture(L"/title/mclogo.png"));
    glBindTexture(GL_TEXTURE_2D, minecraft->textures->loadTexture(TN_TITLE_MCLOGO));
    glColor4f(1, 1, 1, 1);
    blit(logoX + 0, logoY + 0, 0, 0, 155, 44);
    blit(logoX + 155, logoY + 0, 0, 45, 155, 44);
    t->color(0xffffff);
    glPushMatrix();
    glTranslatef((float)width / 2 + 90, 70, 0);
    
    glRotatef(-20, 0, 0, 1);
    float sss = 1.8f - Mth::abs(Mth::sin(System::currentTimeMillis() % 1000 / 1000.0f * PI * 2) * 0.1f);
    
    sss = sss * 100 / (font->width(splash) + 8 * 4);
    glScalef(sss, sss, sss);
    drawCenteredString(font, splash, 0, -8, 0xffff00);
    glPopMatrix();

    //drawString(font, ClientConstants::VERSION_STRING, 2, 2, 0x505050);
    drawString(font, ClientConstants::VERSION_STRING, 2, height - 10, 0xffffff);
    wstring msg = L"Copyright Mojang AB. Do not distribute.";
    drawString(font, msg, width - font->width(msg) - 2, height - 10, 0xffffff);

    Screen::render(xm, ym, a);
//#endif
}

void TitleScreen::renderMinecraftLogo(float a) {
    if (!initializedLetterBlocks) {
        initializedLetterBlocks = true;

        for (int x = 0; x < 39; x++) {
            for (int y = 0; y < 5; y++) {
                letterBlocks[x][y] = TitleScreen::LetterBlock(x, y);
            }
        }
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    ScreenSizeCalculator ssc(minecraft->options, minecraft->width, minecraft->height);
    int height = 120 * ssc.scale;
    gluPerspective(70.0F, (float)minecraft->width / (float)height, 0.05F, 100.0F);
    glViewport(0, minecraft->height - height, minecraft->width, height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthMask(true);
    unique_ptr<TileRenderer> tileRenderer = make_unique<TileRenderer>();

    for (int layer = 0; layer < 3; layer++) {
        glPushMatrix();
        glTranslatef(0.4F, 0.6F, -13.0F);
        if (layer == 0) {
            glClear(GL_DEPTH_BUFFER_BIT);
            glTranslatef(0.0F, -0.4F, 0.0F);
            glScalef(0.98F, 1.0F, 1.0F);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        if (layer == 1) {
            glDisable(GL_BLEND);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        if (layer == 2) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_COLOR, GL_ONE);
        }

        glScalef(1.0F, -1.0F, 1.0F);
        glRotatef(15.0F, 1.0F, 0.0F, 0.0F);
        glScalef(0.89F, 1.0F, 0.4F);
        glTranslatef((float)(-39) * 0.5F, (float)(-5) * 0.5F, 0.0F);
        //glBindTexture(GL_TEXTURE_2D, minecraft->textures->loadTexture(TN_TERRAIN));
        if (layer == 0) {
            //glBindTexture(GL_TEXTURE_2D, this.minecraft.textures.loadTexture("/title/black.png"));
        }

        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 39; x++) {
                char c = logo[y][x];
                if (c != ' ') {
                    glPushMatrix();
                    TitleScreen::LetterBlock &letterBlock = letterBlocks[x][y];
                    float z = (float)(letterBlock.yO + (letterBlock.y - letterBlock.yO) * (double)a);
                    float scale = 1.0F;
                    float alpha = 1.0F;
                    float rotation = 0.0F;
                    if (layer == 0)
                    {
                        scale = z * 0.04F + 1.0F;
                        alpha = 1.0F / scale;
                        z = 0.0F;
                    }

                    glTranslatef((float)x, (float)y, z);
                    glScalef(scale, scale, scale);
                    glRotatef(rotation, 0.0F, 1.0F, 0.0F);
                    tileRenderer->renderCube(Tile::stone, alpha);
                    glPopMatrix();
                }
            }
        }

        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(0, 0, minecraft->width, minecraft->height);
    glEnable(GL_CULL_FACE);
}