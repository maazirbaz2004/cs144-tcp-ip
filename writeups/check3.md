Checkpoint 3 Writeup
====================

My name: [Mohib]

My SUNet ID: [23L-0650]

I collaborated with: [23L-0728]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [8] hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the TCPSender [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: []

> Used a queue to track outstanding segments that haven't been acknowledged yet.
This made it simple to retransmit the oldest segment when the timer expired.
> Stored tracking variables like nextSeqno, ackedSeqno, windowSize to manage the sender state.
> Implemented the retransmission timer logic directly in the TCPSender class rather than
creating a separate timer class.
> The push method fills the receiver's window by reading from the ByteStream and creating
segments with SYN/FIN flags as needed. It respects MAX_PAYLOAD_SIZE and treats zero window
as size 1 for probing.
> The receive() method processes acknowledgments and removes fully acknowledged segments
from the outstanding queue. It also handles the exponential backoff logic properly.
> Alternative design I considered was using a deque or vector for outstanding segments,
but queue was cleaner since we only ever need to access the front element for retransmission.


Report from the hands-on component: []
> Successfully tested my TCP implementation against real Internet servers using webget
> My implementation was able to fetch web pages from cs144.keithw.org successfully
> The connection completed cleanly with proper acknowledgment of all data

Implementation Challenges:
[]
> Understanding the retransmission timer logic from section 2.1 was tricky at first.
Had to read through the rules multiple times to get the exponential backoff right.
> The zero window size special case caught me off guard initially. 
> Figuring out when to start/stop/restart the timer took some debugging. Had to ensure
the timer only runs when there's outstanding data and gets reset properly on new acks.
> Initially forgot to handle the RST flag in both directions (incoming and outgoing).
Got test failures until I added RST handling in receive() and make_empty_message().
> Had to be careful about the difference between "outstanding segments" and "empty segments".
Only segments with sequence length > 0 should be tracked and retransmitted.


Remaining Bugs:
[None that I'm aware of, all tests pass]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
