#pragma once
#include <Windows.h>
#include <string>
std::string formatTime(SYSTEMTIME s);
std::string formatPriorities(int clPriority, int srv1Priority, int srv2Priority);
std::string getPriorityClassName(int priority);
std::string getTPriorityClassName(int priority);
std::string formatSTime(ULONGLONG sTime);
