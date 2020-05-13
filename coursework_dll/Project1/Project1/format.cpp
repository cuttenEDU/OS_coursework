#include "Format.h"
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

std::string formatPriorities(int clPriority, int srv1Priority, int srv2Priority)
{
	std::string result;
	result += getPriorityClassName(clPriority) + "\n";
	result += getPriorityClassName(srv1Priority) + "\n";
	result += getPriorityClassName(srv2Priority) + "\n";
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
	}
	return result;
}

std::string getTPriorityClassName(int priority) {
	std::string result;
	switch (priority) {
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
				sTime -= (int)(sTime / 86400) * 86400;
			}
			result += std::to_string((int)(sTime / 3600)) + " Hours, ";
			sTime -= (int)(sTime / 3600) * 3600;
		}
		result += std::to_string((int)(sTime / 60)) + " Minutes, ";
		sTime -= (int)(sTime / 60) * 60;

	}
	result += std::to_string(sTime % 60) + " Seconds.";
	return result;
}