#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <wchar.h>
#include <math.h>


void* filePointer;

HANDLE clientEvent;
HANDLE server1Event;
HANDLE server2Event;

std::string formatTime(SYSTEMTIME s);
std::string formatPriorities(int clPriority, int srv1Priority, int srv2Priority);
std::string getPriorityClassName(int priority);
std::string getTPriorityClassName(int priority);
std::string formatSTime(ULONGLONG sTime);

int main(int argc, char* argv[])
{
	std::cout << "Client application\n" << "Checking if filemap exists... ";
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
	clientEvent = OpenEvent(EVENT_ALL_ACCESS, false, "client");
	if (clientEvent == NULL)
	{
		clientEvent = CreateEvent(NULL, TRUE, FALSE, "client");
	}
	std::cout << "Trying to connect to server1...";
	server1Event = OpenEvent(EVENT_ALL_ACCESS, false, "server1");
	if (server1Event == NULL)
	{
		std::cout << "FAILED! Waiting for server1 to launch...\n";
		while (server1Event == NULL)
		{
			server1Event = OpenEvent(EVENT_ALL_ACCESS, false, "server1");
		}
	}
	std::cout << "Connected to server1\n";
	std::cout << "Trying to connect to server2...\n";
	server2Event = OpenEvent(EVENT_ALL_ACCESS, false, "server2");
	if (server2Event == NULL)
	{
		std::cout << "FAILED! Waiting for server2 to launch...\n";
		while (server2Event == NULL)
		{
			server2Event = OpenEvent(EVENT_ALL_ACCESS, false, "server2");
		}
	}
	std::cout << "Connected to server2\n";
	//std::cout << "Creating file mapping... ";
	//HANDLE filemap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "coursework_map");
	//if (filemap == NULL)
	//{
	//	std::cout << "Mapping failed! Error: " << GetLastError();
	//	_getwch();
	//	return -1;
	//}
	//std::cout << "Success!\n"
	//	<< "Creating pointer...";
	//filePointer = MapViewOfFile(filemap,
	//                            FILE_MAP_ALL_ACCESS, 0, 0, 2048);
	//if (!filePointer)
	//{
	//	std::cout << "Creating pointer failed! Error: " << GetLastError();
	//	_getwch();
	//	return -1;
	//}
	std::cout << "Success\n";
	std::cout << "type \"help\" to see all commands\n";
	std::string command;
	while (command.c_str() != "stop")
	{
		std::cout << "> ";
		std::getline(std::cin, command);
		if (command == "time")
		{
			strcpy((char*)filePointer, "time");
			SetEvent(server1Event);
			WaitForSingleObject(clientEvent, -1);
			ResetEvent(clientEvent);
			SYSTEMTIME s;
			memcpy(&s, filePointer, sizeof(SYSTEMTIME));
			ZeroMemory(filePointer, sizeof(SYSTEMTIME));
			std::cout << formatTime(s) << std::endl;
		}
		else if (command == "session")
		{
			strcpy((char*)filePointer, "session");
			SetEvent(server1Event);
			WaitForSingleObject(clientEvent, -1);
			ResetEvent(clientEvent);
			ULONGLONG seconds = *(ULONGLONG*)filePointer;
			ZeroMemory(filePointer, sizeof(ULONGLONG));
			std::cout << "Time has passed since startup (uptime): " << formatSTime(seconds) << std::endl;
		}
		else if (command == "priorities")
		{
			char* p = (char*)filePointer;
			strcpy(p, "priorities");
			p += command.length() + 1;
			*(int*)p = GetCurrentProcessId();
			SetEvent(server2Event);
			WaitForSingleObject(clientEvent, -1);
			ResetEvent(clientEvent);
			int* ip = (int*)filePointer;
			int clPriority = *ip;
			int srv1Priority = *(ip + 1);
			int srv2Priority = *(ip + 2);
			ZeroMemory(filePointer, sizeof(int) * 3);
			std::cout << formatPriorities(clPriority, srv1Priority, srv2Priority);




		}
		else if (command == "tpriorities")
		{
			char* p = (char*)filePointer;
			strcpy(p, command.c_str());
			p += command.length() + 1;
			*(int*)p = GetCurrentProcessId();
			SetEvent(server2Event);
			WaitForSingleObject(clientEvent, -1);
			ResetEvent(clientEvent);
			int* ip = (int*)filePointer;
			int ccount = *ip;
			ip++;
			std::cout << "Client threads: \n";
			for (int i = 0; i < ccount; ++i) {
				std::cout << "Thread #" << i << ": Priority: " << getTPriorityClassName(*ip) << std::endl;
				ip++;
			}
			int s1count = *ip;
			ip++;
			std::cout << "Server1 threads: \n";
			for (int i = 0; i < s1count; ++i) {
				std::cout << "Thread #" << i << ": Priority: " << getTPriorityClassName(*ip) << std::endl;
				ip++;
			}
			int s2count = *ip;
			ip++;
			std::cout << "Server2 threads:\n";
			for (int i = 0; i < s2count; ++i) {
				std::cout << "Thread #" << i << ": Priority: " << getTPriorityClassName(*ip) << std::endl;
				ip++;
			}
			ZeroMemory(filePointer, ccount + s1count + s2count + 3);
			std::cout << *(int*)filePointer;


		}
		else if (command == "help")
		{
			std::cout << "time - get current system time (server1 call)\n"
				<< "session - get current session time (server1 call)\n"
				<< "priorities - get client and all servers system priorities (server2 call)\n"
				<< "tpriorities - get relative threads priorities of client and servers processes (server2 call)\n"
				<< "help - get this message again (but why would you want that?)\n";
		}
		else
		{
			std::cout << "Unknown command! Type \"help\" to see all commands\n";
		}
	}


	_getwch();
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
	switch(s.wDay % 10)
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

std::string formatPriorities(int clPriority, int srv1Priority, int srv2Priority)
{
	std::string result;
	result += "Client priority: " + getPriorityClassName(clPriority) + "\n";
	result += "Server1 priority: " + getPriorityClassName(srv1Priority) + "\n";
	result += "Server2 priority: " + getPriorityClassName(srv2Priority) + "\n";
	return result;
}

std::string getPriorityClassName(int priority)
{
	std::string result;
	switch (priority)
	{
	case ABOVE_NORMAL_PRIORITY_CLASS:
		result = "ABOVE_NORMAL_PRIORITY_CLASS";
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		result = "BELOW_NORMAL_PRIORITY_CLASS";
		break;
	case HIGH_PRIORITY_CLASS:
		result = "HIGH_PRIORITY_CLASS";
		break;
	case IDLE_PRIORITY_CLASS:
		result = "IDLE_PRIORITY_CLASS";
		break;
	case NORMAL_PRIORITY_CLASS:
		result = "NORMAL_PRIORITY_CLASS";
		break;
	case REALTIME_PRIORITY_CLASS:
		result = "REALTIME_PRIORITY_CLASS";
		break;
	default:
		std::cout << "format got " << priority;
		result = "error";
	}
	return result;
}

std::string getTPriorityClassName(int priority) {
	std::string result;
	switch(priority) {
	case THREAD_MODE_BACKGROUND_BEGIN:
		result = "THREAD_MODE_BACKGROUND_BEGIN";
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		result = "THREAD_PRIORITY_ABOVE_NORMAL";
		break;
	case THREAD_MODE_BACKGROUND_END:
		result = "THREAD_MODE_BACKGROUND_END";
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		result = "THREAD_PRIORITY_BELOW_NORMAL";
		break;
	case THREAD_PRIORITY_HIGHEST:
		result = "THREAD_PRIORITY_HIGHEST";
		break;
	case THREAD_PRIORITY_IDLE:
		result = "THREAD_PRIORITY_IDLE";
		break;
	case THREAD_PRIORITY_LOWEST:
		result = "THREAD_PRIORITY_LOWEST";
		break;
	case THREAD_PRIORITY_NORMAL:
		result = "THREAD_PRIORITY_NORMAL";
		break;
	case THREAD_PRIORITY_TIME_CRITICAL:
		result = "THREAD_PRIORITY_TIME_CRITICAL";
		break;
	}
	return result;
}

std::string formatSTime(ULONGLONG sTime)
{
	std::string result;
	if (sTime > 60)
	{
		if (sTime > 3600)
		{
			if (sTime > 86400)
			{
				result += std::to_string((int)(sTime / 86400)) + " Days, ";
				sTime -= (int)(sTime / 86400)*86400;
			}
			result += std::to_string((int)(sTime / 3600)) + " Hours, ";
			sTime -= (int)(sTime / 3600)*3600;
		}
		result += std::to_string((int)(sTime / 60)) + " Minutes, ";
		sTime -= (int)(sTime / 60)*60;

	}
	result += std::to_string(sTime % 60) + " Seconds.";
	return result;
}