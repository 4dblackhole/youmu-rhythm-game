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

	void OnBeginScene();
	void OnEndScene();

	void OnResize(float newW, float newH);
	void OnMouseWheel(WPARAM wState, int x, int y);

	void Update(float dt);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& cam);

	void LoadMusic();

private:
	void World2DResize(float newW,float newH);
	
	void InitMusicScroll();
	void CreateBoxes();
	void CreateTexts();

	Sprite scrollImg;
	ClipRect clipArea; //area to draw boxes
	ClipRect textArea; //area to draw texts
	
	unique_ptr<DwLayout> noMusicText;

	vector<Music*> musicList;
	vector<Rectangle2D*> musicBoxList;
	vector<DwLayout*> musicTextList;
	unordered_map<wstring, Music*> musicPatternMap;
	unordered_map<wstring, size_t> musicIndexMap;

	int scrollPos = 0;
	D2D1::Matrix3x2F scrollMatrix{ D2D1::Matrix3x2F::Identity() };
	void UpdateScroll();

	size_t currentSelectMusic = 0;
	size_t prevSelectMusic = 0;
	void ChangePreviewMusic();

	void PlayMusic();
	void StopMusic();

};