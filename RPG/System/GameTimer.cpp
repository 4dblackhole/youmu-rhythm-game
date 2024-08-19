#include "framework.h"
#include "GameTimer.h"

GameTimer::GameTimer()
	: mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0),
	mPrevTime(0), mCurrTime(0), mStopTime(0), mStopped(false)
{
	INT64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::TotalTime() const
{
	return
		mStopped
		? (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount)
		: (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	//Get current time from mainboard
	INT64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	/*
	
	chrono::steady_clock::time_point currTime = chrono::high_resolution_clock::now();
	*/

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mPausedTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{	
	if (mStopped)
	{
		INT64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mPausedTime += (currTime - mStopTime);

		mPrevTime = currTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		INT64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}

}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	//Get current time from mainboard
	INT64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;
	
	//time change
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0) mDeltaTime = 0.0;
}
