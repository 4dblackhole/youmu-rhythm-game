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

	class Name
	{
	public:
		static constexpr LPCSTR test = "test";
		static constexpr LPCSTR Intro = "Intro";
		static constexpr LPCSTR Title = "Title";
	};

private:
	mapSceneList sceneList;
	GameScene* previousScene = nullptr;
	GameScene* currentScene = nullptr;

};

#define SCENEMANAGER SceneManager::GetInstance()