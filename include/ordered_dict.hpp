#ifndef ORDERED_DICT_HPP
#define ORDERED_DICT_HPP

#include <vector>
#include <unordered_map>
#include <optional>

#include <string>
#include <stdexcept>


namespace json {

namespace utils {


template <class KeyType, class ValueType>
class OrderedDict {

private:
    std::unordered_map<KeyType, std::pair<ValueType, size_t>> dict;
    std::vector<std::optional<KeyType>> order;
    bool has_empty = false;

public:
    OrderedDict() {}
    OrderedDict(const std::vector<std::pair<KeyType, ValueType>>& key_value_pairs) {
        for (const auto& key_value: key_value_pairs) insert(key_value.first, key_value.second);
    }
    OrderedDict(const OrderedDict& ordered_dict) {
        if (!ordered_dict.has_empty) {
            dict = ordered_dict.dict;
            order = ordered_dict.order;
        }
        else {
            for (const auto& opt_key: ordered_dict.order) {
                if (opt_key.has_value()) {
                    const auto& key = opt_key.value();
                    insert(key, ordered_dict.at(key));
                }
            }
        }
    }

    const size_t size() const { return dict.size(); }
    void clear() {
        dict.clear();
        order.clear();
        has_empty = false;
    }

    OrderedDict& operator = (const std::vector<std::pair<KeyType, ValueType>>& key_value_pairs) {
        this->clear();
        for (const auto& key_value: key_value_pairs) insert(key_value.first, key_value.second);
        return *this;
    }
    OrderedDict& operator = (const OrderedDict& ordered_dict) {
        this->clear();
        if (!ordered_dict.has_empty) {
            dict = ordered_dict.dict;
            order = ordered_dict.order;
        }
        else {
            for (const auto& opt_key: ordered_dict.order) {
                if (opt_key.has_value()) {
                    const auto& key = opt_key.value();
                    insert(key, ordered_dict.at(key));
                }
            }
        }
        return *this;
    }

    void swap(OrderedDict& ordered_dict) {
        std::swap(this->has_empty, ordered_dict.has_empty);
        this->dict.swap(ordered_dict.dict);
        this->order.swap(ordered_dict.order);
    }

    void insert(const KeyType& key, const ValueType& value) {
        if (dict.find(key) == dict.end()) {
            dict[key] = {value, order.size()};
            order.push_back(key);
        }
        else {
            dict[key].first = value;
        }
    }

    ValueType& at(const KeyType& key) { return dict.at(key).first; }
    const ValueType& at(const KeyType& key) const { return dict.at(key).first; }

    ValueType& operator [] (const KeyType& key) {
        if (dict.find(key) == dict.end()) {
            dict[key] = {ValueType(), order.size()};
            order.push_back(key);
        }
        return dict[key].first;
    }

    const std::vector<KeyType> keys() {
        std::vector<KeyType> valid_keys;
        if (has_empty) {
            OrderedDict no_empty(*this);
            this->swap(no_empty);
            no_empty.clear();
        }
        for (const auto& k: order) valid_keys.push_back(k.value());
        return valid_keys;
    }
    const std::vector<KeyType> keys() const {
        std::vector<KeyType> valid_keys;
        if (!has_empty) {
            for (const auto& k: order) valid_keys.push_back(k.value());
        }
        if (has_empty) {
            for (const auto& k: order) {
                if (k.has_value()) valid_keys.push_back(k.value());
            }
        }
        return valid_keys;
    }

    const std::vector<ValueType> values() {
        std::vector<ValueType> valid_values;
        if (has_empty) {
            OrderedDict no_empty(*this);
            this->swap(no_empty);
            no_empty.clear();
        }
        for (const auto& k: order) valid_values.push_back(this->at(k.value()));
        return valid_values;
    }
    const std::vector<ValueType> values() const {
        std::vector<ValueType> valid_values;
        if (!has_empty) {
            for (const auto& k: order) valid_values.push_back(this->at(k.value()));
        }
        if (has_empty) {
            for (const auto& k: order) {
                if (k.has_value()) valid_values.push_back(this->at(k.value()));
            }
        }
        return valid_values;
    }

    const std::vector<std::pair<KeyType, ValueType>> items() {
        std::vector<std::pair<KeyType, ValueType>> valid_items;
        if (has_empty) {
            OrderedDict no_empty(*this);
            this->swap(no_empty);
            no_empty.clear();
        }
        for (const auto& k: order) {
            const auto& key = k.value();
            valid_items.push_back({key, this->at(key)});
        }
        return valid_items;
    }
    const std::vector<std::pair<KeyType, ValueType>> items() const {
        std::vector<std::pair<KeyType, ValueType>> valid_items;
        if (!has_empty) {
            for (const auto& k: order) {
                const auto& key = k.value();
                valid_items.push_back({key, this->at(key)});
            }
        }
        if (has_empty) {
            for (const auto& k: order) {
                if (k.has_value()) {
                    const auto& key = k.value();
                    valid_items.push_back({key, this->at(key)});
                }
            }
        }
        return valid_items;
    }


    std::pair<KeyType, ValueType> operator [] (size_t index) const {
        if (index >= order.size()) throw std::out_of_range("Index out of range");
        const KeyType& key = order[index].value();
        return {key, dict.at(key).first};
    }

    bool erase(const KeyType& key) {
        auto it = dict.find(key);
        if (it == dict.end()) return false;
        size_t index = it->second.second;
        dict.erase(it);
        order[index].reset();
        has_empty = true;
        return true;
    }


};






}

}


#endif