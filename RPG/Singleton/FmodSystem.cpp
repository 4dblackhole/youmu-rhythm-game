#include "framework.h"
#include "FmodSystem.h"

FmodSystem::FmodSystem()
{
    Init(FMOD_OUTPUTTYPE_AUTODETECT, 64);
    //Init(FMOD_OUTPUTTYPE_ASIO, 64);
    InitSounds();
}

FmodSystem::~FmodSystem()
{
    Release();
    TRACE(_T("FmodSystem Release\n"));
}

FMOD_RESULT FmodSystem::Init(FMOD_OUTPUTTYPE outputMode, unsigned int bufferlength)
{
    FMOD_RESULT result = FMOD_OK;
    //FMOD Init
    result = FMOD::System_Create(&mSystem);
    if (result != FMOD_OK) return result;

    result = mSystem->getVersion(&version);
    if (result != FMOD_OK) return result;

    result = mSystem->setOutput(outputMode);
    if (result != FMOD_OK) return result;

    result = mSystem->getNumDrivers(&driveCount);
    if (driveCount == 0)
    {
        if (MessageBox(NULL, _T("No ASIO driver detected."), _T("Buy your own Audio Interface."), MB_YESNO) == IDNO)
        {
            MessageBox(NULL, _T("Buy your own Audio Interface."), _T("Buy your own Audio Interface."), MB_OK);
        }
        return result;
    }

    result = mSystem->setDSPBufferSize(bufferlength, 4);

    result = mSystem->init(256, FMOD_INIT_NORMAL, extradriverdata);
    if (result != FMOD_OK) return result;

    result = mSystem->getDriver(&currentDriveIdx);

    return result;
}

FMOD_RESULT FmodSystem::Release()
{
    FMOD_RESULT result = FMOD_OK;

    for (auto& it : defaultSoundList)
    {
        it.second.second->stop(); //channel
        it.second.first->release(); //sound
    }

    result = mSystem->close();
    result = mSystem->release();

    return result;
}

FMOD_RESULT FmodSystem::ChangeDrive(int idx)
{
    FMOD_RESULT result = FMOD_OK;
    result = mSystem->setDriver(idx);
    
    return result;
}

FMOD_RESULT FmodSystem::ChangeDriveOutputType(FMOD_OUTPUTTYPE type)
{
    FMOD_RESULT result = FMOD_OK;
    FMOD_OUTPUTTYPE currentType{};
    mSystem->getOutput(&currentType);

    if (type == currentType) return result;

    result = mSystem->setOutput(type);

    return result;
}

vector<wstring> FmodSystem::EnumDriverList()
{
    vector<wstring> result;
    char tempDriveName[256];
    result.reserve(driveCount);
    for (int i = 0; i < driveCount; ++i)
    {
        mSystem->getDriverInfo(i, (char*)tempDriveName, sizeof(tempDriveName), 0, 0, 0, 0);
        result.emplace_back(ShortCut::UTF8ToWstring(tempDriveName));
    }

    return result;
}


FMOD::Sound* FmodSystem::GetSound(const string& str)
{
    // sound search
    auto it = defaultSoundList.find(str);
    if (it != defaultSoundList.end()) return it->second.first;
    else return nullptr;
}

FMOD::Channel*& FmodSystem::GetChannel(const string& str)
{
    // channel search
    auto it = defaultSoundList.find(str);
    return it->second.second;

}

void FmodSystem::Play(const string& str)
{
    FMODSYSTEM.System()->playSound(FMODSYSTEM.GetSound(str), nullptr, false, &(FMODSYSTEM.GetChannel(str)));
}

void FmodSystem::Stop(const string& str)
{
    FMODSYSTEM.GetChannel(str)->stop();
}

void FmodSystem::InitSounds()
{
    FMOD::Sound* tempSound;

    const string& systemSoundDirU8 = ShortCut::WstringToUTF8(SystemSoundDir);

    mSystem->createStream((systemSoundDirU8 + "button01a.mp3").c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &tempSound);
    defaultSoundList.emplace(make_pair(Name::button01a, make_pair(tempSound, nullptr)));

    mSystem->createStream((systemSoundDirU8 + "select05.mp3").c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &tempSound);
    defaultSoundList.emplace(make_pair(Name::select05, make_pair(tempSound, nullptr)));

}
