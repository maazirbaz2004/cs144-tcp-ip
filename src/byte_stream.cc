#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_(capacity), error_(false), buffer_(), read_pos_(0),
bytes_popped_(0), bytes_pushed_(0), closed_(false) {}

void Writer::push( string data )
{
  if (is_closed()) return; //to ensure it doesn't try to push if already closed

  uint64_t avail = capacity_ - (buffer_.size() - read_pos_);
  uint64_t to_push = min((uint64_t)data.size(), avail); //only pushing the smallest of the two

  buffer_.insert(buffer_.end(), data.begin(), data.begin() + to_push);
  bytes_pushed_ += to_push;
}

void Writer::close()
{

  closed_ = true;
}

bool Writer::is_closed()  const
{
  return {closed_}; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return {capacity_ - (buffer_.size() - read_pos_)}; // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return {bytes_pushed_}; // Your code here.
}

string_view Reader::peek() const
{
  // Your code here.
  if (read_pos_ >= buffer_.size()) return {};

  return string_view(&buffer_[read_pos_], buffer_.size() - read_pos_);
}

void Reader::pop( uint64_t len )
{
  uint64_t remaining = buffer_.size() - read_pos_;
  uint64_t to_pop = min(len, remaining);

  read_pos_ += to_pop;
  bytes_popped_ += to_pop;

  if (read_pos_ > buffer_.size() / 2) 
  {
      buffer_.erase(buffer_.begin(), buffer_.begin() + read_pos_);
      read_pos_ = 0;
  }
}

bool Reader::is_finished() const
{
  return {closed_ && (read_pos_ >= buffer_.size())}; // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return {buffer_.size() - read_pos_}; // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return {bytes_popped_}; // Your code here.
}

