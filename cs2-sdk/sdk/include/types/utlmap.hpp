#pragma once

#include <optional>

template <typename K, typename V>
class CUtlMap {
   public:
    struct Node_t {
        int m_left;
        int m_right;
        int m_parent;
        int m_tag;
        K m_key;
        V m_value;
    };

    auto begin() const { return m_Data; }
    auto end() const { return m_Data + m_Size; }

    std::optional<V> FindByKey(K key) const {
        int current = m_Root;
        while (current != -1) {
            const Node_t& element = m_Data[current];
            if (element.m_key < key)
                current = element.m_right;
            else if (element.m_key > key)
                current = element.m_left;
            else
                return element.m_value;
        }
        return {};
    }

    char pad_0[8];
    Node_t* m_Data;
    int m_nAllocationCount;
    int m_nGrowSize;

    int m_Root;
    int m_NumElements;
    int m_FirstFree;
    int m_Size;
};
