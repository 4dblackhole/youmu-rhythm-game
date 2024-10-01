#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer() {}

	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double mSecondsPerCount;
	double mDeltaTime;

	INT64 mBaseTime;
	INT64 mPausedTime;
	INT64 mStopTime;
	INT64 mPrevTime;
	INT64 mCurrTime;

	bool mStopped;
};

