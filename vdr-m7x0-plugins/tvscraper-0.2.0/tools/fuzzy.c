#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
 
template<typename T, typename C>
size_t
seq_distance(const T& seq1, const T& seq2, const C& cost,
             const typename T::value_type& empty = typename T::value_type()) {
  const size_t size1 = seq1.size();
  const size_t size2 = seq2.size();
 
  std::vector<size_t> curr_col(size2 + 1);
  std::vector<size_t> prev_col(size2 + 1);
 
  // Prime the previous column for use in the following loop:
  prev_col[0] = 0;
  for (size_t idx2 = 0; idx2 < size2; ++idx2) {
    prev_col[idx2 + 1] = prev_col[idx2] + cost(empty, seq2[idx2]);
  }
 
  for (size_t idx1 = 0; idx1 < size1; ++idx1) {
    curr_col[0] = curr_col[0] + cost(seq1[idx1], empty);
 
    for (size_t idx2 = 0; idx2 < size2; ++idx2) {
      curr_col[idx2 + 1] = std::min(std::min(
        curr_col[idx2] + cost(empty, seq2[idx2]),
        prev_col[idx2 + 1] + cost(seq1[idx1], empty)),
        prev_col[idx2] + cost(seq1[idx1], seq2[idx2]));
    }
 
    curr_col.swap(prev_col);
    curr_col[0] = prev_col[0];
  }
 
  return prev_col[size2];
}
 
size_t
letter_distance(char letter1, char letter2) {
  return letter1 != letter2 ? 1 : 0;
}
 
size_t
word_distance(const std::string& word1, const std::string& word2) {
  return seq_distance(word1, word2, &letter_distance);
}
 
size_t
sentence_distance(const std::string& sentence1, const std::string& sentence2) {
  std::vector<std::string> words1;
  std::vector<std::string> words2;
  std::istringstream iss1(sentence1);
  std::istringstream iss2(sentence2);
  for(std::istream_iterator<std::string> it(iss1), end; ; ) {
    words1.push_back(*it);
    if(++it == end)
      break;
    words1.push_back(" ");
  }
  for(std::istream_iterator<std::string> it(iss2), end; ; ) {
    words2.push_back(*it);
    if(++it == end)
      break;
    words2.push_back(" ");
  }
  return seq_distance(words1, words2, &word_distance);
}