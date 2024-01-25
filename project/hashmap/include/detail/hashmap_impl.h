template <typename K, typename V, typename H, typename E>
template <typename OK, typename OV>
std::pair<std::optional<std::pair<K, V>>, typename hashmap<K, V, H, E>::iterator> hashmap<K, V, H, E>::swap(OK&& key, OV&& value)
{
    return swap(std::make_pair(std::forward<OK>(key), std::forward<OV>(value)));
}

template <typename K, typename V, typename H, typename E>
std::pair<std::optional<std::pair<K, V>>, typename hashmap<K, V, H, E>::iterator> hashmap<K, V, H, E>::swap(std::pair<K, V>&& value_pair)
{
    if (buckets.empty() || length > 3 * buckets.size() / 4)
        resize();
    return swap_inner(std::forward<std::pair<K, V>>(value_pair));
}

template <typename K, typename V, typename H, typename E>
std::pair<std::optional<std::pair<K, V>>, typename hashmap<K, V, H, E>::iterator> hashmap<K, V, H, E>::swap_inner(std::pair<K, V>&& value_pair)
{
    E equals;
    const auto idx = get_idx(value_pair.first);
    auto& bucket = buckets[idx];
    auto it = bucket.begin(), itEnd = bucket.end();
    auto element = std::find_if(it, itEnd, [&equals, &value_pair](std::pair<K, V> const& stored) {
        return equals(stored.first, value_pair.first);
    });

    const bool found = element != bucket.end();
    if (found) {
        std::swap(value_pair, *element);
    } else {
        ++length;
        bucket.emplace_back(std::forward<std::pair<K, V>>(value_pair));
    }

    return std::make_pair(!found ? std::nullopt : std::forward<std::pair<K, V>>(value_pair), iterator(buckets, idx, found ? std::distance(it, element) : 0));
}

template <typename K, typename V, typename H, typename E>
template <typename OK, typename OV>
std::pair<bool, typename hashmap<K, V, H, E>::iterator> hashmap<K, V, H, E>::insert(OK&& key, OV&& value)
{
    return insert(std::make_pair(std::forward<OK>(key), std::forward<OV>(value)));
}

template <typename K, typename V, typename H, typename E>
std::pair<bool, typename hashmap<K, V, H, E>::iterator> hashmap<K, V, H, E>::insert(std::pair<K, V>&& value_pair)
{
    if (buckets.empty() || length > 3 * buckets.size() / 4)
        resize();
    return insert_inner(std::forward<std::pair<K, V>>(value_pair));
}

template <typename K, typename V, typename H, typename E>
std::pair<bool, typename hashmap<K, V, H, E>::iterator> hashmap<K, V, H, E>::insert_inner(std::pair<K, V>&& value_pair)
{
    E equals;
    const auto idx = get_idx(value_pair.first);
    auto& bucket = buckets[idx];
    auto it = bucket.begin(), itEnd = bucket.end();
    auto element = std::find_if(it, itEnd, [&equals, &value_pair](std::pair<K, V> const& stored) {
        return equals(stored.first, value_pair.first);
    });

    const bool found = element != bucket.end();
    if (found) {
        *element = std::forward<std::pair<K, V>>(value_pair);
    } else {
        ++length;
        bucket.emplace_back(std::forward<std::pair<K, V>>(value_pair));
    }

    return std::make_pair(!found, iterator(buckets, idx, found ? std::distance(it, element) : 0));
}

template <typename K, typename V, typename H, typename E>
template <typename OK>
std::optional<V> hashmap<K, V, H, E>::pop(OK&& key)
{
    return pop_inner(std::forward<OK>(key));
}

template <typename K, typename V, typename H, typename E>
template <typename T>
void hashmap<K, V, H, E>::erase(base_iterator<T> const& key)
{
    auto& bucket = buckets[key.get_pos()];
    auto iter = bucket.begin();
    std::advance(iter, key.get_inner_pos());
    std::iter_swap(iter, bucket.rbegin());
    bucket.pop_back();
    --length;
    if (buckets.empty() || length > 3 * buckets.size() / 4)
        resize();
}

template <typename K, typename V, typename H, typename E>
template <typename OK>
std::optional<V> hashmap<K, V, H, E>::pop_inner(OK&& key)
{
    auto val = cfind(std::forward<OK>(key));
    if (val == cend())
        return std::nullopt;

    auto pop_value = std::move(val->second);
    erase(val);
    return pop_value;
}

template <typename K, typename V, typename H, typename E>
std::size_t hashmap<K, V, H, E>::get_hash(K const& key) const noexcept
{
    return H {}(key);
}

template <typename K, typename V, typename H, typename E>
template <typename OK>
std::size_t hashmap<K, V, H, E>::get_idx(OK const& key) const noexcept
{
    return get_hash(key) % buckets.size();
}

template <typename K, typename V, typename H, typename E>
hashmap<K, V, H, E>::hashmap(std::size_t capacity)
{
    buckets.reserve(capacity);
}

template <typename K, typename V, typename H, typename E>
void hashmap<K, V, H, E>::resize()
{
    const auto num_buckets = [](auto const length) noexcept -> std::size_t {
        if (length == 0)
            return 32;

        return length * 2;
    }(length);

    std::vector<std::pair<K, V>> inner;
    inner.reserve(1);
    std::vector<std::vector<std::pair<K, V>>> local_buckets(num_buckets, inner);
    std::swap(local_buckets, buckets);
    length = 0;
    for (auto& bucket : local_buckets) {
        for (auto&& element : bucket)
            insert_inner(std::move(element));
    }
}

template <typename K, typename V, typename H, typename E>
std::size_t hashmap<K, V, H, E>::size() const noexcept
{
    return length;
}

template <typename K, typename V, typename H, typename E>
bool hashmap<K, V, H, E>::is_empty() const noexcept
{
    return length == 0;
}

template <typename K, typename V, typename H, typename E>
template <typename OK>
typename hashmap<K, V, H, E>::iterator hashmap<K, V, H, E>::find(OK&& key) const
{
    return base_iterator<iterator>::iterator_from_const(
        buckets, cfind(std::forward<OK>(key)));
}

template <typename K, typename V, typename H, typename E>
template <typename OK>
typename hashmap<K, V, H, E>::const_iterator hashmap<K, V, H, E>::cfind(
    OK&& key) const
{
    E equals;
    const auto bucket_idx = get_idx(key);
    auto it = const_iterator(buckets, bucket_idx, 0);
    return std::find_if(it, cend(),
        [&equals, &key](const auto& pair) { return equals(pair.first, key); });
}

template <typename K, typename V, typename H, typename E>
typename hashmap<K, V, H, E>::iterator hashmap<K, V, H, E>::begin() const
{
    return iterator(buckets, 0, 0);
}

template <typename K, typename V, typename H, typename E>
typename hashmap<K, V, H, E>::iterator hashmap<K, V, H, E>::end() const
{
    return iterator(buckets, buckets.size(), 0);
}

template <typename K, typename V, typename H, typename E>
typename hashmap<K, V, H, E>::const_iterator hashmap<K, V, H, E>::cbegin() const
{
    return const_iterator(buckets, 0, 0);
}

template <typename K, typename V, typename H, typename E>
typename hashmap<K, V, H, E>::const_iterator hashmap<K, V, H, E>::cend() const
{
    return const_iterator(buckets, buckets.size(), 0);
}

template <typename K, typename V, typename H, typename E>
template <typename T>
bool hashmap<K, V, H, E>::base_iterator<T>::operator==(
    const base_iterator<T>& b) const
{
    return position == b.position && inner_position == b.inner_position;
};

template <typename K, typename V, typename H, typename E>
template <typename T>
bool hashmap<K, V, H, E>::base_iterator<T>::operator!=(
    const base_iterator<T>& b) const
{
    return !(*this == b);
};

template <typename K, typename V, typename H, typename E>
template <typename T>
hashmap<K, V, H, E>::base_iterator<T>::base_iterator(
    std::vector<std::vector<std::pair<K, V>>>& buckets, std::size_t pos,
    std::size_t inner)
    : position(pos)
    , inner_position(inner)
{
    for (; position < buckets.size() && buckets[position].size() == 0;
         ++position)
        ;
    if (position < buckets.size())
        inner_position = std::min(inner_position, buckets[position].size());
}

template <typename K, typename V, typename H, typename E>
template <typename T>
inline std::size_t
hashmap<K, V, H, E>::base_iterator<T>::get_pos() const noexcept
{
    return position;
}

template <typename K, typename V, typename H, typename E>
template <typename T>
inline std::size_t
hashmap<K, V, H, E>::base_iterator<T>::get_inner_pos() const noexcept
{
    return inner_position;
}

template <typename K, typename V, typename H, typename E>
template <typename T>
T& hashmap<K, V, H, E>::base_iterator<T>::operator++()
{
    T& ret_val = *static_cast<T*>(this);
    const auto& buckets = ret_val.get_buckets();
    auto inner_it = buckets[position].cbegin();
    std::advance(inner_it, ++inner_position);
    if (inner_it == buckets[position].cend()) {
        inner_position = 0;
        auto it = buckets.cbegin();
        std::advance(it, ++position);
        for (auto it_end = buckets.cend(); it != it_end && it->size() == 0;
             ++it, ++position)
            ;
    }
    return ret_val;
}

template <typename K, typename V, typename H, typename E>
template <typename T>
T& hashmap<K, V, H, E>::base_iterator<T>::operator++(int)
{
    T tmp = *static_cast<T*>(this);
    T& This = *static_cast<T*>(this);
    ++This;
    return tmp;
}

#undef SET_FN
#define SET_FN(theclass)                                                          \
    template <typename K, typename V, typename H, typename E>                     \
    hashmap<K, V, H, E>::theclass ::theclass(                                     \
        std::vector<std::vector<std::pair<K, V>>>& buckets, std::size_t position, \
        std::size_t inner_position)                                               \
        : base_iterator<theclass>(buckets, position, inner_position)              \
        , buckets(buckets)                                                        \
    {                                                                             \
    }

SET_FN(iterator);
SET_FN(const_iterator);

#undef SET_FN
#define SET_FN(class)                                                 \
    template <typename K, typename V, typename H, typename E>         \
    typename hashmap<K, V, H, E>::class ::reference                   \
        hashmap<K, V, H, E>::class ::operator*() const {              \
        return buckets.get()[base_iterator<class>::get_pos()]         \
                            [base_iterator<class>::get_inner_pos()];  \
    } template <typename K, typename V, typename H, typename E>       \
    typename hashmap<K, V, H, E>::class ::pointer                     \
        hashmap<K, V, H, E>::class ::operator->() const {             \
        return &buckets.get()[base_iterator<class>::get_pos()]        \
                             [base_iterator<class>::get_inner_pos()]; \
    }

SET_FN(iterator);
SET_FN(const_iterator);

#undef SET_FN
#define SET_FN(class)                                         \
    template <typename K, typename V, typename H, typename E> \
    const std::vector<std::vector<std::pair<K, V>>>&          \
        hashmap<K, V, H, E>::class ::get_buckets() const      \
    {                                                         \
        return buckets.get();                                 \
    }

SET_FN(iterator);
SET_FN(const_iterator);

#undef SET_FN
