#pragma once
#include "Screen.h"
class Random;
class Button;
using namespace std;

class TitleScreen : public Screen
{
private:
	class LetterBlock
	{
	public:
		double y = 0.0f;
		double yO = 0.0f;
		double ya = 0.0f;

		LetterBlock() {}; // Toru - added

		LetterBlock(int x, int y)
		{
			yO = (double)(10 + y) + TitleScreen::random->nextDouble() * 32.0 + (double)x;
			y = yO;
		}

		void tick()
		{
			yO = y;
			if (y > 0.0) {
				ya -= 0.6;
			}

			y = y + ya;
			ya *= 0.9;
			if (y < 0.0) {
				y = 0.0;
				ya = 0.0;
			}
		}
	};

	static Random *random;

	char logo[5][39] = 
	{
		" *   * * *   * *** *** *** *** *** ***",
		" ** ** * **  * *   *   * * * * *    * ",
		" * * * * * * * **  *   **  *** **   * ",
		" *   * * *  ** *   *   * * * * *    * ",
		" *   * * *   * *** *** * * * * *    * "
	};

	bool initializedLetterBlocks = false; // Toru - added
	class LetterBlock letterBlocks[39][5];

    float vo;

    wstring splash;
    Button *multiplayerButton;

public:
	TitleScreen();
    virtual void tick();
protected:
	virtual void keyPressed(wchar_t eventCharacter, int eventKey);
public:
	virtual void init();
protected:
	virtual void buttonClicked(Button *button);
protected:
	virtual void renderPanoramas(int xm, int ym, float a);
	virtual void renderGaussianBlur(float a);
	virtual void renderBackground(int xm, int ym, float a);
public:
	virtual void render(int xm, int ym, float a);
protected:
	virtual void renderMinecraftLogo(float a);
};