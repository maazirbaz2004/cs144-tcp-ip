Checkpoint 1 Writeup
====================

My name: [Mohib]

My SUNet ID: [23L-0650]

I collaborated with: [23L-0728]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about [5] hours to do. I [did/did not] attend the lab session.


Report from the hands-on component of the lab checkpoint: [include
information from 2.1(4), and report on your experience in 2.2]

Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

what I did: 
I used a std::map to store out of order chunks keyed by their starting index. 
This made it easy to keep the data ordered and merge overlapping pieces before 
writing them in sequence to the output stream. I considered alternatives like arrays,
but the map approach was simpler to implement and less chances of errror. 
At first I tried using a deque to manage chunks as they arrived.
This felt simpler but failed because data can arrive out of order.
With a deque, searching and merging overlapping chunks was messy.
It also made passing the tests harder since gaps weren’t handled well, which is why I 
decided that maps were the best way to deal with this problem.
Despite maps being more intuitive and easy to work with, a downside to them 
is that it’s not that memory efficient so that could be a disadvantage of it.

Implementation Challenges:
Merging overlapping chunks was the hardest part to get correct.
I also had to be careful with trimming data to match capacity.
Handling the EOF case at exactly the right moment was difficult too.

Remaining Bugs:
At the moment my code passes all the tests so I don’t see major bugs.


- Optional: I had unexpected difficulty with: thinking in stream indexes.

- Optional: I think you could make this lab better by: 

- Optional: I'm not sure about: [describe]
