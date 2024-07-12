#pragma once
#include "framework.h"
#include "SingletonBase.h"
#include "System/GameScene.h"

class SceneManager : public SingletonBase<SceneManager>
{
public:
	using mapSceneList = map< string, GameScene* >;
	using mapSceneIter = map< string, GameScene* >::iterator;

public:
	SceneManager();
	virtual ~SceneManager();

	//Add Scene and Init the Scene
	void InitScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene);

	bool AddScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene);
	void ChangeScene(const mapSceneList::key_type& sceneName);

	inline GameScene* GetCurrentScene() { return currentScene; }

#define CREATE_SCENEMANAGER_NAME(x) static constexpr LPCSTR x = #x
	class Name
	{
	public:
		CREATE_SCENEMANAGER_NAME(test);
		CREATE_SCENEMANAGER_NAME(Intro);
		CREATE_SCENEMANAGER_NAME(Title);
		CREATE_SCENEMANAGER_NAME(SelectMusic);
	};

private:
	mapSceneList sceneList;
	GameScene* previousScene = nullptr;
	GameScene* currentScene = nullptr;

};

#define SCENEMANAGER SceneManager::GetInstance()