/**
 * Copyright (c) <2016>, Continental Corporation.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 *
**/

/**
 * @file  hrtimer.h
 *
 * @author  Rex Schilasky  <rex.schilasky@continental-corporation.com>
 *
 * @brief Declares a high resolution timer class using chrono.
 *
**/

#pragma once

//#define USE_CHRONO

#ifdef USE_CHRONO

#include <chrono>
using namespace std::chrono;

class CHRTimer
{
public:

  // get system counter
  static long long GetCounter()
  {
    return(high_resolution_clock::now().time_since_epoch().count());
  }

  // start the timer
  void Start()
  {
    m_start = high_resolution_clock::now();
  }

  // stop the timer
  void Stop()
  {
    m_end = high_resolution_clock::now();
  }

  // return the interval between the last Start() and Stop() in seconds
  double GetElapsedAsSeconds()
  {
    duration<double> time_span = duration_cast<duration<double>>(m_end - m_start);
    return (time_span.count());
  }

  long long GetElapsedAsMicroSeconds()
  {
    return ((long long)(GetElapsedAsSeconds() * 1000 * 1000));
  }

  // return the system time in seconds
  static double GetSeconds()
  {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(now.time_since_epoch());
    return (time_span.count());
  }

  // return the system time in us
  static long long GetMicroSeconds()
  {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    return(duration_cast<microseconds>(now.time_since_epoch()).count());
  }

  // return the system time in ns
  static long long GetNanoSeconds()
  {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    return(duration_cast<nanoseconds>(now.time_since_epoch()).count());
  }

protected:
  high_resolution_clock::time_point m_start;
  high_resolution_clock::time_point m_end;
};


#else /* USE_CHRONO */


#ifdef WIN32

#if defined(_MSC_VER) && defined(__clang__) && !defined(CINTERFACE)
#define CINTERFACE
#endif

#include <windows.h>
#include <mmsystem.h>

#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

#ifndef MAX
#define MAX(x, y) (x > y ? x : y)
#endif

class CHRTimer
{
public:

  CHRTimer(void)
  {
    m_liStart.QuadPart = m_liStop.QuadPart = 0;
  }

  ~CHRTimer(void)
  {
  }

  // Returns the current counter
  static LONGLONG GetCounter()
  {
    LARGE_INTEGER liTime;
    ::QueryPerformanceCounter(&liTime);
    return liTime.QuadPart;
  }

  // Starts the timer
  void Start()
  {
    ::QueryPerformanceCounter(&m_liStart);
  }

  // Stops the timer
  void Stop()
  {
    ::QueryPerformanceCounter(&m_liStop);
  }


  // Returns the interval between the last Start() and Stop() in seconds
  double GetElapsedAsSeconds()
  {
    LARGE_INTEGER liFrequency;
    ::QueryPerformanceFrequency(&liFrequency);
    return ((double)(m_liStop.QuadPart - m_liStart.QuadPart) / (double)liFrequency.QuadPart);
  }

  // Returns the interval between the last Start() and Stop() in seconds
  long long GetElapsedAsMicroSeconds()
  {
    LARGE_INTEGER liFrequency;
    ::QueryPerformanceFrequency(&liFrequency);
    long long secs = (long long)(m_liStop.QuadPart - m_liStart.QuadPart) / liFrequency.QuadPart;
    long long remain = (long long)(m_liStop.QuadPart - m_liStart.QuadPart) % liFrequency.QuadPart;
    return secs * 1000000 + remain * 1000000 / liFrequency.QuadPart;
  }

  // Returns the system time in seconds
  static double GetSeconds()
  {
    LARGE_INTEGER liFrequency;
    LARGE_INTEGER liTime;
    ::QueryPerformanceFrequency(&liFrequency);
    ::QueryPerformanceCounter(&liTime);
    return (liTime.QuadPart / (double)liFrequency.QuadPart);
  }

  // Returns the system clock in us
  static long long GetMicroSeconds()
  {
    LARGE_INTEGER liFrequency;
    LARGE_INTEGER liTime;
    ::QueryPerformanceFrequency(&liFrequency);
    ::QueryPerformanceCounter(&liTime);
    long long secs = (long long)liTime.QuadPart / liFrequency.QuadPart;
    long long remain = (long long)liTime.QuadPart % liFrequency.QuadPart;
    return secs * 1000000 + remain * 1000000 / liFrequency.QuadPart;
  }

  // Returns the system clock in ns
  static long long GetNanoSeconds()
  {
    LARGE_INTEGER liFrequency;
    LARGE_INTEGER liTime;
    ::QueryPerformanceFrequency(&liFrequency);
    ::QueryPerformanceCounter(&liTime);
    long long secs = (long long)liTime.QuadPart / liFrequency.QuadPart;
    long long remain = (long long)liTime.QuadPart % liFrequency.QuadPart;
    return secs * 1000000000 + remain * 1000000000 / liFrequency.QuadPart;
  }

protected:

  LARGE_INTEGER m_liStart;
  LARGE_INTEGER m_liStop;
};

#else /* WIN32 */

#include <cstdint>
#include <sys/time.h>

static const unsigned g_usec_per_sec = 1000000;
static const unsigned g_usec_per_msec = 1000;

class CHRTimer
{
public:

  CHRTimer(void)
  {
    m_liStart.tv_sec = m_liStop.tv_sec = 0;
    m_liStart.tv_usec = m_liStop.tv_usec = 0;
  }

  ~CHRTimer(void)
  {
  }

  static long long GetCounter()
  {
    struct timeval time;

    /* Grab the current time. */
    gettimeofday(&time, nullptr);
    int64_t performance_count = time.tv_usec +    /* Microseconds. */
      time.tv_sec * g_usec_per_sec;                 /* Seconds.      */

    return(performance_count);
  }

  // Starts the timer
  void Start()
  {
    gettimeofday(&m_liStart, nullptr);
  }

  // Stops the timer
  void Stop()
  {
    gettimeofday(&m_liStop, nullptr);
  }

  // Returns the interval between the last Start() and Stop() in seconds
  double GetElapsedAsSeconds()
  {
    double stop_usec = (m_liStop.tv_sec  * g_usec_per_sec) + m_liStop.tv_usec;
    double start_usec = (m_liStart.tv_sec * g_usec_per_sec) + m_liStart.tv_usec;
    double diff = (stop_usec - start_usec) / g_usec_per_sec;
    return(diff);
  }

  // Returns the interval between the last Start() and Stop() in microseconds
  long long GetElapsedAsMicroSeconds()
  {
    double stop_usec = (m_liStop.tv_sec  * g_usec_per_sec) + m_liStop.tv_usec;
    double start_usec = (m_liStart.tv_sec * g_usec_per_sec) + m_liStart.tv_usec;
    double diff = stop_usec - start_usec;
    return(diff);
  }


  // Returns the system time in seconds
  static double GetSeconds()
  {
    return(((double)GetCounter()) / g_usec_per_sec);
  }

  // Returns the system clock in us
  static long long GetMicroSeconds()
  {
    return(GetCounter());
  }

  // Returns the system clock in ns
  static long long GetNanoSeconds()
  {
    return(GetMicroSeconds() * 1000);  // has to be improved :)
  }

protected:

  timeval m_liStart;
  timeval m_liStop;
};

#endif  /* WIN32 */

#endif /* USE_CHRONO */
