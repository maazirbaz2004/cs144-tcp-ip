#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
    //set rst flag if error
    if (message.RST) {
        reassembler_.reader().set_error();
        return;
    }

    //set ISN if this is the first SYN segment
    if (message.SYN)
        ISN = message.seqno;
    if (!ISN.has_value()) 
        return;

    //convert seqno to absolute seqno then to stream index
    uint64_t checkpoint = writer().bytes_pushed();
    uint64_t AbsSeqno = message.seqno.unwrap(ISN.value(), checkpoint);
    uint64_t streamIndex = AbsSeqno + message.SYN - 1;

    //push to reassembler
    reassembler_.insert(streamIndex, message.payload, message.FIN);
}

TCPReceiverMessage TCPReceiver::send() const
{
    TCPReceiverMessage msg;

    //set window size(capped at UINT16_MAX as mentioned in file)
    uint64_t capacity = writer().available_capacity();
    msg.window_size = capacity > UINT16_MAX ? UINT16_MAX : static_cast<uint16_t>(capacity);

    //setting ackno 
    if (ISN.has_value()) {
        //ackno is the next byte we need (bytes_pushed + 1 (for SYN))
        uint64_t nextAbsSeqno = writer().bytes_pushed() + 1;
        if (writer().is_closed()) 
            nextAbsSeqno++;  //add 1 for FIN
        msg.ackno = Wrap32::wrap(nextAbsSeqno, ISN.value());
    }
    //check for error
    msg.RST = writer().has_error();
    return msg;
}
