/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <memory>

//==============================================================================

// Max sandbox type area (todo: setup unit test environment)

class Parameter
{
public:
    virtual ~Parameter() = default;

    // virtual interface
    virtual const juce::Identifier& GetType() const = 0;
    
};


template <typename T>
class ParamType : public Parameter
{
public:
    // ctor
    ParamType(const juce::Identifier& TypeId, const T& InitialValue = {})
    : data_(InitialValue)
    , type_(TypeId)
    {}

    ParamType(const juce::Identifier& TypeId, T&& InitialValue)
    : data_(InitialValue)
    , type_(TypeId)
    {}

    // dtor
    virtual ~ParamType() override = default;
    
    // template interface
    T get() const { return data_; }

    void set(const T& inValue) { data_ = inValue; }

    // base interface
    virtual const juce::Identifier& GetType() const override { return type_; }
    
private:
    
    T data_;
    juce::Identifier type_;
    
};


template <typename T>
std::unique_ptr<Parameter> Create(const juce::Identifier TypeId, const T& InitialValue = {})
{
    return std::unique_ptr<Parameter>(static_cast<Parameter*>(new ParamType<T>(TypeId, InitialValue)));
}

template <typename T>
std::unique_ptr<Parameter> Create(const juce::Identifier TypeId, T&& InitialValue)
{
    return std::make_unique<ParamType<T>>(TypeId, InitialValue);
}



int main (int argc, char* argv[])
{
    std::vector<float> myBuffer;
    for(int i = 0; i < 100; ++i)
    {
        myBuffer.emplace_back(static_cast<float>(i));
    }

    std::vector<std::unique_ptr<Parameter>> paramList;

    paramList.push_back(Create<float>("pitch"));
    paramList.push_back(Create<float>("lpf freq", 1400.f));
    paramList.push_back(Create<float>("hpf pitch", 50.f));
    paramList.push_back(Create<bool>("is enabled", false));
    paramList.push_back(Create<std::vector<float>>("buffer", myBuffer));

    
    for(const auto& param : paramList)
    {
        DBG(param->GetType());
    }

    return 0;
}
