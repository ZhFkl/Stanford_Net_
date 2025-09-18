#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
      // Your code here.
      if(data.empty()){
        if(is_last_substring){
          is_last_ = true;
          this->check_and_close();
        }
        return;
      }
      
      const uint64_t start = first_index;
      const uint64_t end = first_index + data.size() - 1;
      const uint64_t max_capacity = output_.writer().bytes_pushed() + output_.writer().available_capacity() - 1;
      if(end < output_.writer().bytes_pushed()|| start > max_capacity){
        return ;
      }

      uint64_t trunncated_end = min(end,max_capacity);
      uint64_t trunncated_start = start;
      string truncated_data = data;

      if(end > max_capacity){
        const size_t truncated_size = max_capacity - start + 1;
        truncated_data = data.substr(0,truncated_size);
        is_last_substring = false;
      }
      //
      if(is_last_substring){
        is_last_ = true;
        last_index = trunncated_end;
      }
      //first condition
      if(trunncated_start > next_index){
        this->add_to_buffer(trunncated_start,truncated_data);
        return;
      }
      const size_t offset = next_index - trunncated_start;
      const string data_to_write = truncated_data.substr(offset);
      output_.writer().push(data_to_write);
      next_index += data_to_write.size();
      
      this->flush_buffer_to_output();

      this->check_and_close();

}


uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  uint64_t total = 0;
  for(const auto& p : buffer_){
    total += p.second.size();
  }
  return total;
}

void Reassembler::add_to_buffer(uint64_t start,const string& data){
  if(data.empty()){
    return;
  }
  uint64_t end = start + data.size() -1;
  auto it = buffer_.find(start);
  if( it != buffer_.end() ){
    if(data.size() > it->second.size()){
      buffer_[start] = data;
      it = buffer_.find(start);
    }else{
      return ;
    }
  }else{
    buffer_[start] = data;
    it = buffer_.find(start);
  }

  if(it != buffer_.begin()){
    auto prev = std::prev(it);
    uint64_t prev_end = prev->first + prev->second.size() -1;
    if(prev_end >= start -1){
      if(prev_end >= end){
        buffer_.erase(it);
        return ;
      }
      uint64_t offset = prev_end - start + 1;
      start = prev->first;
      end = max(end,prev_end);
      string merged = prev->second + data.substr(offset);
      buffer_.erase(prev);
      buffer_[start] = merged;
      it = buffer_.find(start);
    }
  }

  it++;


  while(it != buffer_.end() && static_cast<uint64_t>(it->first) <= static_cast<uint64_t>(end + 1)){
    uint64_t curr_end = it->first + it->second.size() -1;
    if(curr_end <= end){
      buffer_.erase(it);
      it = buffer_.find(start);
      ++it;
      continue;
    }
    uint64_t offset = end - it->first + 1;
    string merged = buffer_[start]  + it->second.substr(offset);
    end = max(end,curr_end);
    buffer_.erase(it);
    buffer_[start] = merged;
    it = buffer_.find(start);
    ++it;
  }


}


void Reassembler::flush_buffer_to_output(){
  //check if the buffer is empty
  if(buffer_.empty()){
    return;
  }
  //find the first segment
  while(!buffer_.empty()){
    auto it = buffer_.begin();
    uint64_t start = it->first;
    if(start > next_index){
      break;
    }
    const string &data = it->second;
    const size_t offset = next_index - start;
    if(offset >= data.size()){
      buffer_.erase(it);
      continue;
    }
    const string data_to_write = data.substr(offset);
    output_.writer().push(data_to_write);
    next_index += data_to_write.size();
    if(offset + data_to_write.size() < data.size()){
      // judge if there is remaining data
      buffer_[start + offset + data_to_write.size()] = data.substr(offset + data_to_write.size());
    }

    buffer_.erase(it);
}
}


void Reassembler::check_and_close(){
  if(is_last_ && next_index >= last_index){
    output_.writer().close();
  }
}


