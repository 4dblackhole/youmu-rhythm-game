#pragma once
#include "framework.h"
#include "SingletonBase.h"

class GameScene;

class SceneManager : public SingletonBase<SceneManager>
{
public:
	using mapSceneList = map<string, GameScene*>;
	using mapSceneIter = map<string, GameScene*>::iterator;

public:
	SceneManager() {};
	virtual ~SceneManager();

	void InitScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene);
	void AddScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene);
	void ChangeScene(const mapSceneList::key_type& sceneName);

	inline GameScene* GetCurrentScene() { return currentScene; }

private:
	mapSceneList sceneList;
	GameScene* previousScene = nullptr;
	GameScene* currentScene = nullptr;

};

#define SCENEMANAGER SceneManager::GetInstance()