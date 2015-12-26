#pragma once

template <class T>
class Handler
{
public:
    // explicit construction.
    explicit Handler(T* pointer = NULL)
    {
        if (pointer)
            Register(pointer);
        else
            _index = size_t(-1);
    }
    // assign operation for new hander.
    void operator =(Handler* other)
    {
        _index = other->_index;
    }
    // get real class pointer
    T* Get() const
    {
        if (_index == size_t(-1))
            return NULL;

        return _container[_index];
    }
    // delete global class pointer in container.  
    void Delete(T* pointer, bool deletePointer = false);
private:
    void Register(T* pointer);
    size_t GetFreeIndex();
private:
    size_t _index;
    static std::vector<T*> _container;
};

// declare a class container.
template <class T> 
std::vector<T*> Handler<T>::_container;


template <class T>
void Handler<T>::Register(T* pointer)
{
    size_t count = _container.size(); 
    for (size_t i = 0; i != count; ++i)
    {
        if (_container[i] == pointer)
        {
            _index = i;
            return;
        }
    }

    size_t newIndex = GetFreeIndex();
    if (newIndex != size_t(-1))
    {
        _index = newIndex;
        _container[_index] = pointer;
        return;
    }

    _index = _container.size();
    _container.push_back(pointer);
}

template <class T>
size_t Handler<T>::GetFreeIndex()
{
    size_t count = _container.size(); 
    for (size_t i = 0; i != count; ++i)
    {
        if (_container[i] == NULL)
        {
            return i;
        }
    }
    return size_t(-1);
}

template <class T>
void Handler<T>::Delete(T* pointer, bool deletePointer/* = false*/)
{
    size_t count = _container.size(); 
    for (size_t i = 0; i != count; ++i)
    {
        if (_container[i] == pointer)
        {
            _container[i] = NULL;

            if (i == _index)
                _index = size_t(-1);

            break;
        }
    }

    if (deletePointer)
    {
        delete pointer;
    }
}

