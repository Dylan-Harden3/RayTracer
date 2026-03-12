#pragma once

#include <string>
#include <vector>

class Image {
public:
	Image(int width, int height);
	virtual ~Image();
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void setPixel(int x, int y, float r, float g, float b);
	void writeToFile(const std::string &filename);
	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	int width, height, comp;
	std::vector<unsigned char> pixels;
};
