#include "framework.h"
#include "NoteFactory.h"
#include "GameScene/PlayScene.h"

#include "MusicalObject/NoteRelated/Note/NormalNote.h"
#include "MusicalObject/NoteRelated/Note/BigNote.h"

NoteObject* NoteFactory::CreateTaikoNote(UINT noteType, UINT actionType, const MusicalNote* p, const chrono::microseconds t)
{
	switch (noteType)
	{
	case (UINT)PlayScene::TaikoNoteType::Don:
	case (UINT)PlayScene::TaikoNoteType::Kat:
		return new NormalNote{ p, t };
	case (UINT)PlayScene::TaikoNoteType::BigDon:
	case (UINT)PlayScene::TaikoNoteType::BigKat:
		return new BigNote{ p, t };
	default:
		return nullptr;
	}
}
