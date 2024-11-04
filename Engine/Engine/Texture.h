#pragma once

typedef unsigned int GLuint;

class Texture
{
public:
	Texture(GLuint id, int width, int height, const char* path = nullptr);
	~Texture();

public:
	GLuint textureId;

	int textureWidth;
	int textureHeight;	
		
	char* texturePath;
};