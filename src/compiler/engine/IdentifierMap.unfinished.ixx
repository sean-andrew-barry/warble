#pragma once

#include <vector>
#include <bit>
#include <optional>

namespace Core
{
  template <typename T>
  class IdentifierMap
  {
  public:
    using GUID = typename Core::GUID<T>;

    struct ValueNode
    {
      T value;       // The actual value
      int next = -1; // The index of the next value in the chain, or -1 if none

      ValueNode(T v) : value(std::move(v)), next(-1) {}
    };
  
  private:
    std::vector<int> keys;            // A vector representing the hash table's buckets, storing indices into values
    std::vector<ValueNode> values;    // Stores the actual values, along with chaining information

    uint32_t state;
    size_t size_mask;
    double max_load_factor;

  private:
    // Helper function to compute the next power of 2 greater or equal to n
    uint64_t UpperPowerOf2(uint64_t n)
    {
      return std::bit_ceil(n);
    }

    // Helper function to compute the next power of 2 greater than n
    uint64_t NextPowerOf2(uint64_t n)
    {
      if (n == 0) return 1;
      return std::bit_ceil(n + 1);
    }

    // Hash function that computes an index from the GUID
    size_t Hash(GUID guid) const
    {
      // size_mask + 1 must be a power of 2
      return (guid.Get() - 1) & size_mask;
    }

  public:
    // Default constructor initializing with 8 buckets
    IdentifierMap() : IdentifierMap{8} {}

    // Constructor that initializes with a specified capacity
    IdentifierMap(size_t capacity)
      : keys(UpperPowerOf2(capacity), -1), values{}, state{1}, size_mask{keys.size() - 1}, max_load_factor{1.0}
    {
      values.reserve(keys.size());
    }

    // Rehashes the table when the load factor exceeds the maximum
    void Rehash()
    {
      if (values.size() > max_load_factor * keys.size())
      {
        auto new_size = NextPowerOf2(keys.capacity());
        keys.assign(new_size, -1);
        size_mask = new_size - 1;

        // Reassign all values to new buckets
        for (int i = 0; i < static_cast<int>(values.size()); ++i)
        {
          auto guid = values[i].value.GetGUID();
          size_t keyIndex = Hash(guid);

          if (keys[keyIndex] == -1)
          {
            keys[keyIndex] = i;
          }
          else
          {
            // Chain the new value to the existing one
            int current = keys[keyIndex];
            while (values[current].next != -1)
            {
              current = values[current].next;
            }
            values[current].next = i;
          }
        }
      }
    }

    // Emplace a new value and return its GUID
    template <typename... Args>
    GUID Emplace(Args&&... args)
    {
      ++state;
      auto& valueNode = values.emplace_back(T(std::forward<Args>(args)...));
      auto guid = valueNode.value.GetGUID();
      size_t keyIndex = Hash(guid);

      if (keys[keyIndex] == -1)
      {
        keys[keyIndex] = static_cast<int>(values.size() - 1);
      }
      else
      {
        // Chain the new value to the existing one
        int current = keys[keyIndex];
        while (values[current].next != -1)
        {
          current = values[current].next;
        }
        values[current].next = static_cast<int>(values.size() - 1);
      }

      // Rehash if load factor is exceeded
      if (values.size() > max_load_factor * keys.size())
      {
        Rehash();
      }

      return guid;
    }

    // Insert a copy of an existing value and return its GUID
    GUID Insert(const T& t)
    {
      ++state;
      auto& valueNode = values.emplace_back(t);
      auto guid = valueNode.value.GetGUID();
      size_t keyIndex = Hash(guid);

      if (keys[keyIndex] == -1)
      {
        keys[keyIndex] = static_cast<int>(values.size() - 1);
      }
      else
      {
        // Chain the new value to the existing one
        int current = keys[keyIndex];
        while (values[current].next != -1)
        {
          current = values[current].next;
        }
        values[current].next = static_cast<int>(values.size() - 1);
      }

      // Rehash if load factor is exceeded
      if (values.size() > max_load_factor * keys.size())
      {
        Rehash();
      }

      return guid;
    }

    // Find a value by GUID and return a pointer, or nullptr if not found
    T* Find(GUID guid)
    {
      size_t keyIndex = Hash(guid);
      int current = keys[keyIndex];

      while (current != -1)
      {
        if (values[current].value.GetGUID() == guid)
        {
          return &values[current].value;
        }
        current = values[current].next;
      }
      return nullptr;
    }

    // Find a value using a cached key and return a pointer, or nullptr if not found
    T* Find(CachedKey<GUID, T>& cache)
    {
      if (cache.GetState() == state)
      {
        return cache.GetValue();
      }
      else
      {
        size_t keyIndex = Hash(cache.GetKey());
        int current = keys[keyIndex];

        while (current != -1)
        {
          if (values[current].value.GetGUID() == cache.GetKey())
          {
            cache.SetState(state);
            cache.SetValue(&values[current].value);
            return &values[current].value;
          }
          current = values[current].next;
        }
      }
      return nullptr;
    }
  };
};
