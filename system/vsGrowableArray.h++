// File vsGrowableArray.h++

#ifndef VS_GROWABLE_ARRAY_HPP
#define VS_GROWABLE_ARRAY_HPP

class vsGrowableArray
{
private:

    void        **storage;
    int         currentSize, stepSize, maxSize;
    void        *nowhere;
    
    int         access(int index);

public:

                vsGrowableArray(int initialSize, int sizeIncrement);
                ~vsGrowableArray();
    
    void        setSize(int newSize);
    int         getSize();
    
    void        setSizeIncrement(int sizeIncrement);
    int         getSizeIncrement();
    
    void        setMaxSize(int newMax);
    int         getMaxSize();
    
    void        setData(int index, void *data);
    void        *getData(int index);
    
    void        *&operator[](int index);
};

#endif
