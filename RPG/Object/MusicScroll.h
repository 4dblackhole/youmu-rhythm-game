#pragma once
#include "framework.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/Rectangle2D.h"
#include "MusicalObject/Music.h"

class MusicScroll
{
public:
	MusicScroll();
	~MusicScroll();

	void OnResize(float newW, float newH);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& cam);

	void LoadMusic();

private:
	void InitMusicScroll();

	Sprite scrollImg;
	unique_ptr<DwLayout> noMusicText;
	vector<Music*> musicList;
	Rectangle2D box;
};