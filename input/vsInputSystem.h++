#ifndef VS_INPUT_SYSTEM_HPP
#define VS_INPUT_SYSTEM_HPP

// Abstract base class for all classes that read input data from hardware

class vsInputSystem
{
public:

                    vsInputSystem();
    virtual         ~vsInputSystem();

    virtual void    update() = 0;
};

#endif
