#include "parametersset.h"


/*
template <class T>
ParametersSet<T>::ParametersSet() {

}

template <class T>
ParametersSet<T>::ParametersSet(ParametersSet<T>& copy) {
    parameters = copy;
}

template <class T>
ParametersSet<T>::ParametersSet(QMap<QString, T>&& data) {
    for(auto& val: data){
        parameters.insert(val.first(), val.second);
    }
}

template <class T>
ParametersSet<T>::~ParametersSet() {

}


template <class T>
T ParametersSet<T>::get(QString key) const {
    auto it = parameters.find(key);
    return (it == parameters.end() ? nullptr : it->value);
}

template <class T>
T ParametersSet<T>::get(QString key, T default_value) const {
    auto it = parameters.find(key);
    return (it == parameters.end() ? default_value : it->value);
}

template <class T>
void ParametersSet<T>::set(QString&& key, T& value) { parameters.insert(key, value); }

template <class T>
void ParametersSet<T>::set(QMap<QString, T>&& data) {
    for(auto& val: data)
        parameters.insert(val.first(), val.second);
}

template <class T>
inline void ParametersSet<T>::clear() {
    parameters.clear();
}

template <class T>
T& ParametersSet<T>::operator[](QString&& key) {
    return parameters[key];
}
template <class T>
const T& ParametersSet<T>::operator[](QString&& key) const {
    return static_cast<const T>(parameters[key]);
}
*/
