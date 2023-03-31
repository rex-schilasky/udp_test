#include <string>
#include <iostream>

#include "udp_receiver.h"
#include "hrtimer.h"

constexpr auto UDP_IP_GROUP = "127.255.255.255";
constexpr auto UDP_IP_PORT  = 6788;
constexpr auto UDP_RCVBUF   = (1*1024*1024)   /* 1 MByte */;

// globals
eCAL::CUDPReceiver  g_udp_receiver;
CHRTimer            g_timer;
const int           g_rcv_buf_size(32*1024) /* 32 kByte */;
long long           g_rcv_buf[g_rcv_buf_size / sizeof(long long)];
long long           g_rcv_index = 0;
long long           g_rcv_msgs = 0;
long long           g_rcv_bytes = 0;

int main(int /*argc*/, char** /*argv*/)
{
  std::cout << "Message size = " << g_rcv_buf_size << " Byte" << std::endl << std::endl;

  // set udp parameter
  eCAL::SReceiverAttr attr;
  attr.ipaddr    = UDP_IP_GROUP;
  attr.port      = UDP_IP_PORT;
  attr.broadcast = true;
  attr.unicast   = false;
  attr.loopback  = false;
  attr.rcvbuf    = UDP_RCVBUF;

  // create udp receiver
  g_udp_receiver.Create(attr);

  // receive for ever
  g_timer.Start();
  for(;;)
  {
    g_rcv_bytes += g_udp_receiver.Receive((char*)g_rcv_buf, g_rcv_buf_size, 10000);
    g_rcv_index++;
    g_rcv_msgs++;
    if(g_rcv_index%100 == 0)
    {
      g_timer.Stop();
      double diff_time = g_timer.GetElapsedAsSeconds();
      if(diff_time >= 1.0)
      {
        std::cout << "Received : " << g_rcv_index                                 << std::endl;
        std::cout << "msgs/s   : " << int(g_rcv_msgs  / diff_time)                << std::endl;
        std::cout << "kByte/s  : " << int(g_rcv_bytes / 1024 / diff_time)         << std::endl;
        std::cout << "MByte/s  : " << int(g_rcv_bytes / 1024 / 1024 / diff_time)  << std::endl;
        std::cout << std::endl;
        g_timer.Start();
        g_rcv_bytes = 0;
        g_rcv_msgs  = 0;
     }
    }
  }
}
