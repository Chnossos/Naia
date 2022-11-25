#include "ProgressUnit.hpp"
#include <iostream>

using namespace std;

ProgressBar::ProgressBar (int stp_count) : lenght (50), step_count (stp_count), current_pos (0), current_step (0)
{
#ifdef _WIN32
    cout << "\n\x3D";
#else
    cout << "[";
#endif
    for (int i = 0 ; i < lenght ; ++i)
		cout << " ";
#ifdef _WIN32
    cout << "\x3D 0%\r\x3D";
#else
    cout << "] 0%\r[";
#endif
	if (stp_count == 0)
		step();
}

ProgressBar::~ProgressBar ()
{
    cout << endl;
}

void ProgressBar::step ()
{
    if (step_count == 0)
    {
    #ifdef _WIN32
        cout << "\r\x3D";
    #else
        cout << "\r[";
    #endif
        for (int i = 0 ; i < lenght ; ++i)
                cout << full;
    #ifdef _WIN32
        cout << "\x3D 100%  \r\x3D";
    #else
        cout << "] 100%  \r[";
    #endif
		return;
    }

    ++current_step;
    int position = current_step * lenght / step_count;
    if (position != current_pos)
    {
    #ifdef _WIN32
        cout << "\r\x3D";
    #else
        cout << "\r[";
    #endif
        int i = 0;
        for ( ; i < position ; ++i)
			cout << full;
        for ( ; i < lenght ; ++i)
			cout << " ";
        float percent = ((float (position) / float (lenght)) * 100);
    #ifdef _WIN32
        cout << "\x3D " << int (percent) << "%  \r\x3D";
    #else
        cout << "] " << int (percent) << "%  \r[";
    #endif
        current_pos = position;
    }
}

//////////////////////////////////////////////////

ProgressPercent::ProgressPercent (int stp_count) : step_count (stp_count), last_percent (0), current_pos (0), current_step (0)
{
    cout << " 0%";
	if (stp_count == 0)
		step();
}

ProgressPercent::~ProgressPercent ()
{
    cout << endl;
}

void ProgressPercent::step ()
{
    if (step_count == 0)
    {
        cout << "\b\b100%";
		return;
    }

    ++current_step;
    float percent = ((float (current_step) / float (step_count)) * 100);
    if (last_percent < 10)
        cout << "\b\b";
    else if (last_percent <= 100)
        cout << "\b\b\b";
    cout << int (percent) << "%";
    last_percent = int (percent);
}

