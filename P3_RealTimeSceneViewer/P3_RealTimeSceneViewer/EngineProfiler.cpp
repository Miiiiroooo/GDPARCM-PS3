#include "EngineProfiler.h"
#include <iostream>

EngineProfiler::EngineProfiler(std::string name) : AUIPanel::AUIPanel(name)
{
}

EngineProfiler::~EngineProfiler()
{
}

void EngineProfiler::draw()
{
	ImGui::SetNextWindowSize(ImVec2(300, 85));
	ImGui::SetNextWindowPos(ImVec2(20, 700));

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoResize;
	ImGui::Begin("GDENG03 Engine Profiler", NULL, windowFlags);

	ImGui::Text("Average time per frame: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(mAverageDeltaTime * 1000.f).c_str());
	ImGui::SameLine();
	ImGui::Text(" ms");

	ImGui::Text("Frames per second: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(mFramesPerSecond).c_str());

	ImGui::Text("Seconds since execution: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(mElapsedTime).c_str());

	ImGui::End();
}

void EngineProfiler::UpdateFPS(float deltaTime)
{
	mFramesSinceLastCalc += 1.f;
	mTimeSinceLastCalc += deltaTime;
	mElapsedTime += deltaTime;
	if (mTimeSinceLastCalc >= 1.f) {
		mAverageDeltaTime = mTimeSinceLastCalc / mFramesSinceLastCalc;
		mFramesPerSecond = 1.f / mAverageDeltaTime;
		mFramesSinceLastCalc = 0.f;
		mTimeSinceLastCalc = 0.f;
	}
}
