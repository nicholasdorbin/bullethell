#pragma once
#include "Engine/Core/Memory/ObjectPool.hpp"
typedef unsigned long long int uint64_t;

#include <vector>
#include <string>

class Sample
{
public:
	Sample();
	~Sample();

	char const* m_tag;
	Sample *m_prev, *m_next;
	Sample *m_children;
	Sample *m_parent;
	uint64_t m_start, m_stop;

	void AddChild(Sample* childSample);
};


struct SampleData
{
	const char* m_tag;
	const char* m_parentTag;
	int m_calls;
	int m_level;
	uint64_t m_totalOps;
	double m_totalTime;
	uint64_t m_selfOps;
	double m_selfTime;
	float m_percentOfFrame;
};

class Profiler;

extern Profiler* g_theProfiler;

class Profiler
{
private:
	
	bool m_Enabled;
	uint64_t m_initialOpCount;
	double m_secondsPerOp;
public:

	Sample* m_CurrentSample;
	Sample* m_CurrentFrame;
	Sample* m_PrevFrame;

	bool m_DesiredEnabled;

	bool m_showLiveUpdate;
	bool m_liveUpdateMode; //0 = list, 1 = flat

	ObjectPool<Sample> m_samplePool;



	Profiler();
	~Profiler();

	void ProfileFrameMark();
	void ProfilerSystemStartup();
	void ProfilerSystemShutdown();
	void StartProfilerSample(const char* tag); //Push
	void EndProfilerSample();//Pop
	void Delete(Sample* sampleToDelete);


	uint64_t GetCurrentPerformanceCount();
	double GetPerformanceCountInSeconds(uint64_t& ops);
	double GetTimeOfSample(Sample* sample);
	double GetSampleSelfTime(Sample* sample);
	double GetSampleChildrenTime(Sample* sample);
	uint64_t GetSampleSelfOps(Sample* sample);
	uint64_t GetSampleChildrenOps(Sample* sample);
	double GetTimeOfPrevFrame();
	Sample* GetLastFrame();

	void PrintFrameToLogListView();
	void PrintFrameToLogFlatView();

	std::vector<SampleData> GetAllDataFromPrevFrame(bool isFlatView = false);
	std::vector<std::string> GetLiveFrameStrings();
	SampleData GetSampleData(Sample* sample);
	std::string SampleToString(Sample* sample);
	std::string SampleDataToString(SampleData& data, bool printLevel = true);

	bool IsEnabled() { return m_Enabled; };

	//#define PROFILE_PRINT_SCOPE(tag) ProfilePrintScoped _s_##__LINE__##(tag);
};


