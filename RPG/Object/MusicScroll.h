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
	void CreateMusicBoxes();

	void CreatePatternBoxes();

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

	vector<Rectangle2D*> patternBoxList;
	vector<DwLayout*> patternTextList;

	Rectangle2D patternBox;

	float scrollPos = 0.0f;
	D2D1::Matrix3x2F scrollMatrix{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F patternHeightMatrix{ D2D1::Matrix3x2F::Identity() };
	D2D1::Matrix3x2F scrollWithPatternMatrix{ D2D1::Matrix3x2F::Identity() };

	void UpdateScrollMatrix();
	void UpdatePatternHeightMatrix(size_t musicIdx);
	void ChangeTargetScrollMatrix();
	void NotifyScrollMatrixUpdate();

	void InitBoxListParentWorld();

	size_t previousSelectMusic = 0;
	size_t currentSelectMusic = 0;
	size_t currentSelectPattern = 0;

	void PlayMusic(size_t idx);
	void StopMusic(size_t idx);

	void ChangeSelectMusic(size_t newIdx);
};