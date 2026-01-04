#include "reassembler.hh"
#include "debug.hh"

using namespace std;
//helper
void Reassembler::store_in_buffer(uint64_t first_index, string data) {
    uint64_t new_start = first_index;
    uint64_t new_end = first_index + data.size();

    //look for existing chunks that overlap
    auto it = buffer.lower_bound(first_index);
    if (it != buffer.begin()) {
        --it; // step back to check the previous chunk
    }

    while (it != buffer.end()) {
        uint64_t exist_start = it->first;
        uint64_t exist_end = exist_start + it->second.size();

        //debugged and fixed this error in check3: break if no overlap 
        if (exist_start > new_end)
            break;  

        //skip if this chunk is completely before new data
        if (exist_end < new_start) {
            ++it;
            continue;
        }

        //extend new_start/new_end to cover overlap
        new_start = min(new_start, exist_start);
        new_end   = max(new_end, exist_end);

        //merge strings (put both into one big buffer)
        string merged(new_end - new_start, '\0');
        for (size_t i = 0; i < data.size(); i++) {
            merged[first_index - new_start + i] = data[i];
        }
        for (size_t i = 0; i < it->second.size(); i++) {
            merged[exist_start - new_start + i] = it->second[i];
        }

        data = move(merged);
        first_index = new_start;

        it = buffer.erase(it); //remove the old chunk
    }

    buffer[first_index] = move(data);
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // if the substring is empty
  if (is_last_substring && data.empty() && first_index == next_index)
  {
      eof_seen = true;
      eof_index = first_index;   // =0 here
      writer().close();
      return;
  }
  // EOF
  if(is_last_substring)
  {
    eof_index= first_index + data.size();
    eof_seen=true;
  }

  // if entire substring is before next_index
  if(first_index + data.size()<=next_index)
  {
    return;
  }

  //removing already assembled part by cutting it from start
  if(first_index < next_index)
  {
    size_t s=next_index - first_index;
    data.erase(0,s);
    first_index=next_index;
  }
  //trim substring according to available capacity 
  size_t available_capac = writer().available_capacity();
  if(first_index >= next_index + available_capac)
    return;
  
  if(first_index - next_index + data.size() > available_capac)
  {
    size_t keep = available_capac - (first_index - next_index);
    data = data.substr(0, keep);
  }
  
  //store in buffer
  store_in_buffer(first_index,data);

  // if buffer has substring at next_index it moves it into bytestream and updates next_index. Also erases it from buffer
  auto it = buffer.find(next_index);
  while (it != buffer.end()) 
  {
    writer().push(it->second);
    next_index += it->second.size();
    it = buffer.erase(it);
    it = buffer.find(next_index); // check next piece
  }
  // closes if reached eof
  if (eof_seen && next_index >= eof_index) {writer().close();}

}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
    uint64_t total = 0;
    for (const auto &entry : buffer) {
        total += entry.second.size();
    }
    return total;
}

