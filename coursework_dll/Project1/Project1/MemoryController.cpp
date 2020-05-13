#define _CRT_SECURE_NO_WARNINGS
#include "MemoryController.h"
#include "Windows.h"
#include <TlHelp32.h>
#include <string>

#define BUFF_SIZE 100

JNIEXPORT jobject JNICALL Java_MemoryController_globalMemoryStatus
(JNIEnv * env, jclass) {
	jobject memoryState = env->AllocObject(env->FindClass("MemoryStatus"));
	MEMORYSTATUSEX state;
	ZeroMemory(&state, sizeof(state));
	state.dwLength = sizeof(state);
	GlobalMemoryStatusEx(&state);
	//char buff[BUFF_SIZE];
	std::string smemload("" + std::to_string(state.dwMemoryLoad));
	std::string stotalPhys("" + std::to_string(state.ullTotalPhys));
	std::string savailPhys("" + std::to_string(state.ullAvailPhys));
	std::string stotalVirtual("" + std::to_string(state.ullTotalVirtual));
	std::string savailVirtual("" + std::to_string(state.ullAvailVirtual));
	std::string stotalPageFile("" + std::to_string(state.ullTotalPageFile));
	std::string savailPageFile("" + std::to_string(state.ullAvailPageFile));
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "smemoryLoad", "Ljava/lang/String;"), env->NewStringUTF(smemload.c_str()));
	//ZeroMemory(buff, BUFF_SIZE);
	//_itoa(state.ullTotalPhys, buff, 10);
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "stotalPhys", "Ljava/lang/String;"), env->NewStringUTF(stotalPhys.c_str()));
	///ZeroMemory(buff, BUFF_SIZE);
	//_itoa(state.ullAvailPhys, buff, 10);
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "savailPhys", "Ljava/lang/String;"), env->NewStringUTF(savailPhys.c_str()));
	//ZeroMemory(buff, BUFF_SIZE);
	//_itoa(state.ullTotalVirtual, buff, 10);
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "stotalVirtual", "Ljava/lang/String;"), env->NewStringUTF(stotalVirtual.c_str()));
	//ZeroMemory(buff, BUFF_SIZE);
	//_itoa(state.ullAvailVirtual, buff, 10);
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "savailVirtual", "Ljava/lang/String;"), env->NewStringUTF(savailVirtual.c_str()));
	//ZeroMemory(buff, BUFF_SIZE);
	//_itoa(state.ullTotalPageFile, buff, 10);
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "stotalPageFile", "Ljava/lang/String;"), env->NewStringUTF(stotalPageFile.c_str()));
	//ZeroMemory(buff, BUFF_SIZE);
	//_itoa(state.ullAvailPageFile, buff, 10);
	env->SetObjectField(memoryState, env->GetFieldID(env->GetObjectClass(memoryState), "savailPageFile", "Ljava/lang/String;"), env->NewStringUTF(savailPageFile.c_str()));
	jmethodID init = env->GetMethodID(env->FindClass("MemoryStatus"), "init", "()V");
	env->CallVoidMethod(memoryState, init);
	return memoryState;
}

JNIEXPORT jobject JNICALL Java_MemoryController_getProcesses
(JNIEnv *env, jclass, jobject processList) {
	jmethodID addObject = env->GetMethodID(env->GetObjectClass(processList), "add", "(Ljava/lang/Object;)Z");
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);
	Process32First(hSnapshot, &process);
	do {
		jobject procInfo = env->AllocObject(env->FindClass("ProcessInfo"));
		env->SetIntField(procInfo, env->GetFieldID(env->GetObjectClass(procInfo), "id", "I"), process.th32ProcessID);
		env->SetObjectField(procInfo, env->GetFieldID(env->GetObjectClass(procInfo), "name", "Ljava/lang/String;"), env->NewStringUTF(process.szExeFile));
		env->CallBooleanMethod(processList, addObject, procInfo);
	} while (Process32Next(hSnapshot, &process));
	CloseHandle(hSnapshot);
	return processList;
}


JNIEXPORT jobject JNICALL Java_MemoryController_getMemoryMap
(JNIEnv *env, jclass, jobject map, jint processId) {
	jmethodID addObject = env->GetMethodID(env->GetObjectClass(map), "add", "(Ljava/lang/Object;)Z");

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
	MEMORY_BASIC_INFORMATION memory;
	size_t address = 0;
	ZeroMemory(&memory, sizeof(memory));
	while (VirtualQueryEx(hProcess, (void*)address, &memory, sizeof(memory))) {
		jobject memInfo = env->AllocObject(env->FindClass("MemoryMap"));
		env->SetLongField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "allocationBase", "J"), (long)memory.AllocationBase);
		env->SetIntField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "allocationProtect", "I"), memory.AllocationProtect);
		env->SetLongField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "baseAddress", "J"), (long)memory.BaseAddress);
		env->SetIntField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "protect", "I"), memory.Protect);
		env->SetIntField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "regionSize", "I"), memory.RegionSize);
		env->SetIntField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "state", "I"), memory.State);
		env->SetIntField(memInfo, env->GetFieldID(env->GetObjectClass(memInfo), "type", "I"), memory.Type);
		env->CallBooleanMethod(map, addObject, memInfo);
		address = (size_t)(((char*)address) + memory.RegionSize);
	}
	CloseHandle(hProcess);
	return map;
}