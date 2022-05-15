/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <cassert>
#include <JuceHeader.h>
#include <type_traits>
#include <typeinfo>
#include <memory>

//==============================================================================

// Max sandbox type area (todo: setup unit test environment)
template <typename T>
class ParamType;


// (concept)
class Parameter
{
public:
    virtual ~Parameter() = default;

    // virtual interface
    virtual juce::var GetVar() const = 0;
    virtual const std::type_info& GetType() const = 0;
    virtual const juce::Identifier& GetName() const = 0;

    // template interface for runtime type un-erasure
    template <typename T>
    const T& Get() const;
};


// (model)
template <typename T>
class ParamType : public Parameter
{
    // (data_ -> var)
    // make sure juce::Var(T arg) will compile or we have a user-defined 'T::operator juce::var() const' availible
    static_assert(std::is_constructible<T, juce::var>::value
                || std::is_convertible<T, juce::var>::value);

    // (var->data_)
    // if juce::var(T arg) complies, we can find the correct juce::var::getAs
    static_assert(std::is_constructible<T, juce::var>::value
                || std::is_constructible<juce::var, T>::value); // user-provided constructor

public:
    // ctor
    ParamType(const juce::Identifier& Name)
    : name_(Name)
    {}
    
    ParamType(const juce::Identifier& Name, const T& InitialValue)
    : data_(InitialValue)
    , name_(Name)
    {}

    // dtor
    virtual ~ParamType() override = default;
    
    // template interface
    T& operator*() { return data_; }
    const T& operator*() const { return data_; }

    // base interface
    virtual juce::var GetVar() const override { return juce::var(data_); }
    virtual const std::type_info& GetType() const override { return typeid(data_); }
    virtual const juce::Identifier& GetName() const override { return name_; }
    
    
private:
    T data_;
    juce::Identifier name_;
    
};


// todo: make a safe std::optional version 
template <typename T>
const T& Parameter::Get() const
{
    auto* derivedPtr = dynamic_cast<const ParamType<T>*>(this);
    jassert(derivedPtr); // type T does not match underlying PraamType :(

    if(derivedPtr)
    {
        return **derivedPtr;
    }

    static T Failure;
    return Failure;
}



class ParameterList : public std::vector<std::unique_ptr<Parameter>>
{
public:
    // ...from r-value other
    template <typename T>
    ParameterList& add(const juce::Identifier Name)
    {
        push_back(std::make_unique<ParamType<T>>(Name));
        return *this;
    }
    
    template <typename T>
    ParameterList& add(const juce::Identifier Name, T&& InitialValue)
    {
        push_back(std::make_unique<ParamType<T>>(Name, InitialValue));
        return *this;
    }

    juce::String DebugString(int AraryPrintDepth = 0) const
    {
        juce::String dbg;
        
        for(const auto& param : *this)
        {
            auto name = param->GetName();
            juce::String type = param->GetType().name();
            juce::String value = param->GetVar().toString();

            if(param->GetVar().isArray() || AraryPrintDepth > 0)
            {
                // todo
            }

            dbg << '(' << type << ") " << name << " = " << value << '\n';
        }

        return dbg;
    }
    
private:
    
};

struct Position3D
{
    double x, y, z;

    operator juce::var() const
    {
        return juce::Array<juce::var>(x, y, z);
    }
};

struct Position2D
{
    double x = 0.0;
    double y = 0.0;

    operator juce::var() const
    {
        return juce::Array<juce::var>(x, y);
    }
};


int main (int, char*)
{
    std::cout << std::boolalpha;
    
    // example data init
    std::vector<float> myBuffer;
    for(int i = 0; i < 25; ++i)
    {
        juce::String MyFloatString(5.f);
        myBuffer.emplace_back(static_cast<float>(i) + 0.1f);
    }

    // parameter list declaration
    ParameterList paramList;
    
    paramList
        // .add<std::vector<float>>("buffer", { 1.0, 2.0, 3.0 })
        // .add<std::vector<float>>("buffer", { 1.0 })
        // .add<std::vector<float>>("buffer")
        // .add<std::vector<float>>("buffer", { 1.0, 2.0 })
        .add<Position3D>("Source Position3D", { 0.0, 25.0, 10.0 })
        .add<Position2D>("Source Position2D", { 0.0, 25.0 })
        .add<bool>("Enabled?", false)
        .add<int>("Num Taps", 4)
        .add<float>("HPF frequency", 500.f);


    DBG(paramList.DebugString());

    
    // parameter manipulation
    ParamType<float> f("Freq.", 190.f);
    Parameter* p = &f;

    float x = p->Get<float>(); // should compile
    double y = p->Get<float>(); // should compile & cast w/ warning
    int z = p->Get<int>(); // should hit runtime assert
    
    

    return 0;
}
