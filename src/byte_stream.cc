#include "byte_stream.hh"
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), buffer( capacity, '\0' )
{
  buffer.clear();
}

bool Writer::is_closed() const
{
  return this->is_close;
}

void Writer::push( string data )
{
  // Your code here.
  if ( this->error_ || this->is_closed() || ( this->available_capacity() <= 0 ) ) {
    return;
  }

  uint64_t space_left = this->capacity_ - this->pushed;
  if ( data.size() > space_left ) {
    data = data.substr( 0, space_left );
  }
  this->buffer.append( data );
  this->pushed += data.size();
  this->pushed_total += data.size();
}

void Writer::close()
{
  this->is_close = true;
  // Your code here.
}

uint64_t Writer::available_capacity() const
{

  return this->capacity_ - this->pushed;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return this->pushed_total;
}

bool Reader::is_finished() const
{
  return this->is_close && ( this->pushed == 0 );
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return this->poped_total;
}

string_view Reader::peek() const
{
  if ( this->is_finished() )
    return {};
  return std::string_view( this->buffer.data(), this->pushed );
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if ( this->is_finished() ) {
    return;
  }
  if ( len > this->pushed ) {
    len = this->pushed;
  }
  this->buffer.erase( 0, len );
  this->pushed -= len;
  this->poped_total += len;
}

uint64_t Reader::bytes_buffered() const
{
  return this->pushed;
}
