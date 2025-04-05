export module utility.helpers;

import <vector>;
import <algorithm>;

export namespace utility::Helpers {
  template<typename T>
  bool FindAndErase(std::vector<T>& vec, T& element) {
    auto it = std::find(vec.begin(), vec.end(), element);

    if (it == vec.end()) return false;
    
    vec.erase(it);
    return true;
  }

  template<typename T>
  bool ReverseFindAndErase(std::vector<T>& vec, const T& element) {
    auto it = std::find(vec.rbegin(), vec.rend(), element);

    if (it == vec.rend()) return false;

    // Convert reverse iterator to regular iterator
    vec.erase(std::next(it).base());
    return true;
  }

  template<typename T>
  void SwapAndPop(std::vector<T>& vec, size_t index) {
    if (index < vec.size()) {
      std::swap(vec[index], vec.back());
      vec.pop_back();
    }
  }
};