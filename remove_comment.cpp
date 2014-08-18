#include <string>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <vector>

template<class I>
struct Range {
  I begin;
  I end;
  bool valid;
  bool is_exception;
  
  bool has_overlap(const Range &r) const{
    return valid && begin > r.begin && begin < r.end;
  }
};

template<class I>
bool operator<(const Range<I>& lhs, const Range<I>& rhs){
  return !rhs.valid || lhs.begin < rhs.begin;
}

struct MatchingParameters {
  std::string open_condition;
  std::string close_condition;
  std::string escape_sequence;
  bool is_exception;
};

template<class I>
Range<I> find_matching(const I begin, const I end,const MatchingParameters &params) {
  auto open = std::search(begin,end,params.open_condition.begin(),params.open_condition.end());
  auto close = open;
  while(close != end) {
    close = std::search(close+1,end,params.close_condition.begin(),params.close_condition.end());
    if(close == end){ return {end,end,false,params.is_exception}; }
    if (params.escape_sequence.size() > 0) {
      int escape_count = 0;
      while( std::equal(params.escape_sequence.begin(),params.escape_sequence.end(),
                        close-(escape_count+1)*params.escape_sequence.size(),
                        close-escape_count*params.escape_sequence.size())) { 
        ++escape_count; 
      }
      if(escape_count % 2 == 0) return {open,close + params.close_condition.size(),true,params.is_exception};
    } else {
      return {open,close + params.close_condition.size(),true,params.is_exception};
    }
  }
  return {end,end,false,params.is_exception};
}

template<class D>
D strip_comments(D data, const std::vector<MatchingParameters> &matches) {
  std::vector<Range<typename D::iterator>> ranges;
  
  for(const auto& match:matches) { 
    auto get = data.begin();
    while(get != data.end()) {
      auto a_match = find_matching(get,data.end(),match);
      if(!a_match.valid) {
        get = data.end();
      } else {
        ranges.emplace_back(a_match);
        get = a_match.end;
      }
    }
  }
  std::sort(ranges.begin(),ranges.end());
  
  auto get = data.begin();
  auto put = data.begin();

  for(auto it = ranges.begin(); it != ranges.end();
      it = std::find_if(it+1,ranges.end(),[it](const auto &val){ return !val.has_overlap(*it);})) {
    if(it->is_exception) {
      put += std::distance(get,it->end);
      std::copy_backward(get,it->end,put);
    } else {
      put += std::distance(get,it->begin);
      std::copy_backward(get,it->begin,put);
    }
    get = it->end;
  }
  put += std::distance(get,data.end());
  std::copy_backward(get,data.end(),put);
  data.erase(put,data.end());
  return data;
}
