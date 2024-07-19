#pragma once
#include "framework.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/Rectangle2D.h"
#include "Object/ClipRect.h"
#include "MusicalObject/Music.h"

class MusicScroll
{
public:
	MusicScroll();
	MusicScroll(float x, float y, float w, float h);
	~MusicScroll();

	void OnResize(float newW, float newH);
	void OnMouseWheel(WPARAM wState, int x, int y);

	void Update(float dt);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& cam);

	void LoadMusic();

private:
	void InitMusicScroll();
	void CreateBoxes();
	void CreateTexts();

	Sprite scrollImg;
	ClipRect clipArea; //area to draw boxes
	
	unique_ptr<DwLayout> noMusicText;

	vector<Music*> musicList;
	vector<Rectangle2D*> boxList;
	vector<DwLayout*> textList;

	size_t scrollPos = 0;
	D2D1::Matrix3x2F scrollMatrix{ D2D1::Matrix3x2F::Identity() };

};