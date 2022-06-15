#ifndef PARAMETERSSET_H
#define PARAMETERSSET_H

#include <unordered_map>
#include <map>
#include <string>

using std::unordered_map;
using std::string;


template <class T>
class ParametersSet : public unordered_map<string, T> {
public:
    T get(string key, T default_value = T()) const;
    void set(string&& key, T value);
    T remove(string&& key);
};

template <class T>
T ParametersSet<T>::get(string key, T default_value) const {
    auto it = this->find(key);
    return (it == this->end() ? default_value : it->second);
}

template <class T>
void ParametersSet<T>::set(string&& key, T value) { this->insert(std::make_pair(key, value)); }

template <class T>
T ParametersSet<T>::remove(string&& key) {
    auto it = this->find(key);
    if(it != this->end()) {
        T value = (*it).second;
        this->erase(it);
        return value;
    }

    return T();
}

#endif // PARAMETERSSET_H
