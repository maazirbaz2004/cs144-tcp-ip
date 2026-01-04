Checkpoint 2 Writeup
====================

My name: [Mohib]

My SUNet ID: [23L-0650]

I collaborated with: [23L-0728]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about [4] hours to do. I [did] attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

Implementation Challenges:

> when writing the unwrap function in wrapping_integers.cc I had 
much difficulty in trying to understand the core logic behind how to find 
the absolute sequence number that is closest to the checkpoint because it could have
multiple options.
> implementing the wrap function was pretty straightforward though in comparison.
> test case 18 provided some difficulty to complete, and using signed diff instead 
of unsigned, helped avoid the wraparound bug and helped me pass this test case.
> moving onto the  tcp_receiver, the most difficult part was tracking the ISN 
> had to store it once SYN arrived and ignore it till data arrived
> Understanding how everything worked together in the tcp_receiver was also pretty 
difficult. 
> 2 diff structs i.e TCPSenderMessage & TCPReceiverMessage were used and had to 
cater their data members properly.
> e.g I wasn't passing a test case cause I forgot to handle the RST flag of the TCPReceiverMessage
object. 
> tests in this check had very tedious edge cases which proved difficult 
to pass at times.
> cause multiple failures and had to tweak the code to ensure SYN and FIN
were handled CAREFULLY!


Remaining Bugs:
[None]

- Optional: I had unexpected difficulty with: 
>completing the logic for the unwrap function. This took more time 
than I thought it would and was perhaps, the most difficult part of 
this check.

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: 
> the difficult test cases in this check. many edge cases that I had
not considered, were targetted in this check which helped me realise my mistakes

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: 
> Nah, I didn't. there were alr soo many!
