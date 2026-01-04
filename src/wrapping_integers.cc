#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  //basically doing (n + zero point.raw_val)mod 2^32
  return Wrap32 { static_cast<uint32_t>( ( n + zero_point.raw_value_) & 0xFFFFFFFF ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint64_t wrap_size = 1ULL << 32;
  int64_t half_wrap  = static_cast<int64_t>(wrap_size >> 1);

  uint32_t offset = raw_value_ - zero_point.raw_value_;
  uint64_t high_bits = checkpoint & 0xFFFFFFFF00000000ULL; //setting all the bottom bits to 0
  uint64_t choice = high_bits | offset; //choice has first half from checkpoint and second half from offset

  //using signed fixed the problem of test case 18 
  int64_t diff = static_cast<int64_t>(choice) - static_cast<int64_t>(checkpoint);

  if (diff > half_wrap && choice >= wrap_size) 
      choice -= wrap_size;
  else if (diff < -half_wrap)
      choice += wrap_size;

  return choice;
}
