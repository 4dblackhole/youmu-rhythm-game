#include "framework.h"
#include "FmodSystem.h"

FmodSystem::FmodSystem()
{
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
    result = FMOD::System_Create(&_system);
    if (result != FMOD_OK) return result;

    result = _system->getVersion(&version);
    if (result != FMOD_OK) return result;

    result = _system->setOutput(outputMode);
    if (result != FMOD_OK) return result;

    result = _system->getNumDrivers(&driveCount);
    if (driveCount == 0)
    {
        if (MessageBox(NULL, _T("No ASIO driver detected."), _T("Buy your own Audio Interface."), MB_YESNO) == IDNO)
        {
            MessageBox(NULL, _T("Buy your own Audio Interface."), _T("Buy your own Audio Interface."), MB_OK);
        }
        return result;
    }

    result = _system->setDSPBufferSize(bufferlength, 4);

    result = _system->init(256, FMOD_INIT_NORMAL, extradriverdata);
    if (result != FMOD_OK) return result;

    result = _system->getDriver(&currentDriveIdx);

    return result;
}

FMOD_RESULT FmodSystem::Release()
{
    FMOD_RESULT result = FMOD_OK;
    result = _system->close();
    result = _system->release();

    return result;
}

FMOD_RESULT FmodSystem::ChangeDrive(int idx)
{
    FMOD_RESULT result = FMOD_OK;
    result = _system->setDriver(idx);
    
    return result;
}

vector<wstring> FmodSystem::EnumDriverList()
{
    vector<wstring> result;
    char tempDriveName[256];
    result.reserve(driveCount);
    for (int i = 0; i < driveCount; ++i)
    {
        _system->getDriverInfo(i, (char*)tempDriveName, sizeof(tempDriveName), 0, 0, 0, 0);
        result.emplace_back(ShortCut::UTF8ToWstring(tempDriveName));
    }

    return result;
}
