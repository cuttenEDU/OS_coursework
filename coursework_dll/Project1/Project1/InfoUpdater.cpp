#define _CRT_SECURE_NO_WARNINGS
#include "InfoUpdater.h"
#include "format.h"
	void* filePointer;

	HANDLE clientEvent;
	HANDLE server1Event;
	HANDLE server2Event;
	/*
	 * Class:     InfoUpdater
	 * Method:    getSystemTime
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_InfoUpdater_getSystemTime
	(JNIEnv* env, jclass)
	{
		jstring result;
		strcpy((char*)filePointer, "time");
		SetEvent(server1Event);
		WaitForSingleObject(clientEvent, -1);
		ResetEvent(clientEvent);
		SYSTEMTIME s;
		memcpy(&s, filePointer, sizeof(SYSTEMTIME));
		ZeroMemory(filePointer, sizeof(SYSTEMTIME));
		result = env->NewStringUTF(formatTime(s).c_str());
		return result;
		//std::cout << formatTime(s) << std::endl;
	}

	/*
	 * Class:     InfoUpdater
	 * Method:    getSessionTime
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_InfoUpdater_getSessionTime
	(JNIEnv* env, jclass)
	{
		strcpy((char*)filePointer, "session");
		SetEvent(server1Event);
		WaitForSingleObject(clientEvent, -1);
		ResetEvent(clientEvent);
		ULONGLONG seconds = *(ULONGLONG*)filePointer;
		ZeroMemory(filePointer, sizeof(ULONGLONG));
		std::string res = formatSTime(seconds);
		jstring result = env->NewStringUTF(res.c_str());
		return result;
		//std::cout << "Seconds since launch: " << seconds << std::endl;
	}

	/*
	 * Class:     InfoUpdater
	 * Method:    getPriorities
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_InfoUpdater_getPriorities
	(JNIEnv* env, jclass)
	{
		char* p = (char*)filePointer;
		strcpy(p, "priorities");
		p += strlen("priorities") + 1;
		*(int*)p = GetCurrentProcessId();
		SetEvent(server2Event);
		WaitForSingleObject(clientEvent, -1);
		ResetEvent(clientEvent);
		int* ip = (int*)filePointer;
		int clPriority = *ip;
		int srv1Priority = *(ip + 1);
		int srv2Priority = *(ip + 2);
		ZeroMemory(filePointer, sizeof(int) * 3);
		std::string res = formatPriorities(clPriority, srv1Priority, srv2Priority);
		jstring result = env->NewStringUTF(res.c_str());
		return result;
		//std::cout << formatPriorities(clPriority, srv1Priority, srv2Priority);
	}

	/*
	 * Class:     InfoUpdater
	 * Method:    getTPriorities
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_InfoUpdater_getTPriorities
	(JNIEnv* env, jclass)
	{
		char* p = (char*)filePointer;
		strcpy(p, "tpriorities");
		p += strlen("tpriorities") + 1;
		*(int*)p = GetCurrentProcessId();
		SetEvent(server2Event);
		WaitForSingleObject(clientEvent, -1);
		ResetEvent(clientEvent);
		int* ip = (int*)filePointer;
		int ccount = *ip;
		ip++;
		std::string res;
		res += "Client threads: \n";
		for (int i = 0; i < ccount; ++i) {
			res += "Thread #" + std::to_string(i) + ": Priority: " + getTPriorityClassName(*ip) + "(" + std::to_string(*ip) + ")\n";
			ip++;
		}
		int s1count = *ip;
		ip++;
		res += "Server1 threads: \n";
		for (int i = 0; i < s1count; ++i) {
			res += "Thread #" + std::to_string(i) + ": Priority: " + getTPriorityClassName(*ip) + "(" + std::to_string(*ip) + ")\n";
			ip++;
		}
		int s2count = *ip;
		ip++;
		res += "Server2 threads:\n";
		for (int i = 0; i < s2count; ++i) {
			res += "Thread #" + std::to_string(i) + ": Priority: " + getTPriorityClassName(*ip) + "(" + std::to_string(*ip) + ")\n";
			ip++;
		}
		ZeroMemory(filePointer, ccount + s1count + s2count + 3);
		jstring result = env->NewStringUTF(res.c_str());
		return result;
	}

	/*
	 * Class:     InfoUpdater
	 * Method:    init
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_InfoUpdater_init
	(JNIEnv* env, jclass)
	{
		//std::cout << "Client application\n" << "Checking if filemap exists... ";
		HANDLE filemap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "coursework_map");
		if (filemap == NULL)
		{
			//std::cout << "FAILED! Creating filemap...";
			LPCSTR fileName = "coursework";
			HANDLE file = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL);
			filemap = CreateFileMappingA(file,
				NULL, PAGE_READWRITE, 0, 2048, "coursework_map");
		}
		//std::cout << "SUCCESS!\n";
		filePointer = (char*)MapViewOfFile(filemap,
			FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		clientEvent = OpenEvent(EVENT_ALL_ACCESS, false, "client");
		if (clientEvent == NULL)
		{
			clientEvent = CreateEvent(NULL, TRUE, FALSE, "client");
		}
		//std::cout << "Trying to connect to server1...";
		server1Event = OpenEvent(EVENT_ALL_ACCESS, false, "server1");
		if (server1Event == NULL)
		{
			//std::cout << "FAILED! Waiting for server1 to launch...\n";
			while (server1Event == NULL)
			{
				server1Event = OpenEvent(EVENT_ALL_ACCESS, false, "server1");
			}
		}
		//std::cout << "Connected to server1\n";
		//std::cout << "Trying to connect to server2...\n";
		server2Event = OpenEvent(EVENT_ALL_ACCESS, false, "server2");
		if (server2Event == NULL)
		{
			//std::cout << "FAILED! Waiting for server2 to launch...\n";
			while (server2Event == NULL)
			{
				server2Event = OpenEvent(EVENT_ALL_ACCESS, false, "server2");
			}
		}
		//std::cout << "Connected to server2\n";
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
		//std::cout << "Success\n";
	}


