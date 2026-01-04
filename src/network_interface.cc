#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

using namespace std;

// Constants
constexpr size_t ARP_CACHE_TTL_MS = 30000;      // 30 seconds
constexpr size_t ARP_REQUEST_TIMEOUT_MS = 5000; // 5 seconds

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  uint32_t nextHopIp = next_hop.ipv4_numeric();

  //we have the ethernet address in our cache so send frame
  auto cacheIt = arpCache.find( nextHopIp );
  if ( cacheIt != arpCache.end() ) { 
    EthernetFrame frame; 
    frame.header.type = EthernetHeader::TYPE_IPv4;
    frame.header.src = ethernet_address_;
    frame.header.dst = cacheIt->second.ethernetAddr;
    frame.payload = serialize( dgram );
    transmit( frame );
    return;
  }

  //don't know the ethernet address, need to send arp req
  auto requestIt = arpRequestTime.find( nextHopIp );
  bool shouldSendArp = false;

  if ( requestIt == arpRequestTime.end() ) {
    shouldSendArp = true;
  } else if ( requestIt->second == 0 ) {
    shouldSendArp = true; //timer expired, so can send again
  }

  if ( shouldSendArp ) { //send arp req
    ARPMessage arpRequest;
    arpRequest.opcode = ARPMessage::OPCODE_REQUEST;
    arpRequest.sender_ethernet_address = ethernet_address_;
    arpRequest.sender_ip_address = ip_address_.ipv4_numeric();
    arpRequest.target_ethernet_address = {}; 
    arpRequest.target_ip_address = nextHopIp;

    EthernetFrame arpFrame;
    arpFrame.header.type = EthernetHeader::TYPE_ARP;
    arpFrame.header.src = ethernet_address_;
    arpFrame.header.dst = ETHERNET_BROADCAST; 
    arpFrame.payload = serialize( arpRequest );
    transmit( arpFrame );

    arpRequestTime[nextHopIp] = ARP_REQUEST_TIMEOUT_MS;
  }
  waitingDatagrams.push_back( { dgram, next_hop, ARP_REQUEST_TIMEOUT_MS } );
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  //ignore frames not for us
  if ( frame.header.dst != ethernet_address_ && frame.header.dst != ETHERNET_BROADCAST ) {
    return;
  }

  if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
    InternetDatagram dgram;
    if ( parse( dgram, frame.payload ) ) {
      datagrams_received_.push( dgram );
    }
    return;
  }

  //handle arp frames
  if ( frame.header.type == EthernetHeader::TYPE_ARP ) {
    ARPMessage arpMsg;
    if ( !parse( arpMsg, frame.payload ) ) {
      return; 
    }

    //learn the mapping from sender (always learn from both requests and replies)
    uint32_t senderIp = arpMsg.sender_ip_address;
    EthernetAddress senderEth = arpMsg.sender_ethernet_address;
    arpCache[senderIp] = { senderEth, ARP_CACHE_TTL_MS };

    //send any queued datagrams waiting for this IP
    auto it = waitingDatagrams.begin();
    while ( it != waitingDatagrams.end() ) {
      if ( it->nextHop.ipv4_numeric() == senderIp ) {
        EthernetFrame outFrame;
        outFrame.header.type = EthernetHeader::TYPE_IPv4;
        outFrame.header.src = ethernet_address_;
        outFrame.header.dst = senderEth;
        outFrame.payload = serialize( it->datagram );
        transmit( outFrame );
        it = waitingDatagrams.erase( it );
      } else {
        ++it;
      }
    }

    //if arp request for our IP, send reply
    if ( arpMsg.opcode == ARPMessage::OPCODE_REQUEST && arpMsg.target_ip_address == ip_address_.ipv4_numeric() ) {
      ARPMessage arpReply;
      arpReply.opcode = ARPMessage::OPCODE_REPLY;
      arpReply.sender_ethernet_address = ethernet_address_;
      arpReply.sender_ip_address = ip_address_.ipv4_numeric();
      arpReply.target_ethernet_address = arpMsg.sender_ethernet_address;
      arpReply.target_ip_address = arpMsg.sender_ip_address;

      EthernetFrame replyFrame;
      replyFrame.header.type = EthernetHeader::TYPE_ARP;
      replyFrame.header.src = ethernet_address_;
      replyFrame.header.dst = arpMsg.sender_ethernet_address;
      replyFrame.payload = serialize( arpReply );
      transmit( replyFrame );
    }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  //decrement TTL for arp cache entries and remove expired ones
  auto cacheIt = arpCache.begin();
  while ( cacheIt != arpCache.end() ) {
    if ( cacheIt->second.ttl <= ms_since_last_tick ) {
      cacheIt = arpCache.erase( cacheIt );
    } else {
      cacheIt->second.ttl -= ms_since_last_tick;
      ++cacheIt;
    }
  }
  //decrement timers for arp requests
  auto requestIt = arpRequestTime.begin();
  while ( requestIt != arpRequestTime.end() ) {
    if ( requestIt->second <= ms_since_last_tick ) {
      requestIt->second = 0; 
      ++requestIt;
    } else {
      requestIt->second -= ms_since_last_tick;
      ++requestIt;
    }
  }
  //decrement TTL for waiting datagrams and remove expired ones
  auto datagramIt = waitingDatagrams.begin();
  while ( datagramIt != waitingDatagrams.end() ) {
    if ( datagramIt->ttl <= ms_since_last_tick ) {
      datagramIt = waitingDatagrams.erase( datagramIt ); //drop expired datagram
    } else {
      datagramIt->ttl -= ms_since_last_tick;
      ++datagramIt;
    }
  }
}