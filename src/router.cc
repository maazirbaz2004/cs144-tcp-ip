#include "router.hh"
#include "debug.hh"

#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  routingTable.push_back( { route_prefix, prefix_length, next_hop, interface_num } );
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{
  for ( auto& iface : interfaces_ ) {
    auto& datagrams = iface->datagrams_received();

    while ( !datagrams.empty() ) {
      InternetDatagram dgram = datagrams.front();
      datagrams.pop();

      //check ttl and decrement
      if ( dgram.header.ttl == 0 || --dgram.header.ttl == 0 ) {
        continue; //drop if expired
      }
      dgram.header.compute_checksum();
      uint32_t dstIp = dgram.header.dst;

      //finding longest prefix match in routing table
      int bestMatchLength = -1;
      size_t bestInterfaceNum = 0;
      optional<Address> bestNextHop;

      //check if this route matches the destination
      for ( const auto& route : routingTable ) {
        if ( route.prefixLength == 0 ) {
          if ( bestMatchLength < 0 ) {
            bestMatchLength = 0;
            bestInterfaceNum = route.interfaceNum;
            bestNextHop = route.nextHop;
          }
        } else {
          //createdmask for prefix matching
          uint32_t mask = ( route.prefixLength == 32 ) ? 0xFFFFFFFF : ~( 0xFFFFFFFF >> route.prefixLength );
          if ( ( dstIp & mask ) == ( route.routePrefix & mask ) ) {
            if ( static_cast<int>( route.prefixLength ) > bestMatchLength ) {
              bestMatchLength = route.prefixLength;
              bestInterfaceNum = route.interfaceNum;
              bestNextHop = route.nextHop;
            }
          }
        }
      }
      //no match, drop datagram
      if ( bestMatchLength < 0 ) {
        continue;
      }
      Address nextHopAddr = bestNextHop.value_or( Address::from_ipv4_numeric( dstIp ) );
      interface( bestInterfaceNum )->send_datagram( dgram, nextHopAddr );
    }
  }
}