#pragma once
#include <algorithm>
#include <optional>
#include <vector>

namespace my {
template <typename K, typename V, typename H = std::hash<K>,
    typename E = std::equal_to<K>>
struct hashmap {
private:
    template <typename T>
    struct base_iterator;

public:
    hashmap() = default;
    hashmap(std::size_t capacity);

    void resize();

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool is_empty() const noexcept;

    struct iterator;
    struct const_iterator;

    template <typename OK, typename OV>
    std::pair<bool, typename hashmap<K, V, H, E>::iterator> insert(OK&& key, OV&& value);
    std::pair<bool, typename hashmap<K, V, H, E>::iterator> insert(std::pair<K, V>&& value_pair);

    template <typename OK, typename OV>
    std::pair<std::optional<std::pair<K, V>>, typename hashmap<K, V, H, E>::iterator> swap(OK&& key, OV&& value);
    std::pair<std::optional<std::pair<K, V>>, typename hashmap<K, V, H, E>::iterator> swap(std::pair<K, V>&& value_pair);

    template <typename OK>
    [[nodiscard]] iterator find(OK&& key) const;
    template <typename OK>
    [[nodiscard]] const_iterator cfind(OK&& key) const;
    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;
    [[nodiscard]] const_iterator cbegin() const;
    [[nodiscard]] const_iterator cend() const;

    template <typename OK>
    std::optional<V> pop(OK&& key);

    template <typename T>
    void erase(base_iterator<T> const& key);

    struct const_iterator : public base_iterator<const_iterator> {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const std::pair<K, V>;
        using pointer = const std::pair<K, V>*;
        using reference = const std::pair<K, V>&;

        const_iterator(std::vector<std::vector<std::pair<K, V>>>& buckets,
            std::size_t position, std::size_t inner_position);

        [[nodiscard]] reference operator*() const;
        [[nodiscard]] pointer operator->() const;

    private:
        friend class base_iterator<const_iterator>;
        [[nodiscard]] const std::vector<std::vector<std::pair<K, V>>>& get_buckets() const;

        std::reference_wrapper<const std::vector<std::vector<std::pair<K, V>>>>
            buckets;
    };

    struct iterator : public base_iterator<iterator> {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<K, V>;
        using pointer = std::pair<K, V>*;
        using reference = std::pair<K, V>&;

        iterator(std::vector<std::vector<std::pair<K, V>>>& buckets,
            std::size_t position, std::size_t inner_position);

        [[nodiscard]] reference operator*() const;
        [[nodiscard]] pointer operator->() const;

    private:
        friend class base_iterator<iterator>;
        [[nodiscard]] const std::vector<std::vector<std::pair<K, V>>>& get_buckets() const;

        std::reference_wrapper<std::vector<std::vector<std::pair<K, V>>>> buckets;
    };

private:
    template <typename T>
    struct base_iterator {
        T& operator++();
        T& operator++(int);

        bool operator==(const base_iterator<T>& b) const;
        bool operator!=(const base_iterator<T>& b) const;

    protected:
        base_iterator(
            std::vector<std::vector<std::pair<K, V>>>&, std::size_t, std::size_t);
        ~base_iterator() = default;

        friend class hashmap<K, V, H, E>;
        [[nodiscard]] inline std::size_t get_pos() const noexcept;
        [[nodiscard]] inline std::size_t get_inner_pos() const noexcept;

        [[nodiscard]] static iterator iterator_from_const(
            std::vector<std::vector<std::pair<K, V>>>& buckets,
            const_iterator const& cit)
        {
            return iterator(buckets, cit.get_pos(), cit.get_inner_pos());
        }

    private:
        std::size_t position;
        std::size_t inner_position;
    };

    std::pair<bool, typename hashmap<K, V, H, E>::iterator> insert_inner(std::pair<K, V>&& value_pair);
    std::pair<std::optional<std::pair<K, V>>, typename hashmap<K, V, H, E>::iterator> swap_inner(std::pair<K, V>&& value_pair);
    template <typename OK>
    [[nodiscard]] std::optional<V> pop_inner(OK&& key);
    [[nodiscard]] std::size_t get_hash(K const& key) const noexcept;
    template <typename OK>
    [[nodiscard]] std::size_t get_idx(OK const& key) const noexcept;

    mutable std::vector<std::vector<std::pair<K, V>>> buckets;
    std::size_t length = 0;
};
#include "detail/hashmap_impl.h"
} // namespace my
