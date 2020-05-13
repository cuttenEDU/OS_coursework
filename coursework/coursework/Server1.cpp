#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <time.h>
#include <minwinbase.h>
#include <string>


#pragma comment(lib, "Ws2_32.lib")

std::string command;

void* filePointer;

HANDLE clientEvent;
HANDLE server1Event;
HANDLE server2Event;
std::string formatTime(SYSTEMTIME s);

HANDLE mutex;

int main(int argc, char* argv[])
{
	std::cout << "Server 1 application\n" << "Checking if filemap exists... ";
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

	server1Event = CreateEvent(NULL, TRUE, FALSE, "server1");
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
	std::cout << "Trying to connect to server2...\n";
	server2Event = OpenEvent(EVENT_ALL_ACCESS, false, "server2");
	if (server2Event == NULL) {
		std::cout << "FAILED! Waiting for server2 to launch...\n";
		while (server2Event == NULL)
		{
			server2Event = OpenEvent(EVENT_ALL_ACCESS, false, "server2");
		}
	}

	std::cout << "Connected to server2.\n";
	//mutex = OpenMutex(SYNCHRONIZE, FALSE, "coursework");
	//if (mutex == NULL && GetLastError() == ERROR_FILE_NOT_FOUND)
	//	mutex = CreateMutex(NULL, TRUE, "coursework");
	
	do
	{
		std::cout << "Waiting for commands...\n";
		WaitForSingleObject(server1Event, -1);
		ResetEvent(server1Event);
		//strcpy(command, (char*)filePointer);
		command = (char*)filePointer;
		std::cout << "Recieved command: " << command << std::endl;
		if (command == "time")
		{
			ZeroMemory(filePointer, sizeof(char) * (strlen("time")+1));
			SYSTEMTIME s;
			GetLocalTime(&s);
			std::cout << "Sending time: " << formatTime(s) << std::endl;
			memcpy(filePointer, &s,sizeof(SYSTEMTIME));

		}
		else if (command == "session")
		{
			ZeroMemory(filePointer, sizeof(char) * (strlen("session") + 1));
			ULONGLONG time = GetTickCount64() / CLOCKS_PER_SEC;
			memcpy(filePointer, &time, sizeof(ULONGLONG));
			std::cout << "Sent session time information" << std::endl;
		}
		else if (command == "stop1")
		{
			ZeroMemory(filePointer, sizeof(char) * (strlen("stop") + 1));
			int* p = (int*)filePointer;
			*p = 0;
			std::cout << "Recieved stop, shutting down..." << std::endl;
			
			CloseHandle(filemap);
		}
		else if (command == "getid")
		{
			ZeroMemory(filePointer, sizeof(char) * (strlen("getid") + 1));
			*(int*)filePointer = GetCurrentProcessId();
			std::cout << "sent my id\n";
			SetEvent(server2Event);
			continue;
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

std::string formatTime(SYSTEMTIME s)
{
	std::string time;
	time += std::to_string(s.wYear) + " ";

	switch (s.wMonth)
	{
	case 1:
		time += "January";
		break;

	case 2:
		time += "Febuary";
		break;

	case 3:
		time += "March";
		break;

	case 4:
		time += "April";
		break;

	case 5:
		time += "May";
		break;

	case 6:
		time += "June";
		break;

	case 7:
		time += "Jule";
		break;

	case 8:
		time += "August";
		break;

	case 9:
		time += "September";
		break;

	case 10:
		time += "October";
		break;

	case 11:
		time += "November";
		break;

	case 12:
		time += "December";
		break;
	}
	time += " " + std::to_string(s.wDay);
	switch (s.wDay % 10)
	{
	case 1:
		time += "st";
		break;
	case 2:
		time += "nd";
		break;
	case 3:
		time += "rd";
		break;
	default:
		time += "th";
		break;
	}
	time += ", ";
	switch (s.wDayOfWeek)
	{
	case 0:
		time += "Sunday";
		break;
	case 1:
		time += "Monday";
		break;
	case 2:
		time += "Tuesday";
		break;
	case 3:
		time += "Wednesday";
		break;
	case 4:
		time += "Thursday";
		break;
	case 5:
		time += "Friday";
		break;
	case 6:
		time += "Saturday";
		break;
	}
	time += " " + std::to_string(s.wHour) + ":" + std::to_string(s.wMinute) + ":" + std::to_string(s.wSecond) + " (and " + std::to_string(s.wMilliseconds) + " milliseconds)";
	return time;
}

//bool readCommand()
//{
//
//}
//
//bool writeData()
//{
//
//}



