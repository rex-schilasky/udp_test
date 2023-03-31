#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "hrtimer.h"
#include "udp_sender.h"

constexpr auto UDP_IP_GROUP = "127.255.255.255";
constexpr auto UDP_IP_PORT  = 6788;
constexpr auto UDP_SNDBUF   = (1*1024*1024) /* 1 MByte */;

// globals
eCAL::CUDPSender    g_udp_sender;
int                 g_snd_sleep_us(0);
CHRTimer            g_timer;
const int           g_snd_buf_size(32*1024) /* 32 kByte */;
long long           g_snd_buf[g_snd_buf_size / sizeof(long long)];
long long           g_snd_index = 0;
long long           g_snd_msgs = 0;
long long           g_snd_bytes = 0;

int main(int /*argc*/, char** /*argv*/)
{
  std::cout << "Message size = " << g_snd_buf_size << " Byte" << std::endl << std::endl;

  // set udp parameter
  eCAL::SSenderAttr attr;
  attr.ipaddr     = UDP_IP_GROUP;
  attr.port       = UDP_IP_PORT;
  attr.ttl        = 0;
  attr.broadcast  = true;
  attr.unicast    = false;
  attr.loopback   = false;
  attr.sndbuf     = UDP_SNDBUF;

  // create udp sender
  g_udp_sender.Create(attr);

  // send for ever
  g_timer.Start();
  for(;;)
  {
    g_snd_index++;
    g_snd_msgs++;
    g_snd_buf[0] = g_snd_index;
    g_snd_bytes += g_udp_sender.Send((void*)g_snd_buf, g_snd_buf_size);

    if(g_snd_sleep_us > 0)
    {
      long long tstart_us = g_timer.GetMicroSeconds();
      long long tcurr_us  = 0;
      do
      {
        tcurr_us = g_timer.GetMicroSeconds();
      } while((tcurr_us - tstart_us) < g_snd_sleep_us);
    }

    if(g_snd_index%100 == 0)
    {
      g_timer.Stop();
      double diff_time = g_timer.GetElapsedAsSeconds();
      if(diff_time >= 1.0)
      {
        std::cout << "Sent    : " << (int)g_snd_index                           << std::endl;
        std::cout << "msgs/s  : " << int(g_snd_msgs  / diff_time)               << std::endl;
        std::cout << "kByte/s : " << int(g_snd_bytes / 1024 / diff_time)        << std::endl;
        std::cout << "MByte/s : " << int(g_snd_bytes / 1024 / 1024 / diff_time) << std::endl;
        std::cout << std::endl;
        g_timer.Start();
        g_snd_bytes = 0;
        g_snd_msgs  = 0;
      }
    }
  }
}
