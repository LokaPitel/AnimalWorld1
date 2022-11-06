#pragma once
#include <iostream>
#include <chrono>
#include "conio.h"

class KeyboardUtility
{
	static int previouslyPressedKey;
	static int lastPressedKey;
	static int lastTimePressedKey;

	static int delayBetweenKeyPressing;

public:
	KeyboardUtility() {}

	static void init(int delay)
	{
		previouslyPressedKey = -1;
		lastPressedKey = -1;
		lastTimePressedKey = delay;
		delayBetweenKeyPressing = delay;
	}

	static bool delayIsGone()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - lastTimePressedKey>= delayBetweenKeyPressing;
	}

	static void resetDelay()
	{
		lastTimePressedKey = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	static bool isKeyPressed()
	{
		return _kbhit();
	}

	static void handleKeyboard()
	{
		while (true)
		{
			if (isKeyPressed())
			{
				int pressedKey = _getch();

				if (delayIsGone() || previouslyPressedKey != pressedKey)
				{
					resetDelay();
					
					previouslyPressedKey = lastPressedKey;
					lastPressedKey = pressedKey;

					break;
				}

				else
					continue;
			}
		}
	}

	static bool onEnter()
	{
		return lastPressedKey == 13;
	}

	static bool onEsc()
	{
		return lastPressedKey == 27;
	}

	static bool onW()
	{
		return lastPressedKey == 119;
	}
	
	static bool onS()
	{
		return lastPressedKey == 115;
	}
	
	static bool onA()
	{
		return lastPressedKey == 97;
	}
	
	static bool onD()
	{
		return lastPressedKey == 100;
	}

	static bool onSemicolon()
	{
		return lastPressedKey == 59;
	}
};

int KeyboardUtility::previouslyPressedKey;
int KeyboardUtility::lastPressedKey;
int KeyboardUtility::lastTimePressedKey;

int KeyboardUtility::delayBetweenKeyPressing;