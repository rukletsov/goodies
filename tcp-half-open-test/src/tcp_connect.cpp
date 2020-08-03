#include <cerrno>
#include <cstring>

#include <iostream>
#include <string>

#include <unistd.h>

#ifndef __WINDOWS__
#include <arpa/inet.h> // for `inet_pton()`

#define __FAVOR_BSD
#include <netinet/in.h> // for `sockaddr_in`
#include <netinet/ip.h> // for `ip`
#include <netinet/tcp.h> // for `tcphdr`
#undef __FAVOR_BSD

#include <sys/types.h>
#include <sys/socket.h>
#endif // __WINDOWS__

using std::cerr;
using std::cout;
using std::endl;
using std::string;

const int PACKET_LENGTH = 1024;
const char MODE_FULL[] = "full";
const char MODE_HALF_OPEN[] = "half-open";


// Tries to establish a TCP connection to `targetIP`:`targetPort`.
// If a TCP handshake is successful, returns `EXIT_SUCCESS`.
int tcpThreeWay(const string& targetIP, int targetPort);

int tcpHalfOpen(const string& targetIP, int targetPort);


int main(int argc, char *argv[])
{
  if (argc < 4) {
    cerr << "Too few arguments; usage: "
         << "tcpconnect <ip> <port> <mode>:["
         << MODE_FULL << "|" << MODE_HALF_OPEN << "]" << endl;
  }

  string targetIP = argv[1]; // TODO: Add support for IPv6.
  int targetPort = std::stoi(argv[2]);
  string mode = argv[3]; // "full" or "half-open"

  int result = EXIT_FAILURE;
  if (mode == MODE_FULL) {
    result = tcpThreeWay(targetIP, targetPort);
  } else if (mode == MODE_HALF_OPEN) {
    result =  tcpHalfOpen(targetIP, targetPort);
  } else {
    cerr << "Unknown mode; only '" << MODE_FULL
         << "' and '" << MODE_HALF_OPEN << "' are supported" << endl;
  }

  return result;
}


int tcpThreeWay(const string& targetIP, int targetPort)
{
  // Create a TCP socket.
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd < 0) {
    cerr << "Failed to create socket: " << strerror(errno) << endl;
    return EXIT_FAILURE;
  }

  // Set up destination address.
  struct sockaddr_in to;
  memset(&to, 0, sizeof(to));
  to.sin_family = AF_INET;
  inet_pton(AF_INET, targetIP.c_str(), &to.sin_addr);
  to.sin_port = htons(targetPort);

  // Try to connect to socket. If the connection succeeds,
  // zero is returned, indicating the remote port is open.
  cout << "Connecting to " << targetIP << ":" << targetPort << endl;
  if (connect(socketFd, reinterpret_cast<sockaddr*>(&to), sizeof(to)) < 0) {
    cerr << "Connection failed: " << strerror(errno) << endl;
    close(socketFd);
    return EXIT_FAILURE;
  }

  cout << "Successfully established TCP connection" << endl;

  shutdown(socketFd, SHUT_RDWR);
  close(socketFd);

  return EXIT_SUCCESS;
}


int tcpHalfOpen(const string& targetIP, int targetPort)
{
    char send_buffer[PACKET_LENGTH];
    char recv_buffer[PACKET_LENGTH];
    memset(send_buffer, 0, PACKET_LENGTH);
    memset(recv_buffer, 0, PACKET_LENGTH);

    // Create a raw socket.
    int socketFd;
    socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(socketFd < 0){
       cerr << "Failed to create socket" << endl;
       return EXIT_FAILURE;
    }

    cout << "Created socket " << socketFd << endl;

    struct sockaddr_in from;
    memset(&from, 0, sizeof(from));
    from.sin_family = AF_INET;
    inet_pton(AF_INET, targetIP.c_str(), &from.sin_addr);
    from.sin_port = htons(9090);

    bind(socketFd, (struct sockaddr*) &from, sizeof(from));

    // Destination address.
    struct sockaddr_in to;
    socklen_t addrlen = sizeof(to);
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    inet_pton(AF_INET, targetIP.c_str(), &to.sin_addr);
    to.sin_port = htons(targetPort);

    // Prepare the TCP header segment of the raw packet.
    struct tcphdr* tcp = reinterpret_cast<struct tcphdr*>(send_buffer);
    tcp->th_sport = htons(9090);
    tcp->th_dport = to.sin_port;
    tcp->th_seq = htonl(1);
    tcp->th_off = 5;
    tcp->th_flags = TH_SYN;
    tcp->th_win = htons(65535);

    cout << "Prepared the TCP header" << endl;

    // Send the SYN packet.
    ssize_t send = sendto(
        socketFd,
        send_buffer,
        sizeof(*tcp),
        0,
        reinterpret_cast<struct sockaddr*>(&to),
        addrlen);

    cout << "Sent SYN packet" << endl;

    if (send < 0) {
      cerr << "sendto failed" << endl;
      close(socketFd);
      return EXIT_FAILURE;
    }

    // Receive the answer.
    ssize_t recv = recvfrom(
        socketFd,
        recv_buffer,
        PACKET_LENGTH,
        0,
        reinterpret_cast<struct sockaddr*>(&to),
        &addrlen
        );

    cout << "Received the response" << endl;

    if (recv < 0) {
        cerr << "Failed to receive packet" << endl;
        close(socketFd);
        return EXIT_FAILURE;
    } else if (recv == 0) { // timed out
        cerr << "Peer shutdown the connection" << endl;
        close(socketFd);
        return EXIT_FAILURE;
    }

    // Check we received a SYN/ACK.
    // NOTE: Skip the IP header.
    tcphdr* recvTcp = reinterpret_cast<struct tcphdr*>(
        recv_buffer + sizeof(struct ip));

    if (recvTcp->th_flags & (TH_SYN | TH_ACK)) {
      cout << "Successfully established a TCP half-open connection to "
           << targetIP << ":" << targetPort << endl;

      close(socketFd);
      return EXIT_SUCCESS;
    } else {
      cout << "Received packet is not SYN/ACK" << endl;
      close(socketFd);
      return EXIT_FAILURE;
    }


    //TODO(alexr): reply with an RST.

    close(socketFd);
    return 0;
}
