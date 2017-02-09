#include "Engine/Core/Memory/Profiler.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineBuildConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <functional>

#pragma warning( disable : 4127 )  // conditional expression is constant


Profiler* g_theProfiler = nullptr;


#ifdef PROFILER

Profiler::Profiler()
	: m_samplePool(10240)
	, m_PrevFrame(nullptr)
	, m_CurrentFrame(nullptr)
	, m_CurrentSample(nullptr)
	, m_DesiredEnabled(false)
	, m_Enabled(false)
	, m_showLiveUpdate(false)
	, m_liveUpdateMode(false)
{

}

Profiler::~Profiler()
{

}

void Profiler::ProfileFrameMark()
{
	//Marks the start of the frame
	//Set prev frame as the current frame
	//Set current frame to new frame

	if(m_Enabled)
	{
		//FrameEnd
		ASSERT_OR_DIE(m_CurrentFrame == m_CurrentSample, "Current sample is not the top of the frame.");
		EndProfilerSample();
		Delete(m_PrevFrame); //Delete will delete the children before deleting itself
		m_PrevFrame = m_CurrentFrame;
		
	}

	m_Enabled = m_DesiredEnabled;
	if(m_Enabled) 
	{
		//To not profile here, get prev state and current state and see if they are different
		//FrameStart
		StartProfilerSample("Frame");
		m_CurrentFrame = m_CurrentSample;
	}

}


void Profiler::ProfilerSystemStartup()
{
	LARGE_INTEGER initialOpCount;
	QueryPerformanceCounter(&initialOpCount);
	m_initialOpCount = initialOpCount.QuadPart;
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency(&countsPerSecond);
	m_secondsPerOp = 1.0 / static_cast< double >(countsPerSecond.QuadPart);
	m_DesiredEnabled = true;
}

void Profiler::ProfilerSystemShutdown()
{
	//#TODO Implement. Free the ObjectPool.
	m_DesiredEnabled = false;
	m_samplePool.Destroy();
}

void Profiler::StartProfilerSample(const char* tag)
{
	if (!m_Enabled)
		return;
	//Create new sample, add to current node, make current
	Sample* newSample = m_samplePool.Alloc();
	newSample->m_tag = tag;
	newSample->m_start = GetCurrentPerformanceCount();

	if (m_CurrentSample == nullptr)
	{
		m_CurrentSample = newSample;
	}
	else
	{
		m_CurrentSample->AddChild(newSample);
		m_CurrentSample = newSample;
	}
}

void Profiler::EndProfilerSample()
{
	if (!m_Enabled)
		return;
	//End sample, Set cur sample to sample parent
	ASSERT_OR_DIE(m_CurrentSample != nullptr, "ERROR: Attempting to end a sample that doesn't exist.");
	m_CurrentSample->m_stop = GetCurrentPerformanceCount();

	m_CurrentSample = m_CurrentSample->m_parent;
}

void Profiler::Delete(Sample* sampleToDelete)
{
	if (sampleToDelete == nullptr)
		return;
	m_samplePool.Delete(sampleToDelete);
}

uint64_t Profiler::GetCurrentPerformanceCount()
{
	LARGE_INTEGER currentOpCount;
	QueryPerformanceCounter(&currentOpCount);
	return currentOpCount.QuadPart - m_initialOpCount;

}

double Profiler::GetPerformanceCountInSeconds(uint64_t& ops)
{
	LONGLONG elapsedCountsSinceInitialTime = ops;


	double currentSeconds = static_cast<double>(elapsedCountsSinceInitialTime) * m_secondsPerOp;
	return currentSeconds;
}

double Profiler::GetTimeOfSample(Sample* sample)
{
	uint64_t opsElapsed = sample->m_stop - sample->m_start;

	return GetPerformanceCountInSeconds(opsElapsed);
}

double Profiler::GetSampleSelfTime(Sample* sample)
{
	double childrenTime = GetSampleChildrenTime(sample);

	double totalSampletime = GetTimeOfSample(sample);

	double selfTime = totalSampletime - childrenTime;

	return selfTime;
}

double Profiler::GetSampleChildrenTime(Sample* sample)
{
	ASSERT_OR_DIE(sample != nullptr, "Attempting to get time of null sample.");
	Sample* currentSample = sample->m_children;

	double totalChildTime = 0;
	while (currentSample != nullptr)
	{
		double childTime = GetTimeOfSample(currentSample);
		totalChildTime += childTime;
		currentSample = currentSample->m_next;
	}
	return totalChildTime;
}

uint64_t Profiler::GetSampleSelfOps(Sample* sample)
{
	uint64_t childrenOps = GetSampleChildrenOps(sample);

	uint64_t totalSampleOps = sample->m_stop - sample->m_start;

	uint64_t selfOps = totalSampleOps - childrenOps;

	return selfOps;
}

uint64_t Profiler::GetSampleChildrenOps(Sample* sample)
{
	ASSERT_OR_DIE(sample != nullptr, "Attempting to get time of null sample.");
	Sample* currentSample = sample->m_children;

	uint64_t totalChildOps = 0;
	while (currentSample != nullptr)
	{
		uint64_t childOps = currentSample->m_stop - currentSample->m_start;
		totalChildOps += childOps;
		currentSample = currentSample->m_next;
	}
	return totalChildOps;
}

double Profiler::GetTimeOfPrevFrame()
{
	if (m_PrevFrame != nullptr)
	{
		return GetTimeOfSample(m_PrevFrame);
	}
	else
	{
		return 0;
	}
}

Sample* Profiler::GetLastFrame()
{
	return m_PrevFrame;
}

void Profiler::PrintFrameToLogListView()
{
	//Console Command
	std::vector<SampleData> m_dataToPrint;

	std::string headerStr = "TAG    CALLS    TIME    SELF TIME    PERCENT FRAME TIME";

	

	//Tag, Calls, Time (Cycles, then ms), Self Time, Percent Frame Time
	m_dataToPrint = GetAllDataFromPrevFrame(false);



	g_theConsole->ConsolePrint(headerStr);
	for each (SampleData sampleData in m_dataToPrint)
	{
		std::string dataStr = SampleDataToString(sampleData);

		g_theConsole->ConsolePrint(dataStr);
	}

	g_theConsole->ConsolePrint("End Frame", Rgba::GREEN);
}

void Profiler::PrintFrameToLogFlatView()
{
	std::vector<SampleData> m_dataToPrint;

	std::string headerStr = "TAG    CALLS    TIME    SELF TIME    PERCENT FRAME TIME";

	std::multimap < float, SampleData, std::greater<float>> m_dataMap;

	//Tag, Calls, Time (Cycles, then ms), Self Time, Percent Frame Time
	m_dataToPrint = GetAllDataFromPrevFrame(true);



	g_theConsole->ConsolePrint(headerStr);
	for each (SampleData sampleData in m_dataToPrint)
	{

		m_dataMap.insert(std::pair<float, SampleData> (sampleData.m_percentOfFrame, sampleData));

		
	}


	for (auto dataItem : m_dataMap)
	{
		std::string dataStr = SampleDataToString(dataItem.second, false);

		g_theConsole->ConsolePrint(dataStr);
	}

	g_theConsole->ConsolePrint("End Frame", Rgba::GREEN);
}

std::vector<SampleData> Profiler::GetAllDataFromPrevFrame(bool isFlatView /*= false*/)
{
	Sample* prevFrame = GetLastFrame();

	Sample* currentSample = prevFrame;


	std::vector<SampleData> m_dataToPrint;

	//Tag, Calls, Time (Cycles, then ms), Self Time, Percent Frame Time
	int treeLevel = 0;
	while (currentSample != nullptr)
	{
		SampleData data = GetSampleData(currentSample);

		bool dataFound = false;
		Sample* parent = currentSample->m_parent;
		for (auto tagIter = m_dataToPrint.begin(); tagIter != m_dataToPrint.end(); ++tagIter)
		{
			if (parent == nullptr)
			{
				break;
			}
			if (tagIter->m_tag == currentSample->m_tag)
			{
				if (!isFlatView && tagIter->m_parentTag == currentSample->m_parent->m_tag)
				//We care about name & parent matching. If they match add foundData to this sample's Data
				tagIter->m_totalOps += data.m_totalOps;
				tagIter->m_totalTime += data.m_totalTime;
				tagIter->m_selfOps += data.m_selfOps;
				tagIter->m_selfTime += data.m_selfTime;
				tagIter->m_calls++;

				double frameTime = GetTimeOfSample(prevFrame);

				double percent = (tagIter->m_totalTime / frameTime) * 100;

				tagIter->m_percentOfFrame = (float)percent;

				dataFound = true;
			}
		}

		if (!dataFound)
		{
			data.m_level = treeLevel;
			m_dataToPrint.push_back(data);
		}

		if (currentSample->m_children != nullptr)
		{
			currentSample = currentSample->m_children;
			treeLevel++;
		}
		else if (currentSample->m_next != nullptr)
		{
			currentSample = currentSample->m_next;
		}
		else
		{
			currentSample = currentSample->m_parent;
			Sample* nextSample = currentSample;
			if (currentSample != nullptr)
			{
				//Set ourselves to the next non-read node (as we already read the parent)
				while (nextSample != nullptr) //in case we reach the top
				{
					if (nextSample->m_next != nullptr)
					{
						currentSample = nextSample->m_next;
						break;
					}
					else
					{
						nextSample = nextSample->m_parent;
					}
					currentSample = nextSample;
				}
			}
			treeLevel--;
		}



	}


	return m_dataToPrint;
}

std::vector<std::string> Profiler::GetLiveFrameStrings()
{
	std::vector<std::string> linesToShow;
	if (!m_showLiveUpdate)
		return linesToShow;

	std::vector<SampleData> dataToShow = GetAllDataFromPrevFrame(m_liveUpdateMode);

	std::string headerStr = "TAG    CALLS    TIME    SELF TIME    PERCENT FRAME TIME";

	linesToShow.push_back(headerStr);


	if (m_liveUpdateMode == 0) //List Mode
	{
		for each (SampleData sampleData in dataToShow)
		{
			std::string dataStr = SampleDataToString(sampleData);

			linesToShow.push_back(dataStr);
		}
	}
	else //Flat Mode
	{
		std::multimap < float, SampleData, std::greater<float>> m_dataMap;
		for each (SampleData sampleData in dataToShow)
		{

			m_dataMap.insert(std::pair<float, SampleData>(sampleData.m_percentOfFrame, sampleData));


		}


		for (auto dataItem : m_dataMap)
		{
			std::string dataStr = SampleDataToString(dataItem.second, false);

			linesToShow.push_back(dataStr);
		}
	}
	linesToShow.push_back("End Frame");
	return linesToShow;
}

std::string Profiler::SampleToString(Sample* sample)
{
	
	std::string tagStr = sample->m_tag;

	uint64_t ops = sample->m_stop - sample->m_start;
	double time = GetTimeOfSample(sample);

	float timeF = static_cast<float>(time);
	std::string timeOpsStr = Stringf("%u ", ops);

	std::string timeSecsStr = Stringf("( %.3f secs)", timeF);
	std::string timeStr = timeOpsStr + timeSecsStr;

	double selfTime = GetSampleSelfTime(sample);

	std::string selfTimeStr = Stringf("%.5f secs", selfTime);

	double frameTime = GetTimeOfSample(m_PrevFrame);

	double percent = (time / frameTime) * 100;

	std::string percentStr = Stringf("%.2f", percent);



	std::string resultString = tagStr + "    " + timeStr + "    " + selfTimeStr + "    " + percentStr;

	return resultString;
}

std::string Profiler::SampleDataToString(SampleData& data, bool printLevel)
{

	std::string levelStr = "";
	if (printLevel)
	{
		if (data.m_level > 15)
		{
			levelStr = "--X--";
		}
		else
		{
			for (int i = 0; i < data.m_level; i++)
			{
				levelStr += "- ";
			}
		}
	}
	
	
	std::string tagStr = data.m_tag;

	std::string callsStr = Stringf("%i ", data.m_calls);


	std::string timeOpsStr = Stringf("%u ", data.m_totalOps);

	float timeF = static_cast<float>(data.m_totalTime * 1000);
	std::string timeSecsStr = Stringf("( %.3f ms)", timeF);
	std::string timeStr = timeOpsStr + timeSecsStr;

	float selfTimeF = static_cast<float>(data.m_selfTime * 1000);

	std::string selfOpsStr = Stringf("%u ", data.m_selfOps);
	std::string selfTimeSecsStr = Stringf("(%.5f ms)", selfTimeF);

	std::string selfTimeStr = selfOpsStr + selfTimeSecsStr;

	std::string percentStr = Stringf("%.2f %%", data.m_percentOfFrame);

	std::string resultString = levelStr + tagStr + "    "  + callsStr + "    "  + timeStr + "    " + selfTimeStr + "    " + percentStr;

	return resultString;
}

SampleData Profiler::GetSampleData(Sample* sample)
{
	SampleData data;
	data.m_tag = sample->m_tag;

	if (sample->m_parent != nullptr)
	{
		data.m_parentTag = sample->m_parent->m_tag;
	}
	else
	{
		data.m_parentTag = nullptr;
	}

	data.m_calls = 1;

	uint64_t ops = sample->m_stop - sample->m_start;
	data.m_totalOps = ops;

	double time = GetTimeOfSample(sample);
	data.m_totalTime = time;

	double selfTime = GetSampleSelfTime(sample);
	data.m_selfTime = selfTime;

	data.m_selfOps = GetSampleSelfOps(sample);

	double frameTime = GetTimeOfSample(m_PrevFrame);
	double percent = (time / frameTime) * 100;
	data.m_percentOfFrame = (float)percent;

	data.m_level = 0;

	return data;
}

Sample::Sample()
	: m_children(nullptr)
	, m_parent(nullptr)
	, m_next(nullptr)
	, m_prev(nullptr)
	, m_start(0)
	, m_stop(0)
{

}

Sample::~Sample()
{
	//delete all the children
	while (m_children != nullptr)
	{
		g_theProfiler->Delete(m_children);
		m_children = m_children->m_next;
	}
}

void Sample::AddChild(Sample* childSample)
{
	childSample->m_parent = g_theProfiler->m_CurrentSample;

	Sample* parentChildPointer = g_theProfiler->m_CurrentSample->m_children;

	if (parentChildPointer == nullptr)
	{
		g_theProfiler->m_CurrentSample->m_children = childSample;
	}
	else
	{
		while (true)
		{
			if (parentChildPointer->m_next == nullptr)
			{
				parentChildPointer->m_next = childSample;
				childSample->m_prev = parentChildPointer;
				break;
			}
			parentChildPointer = parentChildPointer->m_next;
		}
	}
}


CONSOLE_COMMAND(profiler_print_frame)
{
	args;
	if (args.m_argList.size() == 0)
	{
		g_theProfiler->PrintFrameToLogListView();
	}

	else if (args.m_argList.size() < 2)
	{
		std::string type = args.m_argList[0];

		if (type == "flat")
		{
			g_theProfiler->PrintFrameToLogFlatView();
		}
		else
		{
			g_theProfiler->PrintFrameToLogListView();
		}

	}
	
}


CONSOLE_COMMAND(profiler_toggle)
{
	args;
	g_theProfiler->m_DesiredEnabled = !g_theProfiler->m_DesiredEnabled;
}

CONSOLE_COMMAND(profiler_live_frame_toggle)
{
	args;
	g_theProfiler->m_showLiveUpdate = !g_theProfiler->m_showLiveUpdate;
}

CONSOLE_COMMAND(profiler_live_mode_toggle)
{
	args;
	g_theProfiler->m_liveUpdateMode = !g_theProfiler->m_liveUpdateMode;
}


#else
Profiler::Profiler() : m_samplePool(1024) {}
Profiler::~Profiler() {};
void Profiler::ProfileFrameMark() {}
void Profiler::ProfilerSystemStartup() {}
void Profiler::ProfilerSystemShutdown() {}
void Profiler::StartProfilerSample(const char* tag) {} //Push
void Profiler::EndProfilerSample() {}//Pop
void Profiler::Delete(Sample* sampleToDelete) {}


uint64_t Profiler::GetCurrentPerformanceCount() { return 0; }
double Profiler::GetPerformanceCountInSeconds(uint64_t& ops) { return 0; }
double Profiler::GetTimeOfSample(Sample* sample) { return 0; }
double Profiler::GetTimeOfPrevFrame() { return 0; }
Sample* Profiler::GetLastFrame() { return nullptr; }
#endif

