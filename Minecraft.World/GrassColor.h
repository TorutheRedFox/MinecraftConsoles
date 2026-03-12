#pragma once

class GrassColor
{
	// 4J Stu - We don't use this any more
	// Toru - Well I do
private:
	static intArray pixels;
public:

	static void init(intArray pixels);
    static int get(double temp, double rain);
};