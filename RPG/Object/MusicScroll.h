#pragma once
#include "framework.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/Rectangle2D.h"
#include "Object/ClipRect.h"
#include "MusicalObject/Music.h"
#include "MusicalObject/Pattern.h"

class MusicScroll
{
public:
	MusicScroll();
	MusicScroll(float x, float y, float w, float h);
	~MusicScroll();

	void OnBeginScene();
	void OnEndScene();

	void OnResize(float newW, float newH);
	void OnMouseWheel(WPARAM wState, int x, int y);

	void Update(float dt);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& cam);


private:
	void World2DResize(float newW,float newH);
	
	void InitMusicScroll();
	void CreateBoxes();
	void CreateTexts();

	void LoadMusic();
	void LoadPattern();

	Sprite scrollImg;
	ClipRect clipArea; //area to draw boxes
	ClipRect textArea; //area to draw texts
	
	unique_ptr<DwLayout> noMusicText;

	vector<Music*> musicList;
	vector<Rectangle2D*> musicBoxList;
	vector<DwLayout*> musicTextList;

	map<wstring, size_t> musicIndexMap;

	int scrollPos = 0;
	D2D1::Matrix3x2F scrollMatrix{ D2D1::Matrix3x2F::Identity() };
	void UpdateScrollMatrix();

	size_t currentSelectMusic = 0;
	size_t prevSelectMusic = 0;
	void ChangePreviewMusic();

	void PlayMusic();
	void StopMusic();

};