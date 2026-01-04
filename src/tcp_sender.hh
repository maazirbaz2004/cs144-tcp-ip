#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <functional>
#include <cstdint>
#include <queue>

class TCPSender
{
public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
    : input_( std::move( input ) ), isn_( isn ), initial_RTO_ms_( initial_RTO_ms ), RTOms(initial_RTO_ms)
  {}

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage make_empty_message() const;

  /* Receive and process a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Type of the `transmit` function that the push and tick methods can use to send messages */
  using TransmitFunction = std::function<void( const TCPSenderMessage& )>;

  /* Push bytes from the outbound stream */
  void push( const TransmitFunction& transmit );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called */
  void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );

  // Accessors
  uint64_t sequence_numbers_in_flight() const;  // For testing: how many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // For testing: how many consecutive retransmissions have happened?
  const Writer& writer() const { return input_.writer(); }
  const Reader& reader() const { return input_.reader(); }
  Writer& writer() { return input_.writer(); }

private:
  Reader& reader() { return input_.reader(); }

  // Core member variables
  ByteStream input_;
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;

  //new private members added:
  //tracking state
  uint64_t nextSeqno {0} ; //next absolute seqno to send
  uint64_t ackedSeqno {0}; //abs seqno thats been acknowledged
  uint16_t windowSize {1}; //receiver's window size
  bool synSent {false}; //have we sent SYN?
  bool finSent {false}; //have we sent FIN?

  //retransmission timer 
  uint64_t RTOms; //curr rto value
  uint64_t timerMs {0}; //time elapsed since timer started
  bool timerRunning {false}; //check is timer active
  uint64_t consecutiveRetransmissions {0}; 

  //for outstanding segments(making a queue to store it)
  std::queue<TCPSenderMessage> outstandingSegments {};
  uint64_t outstandingSeqnos {0}; //total sequence numbers outstanding
};