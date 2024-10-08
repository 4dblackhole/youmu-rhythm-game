#pragma once
#include "framework.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/Rectangle2D.h"
#include "Object/Shape2D/DwLayout2D.h"
#include "MusicalObject/Music.h"
#include "MusicalObject/Pattern.h"

class MusicListScroll
{
public:
	MusicListScroll();
	MusicListScroll(float x, float y, float w, float h);
	~MusicListScroll();

	void OnBeginScene();
	void OnEndScene();

	void OnResize(float newW, float newH);

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseWheel(WPARAM wState, int x, int y);

	void Update(float dt);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& cam);

	Music* GetCurrentMusic();
	Pattern* GetCurrentPattern();

private:
	void InitMusicListScroll();
	void CreateMusicBox();

	void ChangePatternBox(size_t musicIdx);

	void LoadMusic();
	void LoadPattern();

	TextureManager textureManager;

	class TextureName
	{
	public:
		DECLARE_VARIABLE_STRING(MusicScroll);
	};
	Sprite scrollImg;
	
	unique_ptr<DwLayout2D> noMusicText;

	vector<Music*> musicList;
	vector<Rectangle2D*> musicBoxList;
	vector<DwLayout2D*> musicTextList;

	map<wstring, size_t> musicIndexMap;

	void ReleasePatternBox();
	vector<Rectangle2D*> patternBoxList;
	vector<DwLayout2D*> patternTextList;

	float scrollPos = 0.0f;
	World2D scrollMatrix;
	World2D patternHeightMatrix;

	void UpdateScrollMatrix();
	void UpdatePatternHeightMatrix(size_t musicIdx);
	void ChangeTargetScrollMatrix();
	void NotifyScrollMatrixUpdate();

	void InitBoxListParentWorld();

	int previousSelectMusic = 0;
	int currentSelectMusic = 0;
	int previousSelectPattern = 0;
	int currentSelectPattern = 0;

	void PlayMusic(size_t idx);
	void StopMusic(size_t idx);

	void ChangeSelectMusic(size_t musicIdx);
	void ChangeSelectPattern(size_t idx);
	int GetAdjustedCurrentPatternIdx() const;
};