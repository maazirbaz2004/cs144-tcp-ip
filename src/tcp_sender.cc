#include "tcp_sender.hh"
#include "tcp_config.hh"

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  return outstandingSeqnos;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  return consecutiveRetransmissions;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  uint64_t effectiveWindow = windowSize > 0 ? windowSize : 1;
  uint64_t bytesInFlight = nextSeqno - ackedSeqno;
  
  //keep sending till there's space in the window
  while (bytesInFlight < effectiveWindow) {
    TCPSenderMessage msg;
    msg.seqno = Wrap32::wrap( nextSeqno, isn_ );
    msg.RST = reader().has_error();  
    
    //send syn if we haven't yet
    if (!synSent) {
      msg.SYN = true;
      synSent = true;
    }
    
    //calculate available payload space
    uint64_t spaceAvailable = effectiveWindow - bytesInFlight;
    uint64_t payloadSize = min( spaceAvailable - msg.SYN, TCPConfig::MAX_PAYLOAD_SIZE );
    
    //read payload from stream if available
    if (payloadSize > 0 && reader().bytes_buffered() > 0) {
      uint64_t bytesToRead = min( payloadSize, reader().bytes_buffered() );
      read( reader(), bytesToRead, msg.payload );
    }
    
    //send fin if stream ended and we have space
    if ( !finSent && reader().is_finished() ) {
      uint64_t seqLength = msg.SYN + msg.payload.size();
      if ( seqLength < spaceAvailable ) {
        msg.FIN = true;
        finSent = true;
      }
    }
    uint64_t segLength = msg.sequence_length();
    //if segment is empty, stop
    if (segLength == 0) 
      break;
    transmit(msg);
    
    //track as outstanding if it occupies sequence numbers
    outstandingSegments.push(msg);
    outstandingSeqnos += segLength;
    nextSeqno += segLength;
    bytesInFlight += segLength;
    
    //start retransmission timer if not running
    if (!timerRunning) {
      timerRunning = true;
      timerMs = 0;
    }
    if ( msg.FIN ) {
      break;
    }
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage msg;
  msg.seqno = Wrap32::wrap( nextSeqno, isn_ );
  msg.RST = reader().has_error();  //set rst if error
  return msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if (msg.RST) {
    reader().set_error();
    return;
  }
  windowSize = msg.window_size;
  if ( msg.ackno.has_value() ) {
    uint64_t acknoAbs = msg.ackno.value().unwrap(isn_, nextSeqno);
    
    //ignore invalid acknos (acknowledging something we haven't sent)
    if ( acknoAbs > nextSeqno ) {
      return;
    }
    
    //check if this acknowledges new data
    bool newDataAcked = acknoAbs > ackedSeqno;
    ackedSeqno = acknoAbs;
    
    //remove fully acknowledged segments from outstanding queue
    while (!outstandingSegments.empty()) {
      const auto& segment = outstandingSegments.front();
      uint64_t segSeqno = segment.seqno.unwrap( isn_, nextSeqno );
      uint64_t segEnd = segSeqno + segment.sequence_length();
      
      //if segment fully acknowledged, remove from queue
      if (segEnd <= ackedSeqno) {
        outstandingSeqnos -= segment.sequence_length();
        outstandingSegments.pop();
      } else 
          break;  
    }
    
    //if new data was acknowledged
    if (newDataAcked) {
      RTOms = initial_RTO_ms_;
      consecutiveRetransmissions = 0;
      
      //restart timer if there's still outstanding data
      if (!outstandingSegments.empty()) {
        timerRunning = true;
        timerMs = 0;
      } else {
        //stop timer if no outstanding data
        timerRunning = false;
      }
    }
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  //update timer
  timerMs += ms_since_last_tick;
  
  //checking if timer expired
  if (timerRunning && timerMs >= RTOms) {
    //retransmit 
    if (!outstandingSegments.empty()) {
      transmit(outstandingSegments.front());
      
      //if window size is nonzero, apply exponential backoff
      if (windowSize > 0) {
        consecutiveRetransmissions++;
        RTOms *= 2;  //double the RTO
      }
      //reset and restart
      timerMs = 0;
      timerRunning = true;
    }
  }
}