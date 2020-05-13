#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <time.h>
#include <minwinbase.h>
#include <tlhelp32.h>
#include <vector>
#include <thread>


#pragma comment(lib, "Ws2_32.lib")

std::string command;

std::vector<int> cTPriors;
std::vector<int> s1TPriors;
std::vector<int> s2TPriors;



void* filePointer;
DWORD WINAPI MyThreadFunction(LPVOID lpParam);

HANDLE clientEvent;
HANDLE server1Event;
HANDLE server2Event;

HANDLE clientProcess;
HANDLE server2Process;
HANDLE server1Process;

int clientID;
int server2ID;
int server1ID;


int main(int argc, char* argv[])
{
	//startup routine
	std::cout << "Server 2 application\n" << "Checking if filemap exists... ";
	HANDLE filemap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "coursework_map");
	if (filemap == NULL)
	{
		std::cout << "FAILED! Creating filemap...";
		LPCSTR fileName = "coursework";
		HANDLE file = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		filemap = CreateFileMappingA(file,
			NULL, PAGE_READWRITE, 0, 2048, "coursework_map");
	}
	std::cout << "SUCCESS!\n";
	filePointer = (char*)MapViewOfFile(filemap,
		FILE_MAP_ALL_ACCESS, 0, 0, 2048);
	server2Event = CreateEvent(NULL, TRUE, FALSE, "server2");
	std::cout << "Trying to connect to client...\n";
	clientEvent = OpenEvent(EVENT_ALL_ACCESS, false, "client");
	if (clientEvent == NULL) {
		std::cout << "FAILED! Waiting for client to launch...\n";
		while (clientEvent == NULL)
		{
			clientEvent = OpenEvent(EVENT_ALL_ACCESS, false, "client");
		}
	}
	std::cout << "Connected to client.\n";
	std::cout << "Trying to connect to server1...\n";
	server1Event = OpenEvent(EVENT_ALL_ACCESS, false, "server1");
	if (server1Event == NULL) {
		std::cout << "FAILED! Waiting for server1 to launch...\n";
		while (server1Event == NULL)
		{
			server1Event = OpenEvent(EVENT_ALL_ACCESS, false, "server1");
		}
	}
	std::cout << "Connected to server1.\n";
	LPDWORD id = new DWORD();
	HANDLE dt = CreateThread(NULL, 0, MyThreadFunction, 0, 0, id);
	SetThreadPriority(dt, THREAD_PRIORITY_HIGHEST);
	std::cout << "Created thread with delta-priority: "<< GetThreadPriority(dt) << " and ID: " << *id << std::endl;
	do
	{
		std::cout << "Waiting for commands...\n";
		WaitForSingleObject(server2Event, -1);
		ResetEvent(server2Event);
		command = (char*)filePointer;
		std::cout << "Recieved command: ";
		std::cout << command.c_str() << std::endl;
		if (command == "priorities")
		{
			char* chp = (char*)filePointer;
			chp += 11;
			clientID = *(int*)chp;
			clientProcess = OpenProcess(PROCESS_ALL_ACCESS, false, clientID);
			server2Process = GetCurrentProcess();
			server2ID = GetCurrentProcessId();
			ZeroMemory(filePointer, strlen((char*)filePointer) + 1 + sizeof(int));
			strcpy((char*)filePointer, "getid");
			SetEvent(server1Event);
			WaitForSingleObject(server2Event,-1);
			ResetEvent(server2Event);
			server1ID = *(int*)filePointer;
			ZeroMemory(filePointer, sizeof(int) * 3);
			server1Process = OpenProcess(PROCESS_ALL_ACCESS, false, server1ID);
			int* p = (int*)filePointer;
			*p = GetPriorityClass(clientProcess);
			p++;
			*p = GetPriorityClass(server1Process);
			p++;
			*p = GetPriorityClass(server2Process);
			std::cout << "Sent priorities information" << std::endl;

		}
		else if (command == "tpriorities")
		{
			char* chp = (char*)filePointer;
			chp += command.length()+1;
			clientID = *(int*)chp;
			clientProcess = OpenProcess(PROCESS_ALL_ACCESS, false, clientID);
			server2Process = GetCurrentProcess();
			server2ID = GetCurrentProcessId();
			ZeroMemory(filePointer, strlen((char*)filePointer) + 1 + sizeof(int));
			strcpy((char*)filePointer, "getid");
			SetEvent(server1Event);
			WaitForSingleObject(server2Event, -1);
			ResetEvent(server2Event);
			server1ID = *(int*)filePointer;
			ZeroMemory(filePointer, sizeof(int) * 3);
			server1Process = OpenProcess(PROCESS_ALL_ACCESS, false, server1ID);
			HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			THREADENTRY32 te32;
			te32.dwSize = sizeof(THREADENTRY32);
			cTPriors.clear();
			s1TPriors.clear();
			s2TPriors.clear();
			std::cout << "My ID: " << server2ID <<std::endl;
			if (!Thread32First(snap, &te32))
			{
				std::cout << "Error! Code: " << GetLastError();;  // Show cause of failure
				CloseHandle(snap);     // Must clean up the snapshot object!
				return(FALSE);
			}
			while (Thread32Next(snap, &te32))
			{
				if (te32.th32OwnerProcessID == clientID) {
					HANDLE t = OpenThread(THREAD_ALL_ACCESS, false, te32.th32ThreadID);
					cTPriors.push_back(GetThreadPriority(t));
				}
			}
			snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			if (!Thread32First(snap, &te32))
			{
				std::cout << "Error! Code: " << GetLastError();;  // Show cause of failure
				CloseHandle(snap);     // Must clean up the snapshot object!
				return(FALSE);
			}
			while (Thread32Next(snap, &te32))
			{
				if (te32.th32OwnerProcessID == server1ID) {
					HANDLE t = OpenThread(THREAD_ALL_ACCESS, false, te32.th32ThreadID);
					s1TPriors.push_back(GetThreadPriority(t));
				}
			}
			snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			if (!Thread32First(snap, &te32))
			{
				std::cout << "Error! Code: " << GetLastError();;  // Show cause of failure
				CloseHandle(snap);     // Must clean up the snapshot object!
				return(FALSE);
			}
			while (Thread32Next(snap, &te32))
			{
				if (te32.th32OwnerProcessID == server2ID) {
					HANDLE t = OpenThread(THREAD_ALL_ACCESS, false, te32.th32ThreadID);
					s2TPriors.push_back(GetThreadPriority(t));
				}
			}

			//threadsPriorities.push_back(cTPriors);
			//
			//threadsPriorities.push_back(s1TPriors);
			//threadsPriorities.push_back(s2TPriors);
			//SIZE_T tpsize = sizeof(std::vector<std::vector<int>>) + sizeof(int) * (threadsPriorities[0].size() + threadsPriorities[1].size() + threadsPriorities[2].size());
			//int* p = (int*)filePointer;
			//*p = tpsize;
			//p++;
			//memcpy(p, &threadsPriorities, sizeof(std::vector<std::vector<int>>) + sizeof(int)*(threadsPriorities[0].size()+threadsPriorities[1].size() + threadsPriorities[2].size()));
			// 

			int *p = (int*)filePointer;
			*p = cTPriors.size();
			p++;
			memcpy(p, cTPriors.data(), sizeof(int)*cTPriors.size());
			p += cTPriors.size();
			*p = s1TPriors.size();
			p++;
			memcpy(p, s1TPriors.data(), sizeof(int)* s1TPriors.size());
			p += s1TPriors.size();
			*p = s2TPriors.size();
			p++;
			memcpy(p, s2TPriors.data(), sizeof(int)* s2TPriors.size());
			std::cout << "Sent thread priorities information" << std::endl;
		}
		else if (command == "stop2")
		{
			ZeroMemory(filePointer, sizeof(char) * (strlen("stop2") + 1));
			int* p = (int*)filePointer;
			*p = 0;
			std::cout << "Recieved stop, shutting down..." << std::endl;
			//CloseHandle(file);
			CloseHandle(filemap);

		}
		else
		{
			std::cout << "Recieved unknown command, sending 404..." << std::endl;
			int* p = (int*)filePointer;
			*p = 0;
		}
		SetEvent(clientEvent);
		Sleep(200);
	} while (command != "stop");
	return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam){
	int n = 0;
	for (int i = 2; i <= 40000000; ++i) {
		bool yes = true;
		for (int j = 2; j < i; ++j) {
			if (i % j == 0) {
				yes = false;
				break;
			}

		}
		if (yes) {
			n++;
		}
	}
	return 0;
}

