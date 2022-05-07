/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <any>

//==============================================================================

// Max sandbox type area (todo: setup unit test environment)

// (concept)
class Parameter
{
public:
    virtual ~Parameter() = default;

    // virtual interface
    virtual const juce::Identifier& GetName() const = 0;
    virtual const std::type_info& GetType() const = 0;
    virtual std::any GetValue() const = 0;
    virtual operator juce::String() const = 0; // for debug & UI purposes
};


// (model)
template <typename T>
class ParamType : public Parameter
{
public:
    // ctor
    ParamType(const juce::Identifier& Name, const T& InitialValue = {})
    : data_(InitialValue)
    , name_(Name)
    {}

    ParamType(const juce::Identifier& Name, T&& InitialValue)
    : data_(InitialValue)
    , name_(Name)
    {}

    // dtor
    virtual ~ParamType() override = default;
    
    // template interface
    T get() const { return data_; }

    void set(const T& inValue) { data_ = inValue; }

    // base interface
    virtual const juce::Identifier& GetName() const override { return name_; }
    virtual const std::type_info& GetType() const override { return typeid(data_); }
    virtual std::any GetValue() const override { return data_; }
    virtual operator juce::String() const override
    {
        if constexpr(std::is_same<T, bool>::value)
        {
            return juce::String(data_? "true" : "false");
        }
        else if constexpr(std::is_same<T, std::vector<float>>::value) // todo: generalize string-ifying containers & arrays(?)
        {
            constexpr int previewLen = 3;
            const int p = juce::jmin<int>(previewLen, static_cast<int>(data_.size()));

            juce::String res;

            res << "["<< data_.size() << "]";

            if(p > 0)
            {
                res << "{ " << data_[0];
                for(int i = 1; i < p; ++i)
                {
                     res << ", " << data_[i];
                }

                if(p < static_cast<int>(data_.size()))
                {
                    res << ", ...";
                }

                res << " }";
            }

            return res;
        }
        else
        {
            static_assert( std::is_convertible_v<T, juce::String> || std::is_constructible_v<juce::String, T>
                , "underlying type must be convertable to juce::String, either add a constexpr case to this switch statement, or support implicit conversion" );
            return juce::String(data_);
        }
    }
    
private:
    T data_;
    juce::Identifier name_;
    
};

namespace ParamHelpers
{
    template <typename T>
    std::unique_ptr<Parameter> Create(const juce::Identifier Name, const T& InitialValue = {})
    {
        return std::unique_ptr<Parameter>(static_cast<Parameter*>(new ParamType<T>(Name, InitialValue)));
    }

    template <typename T>
    std::unique_ptr<Parameter> Create(const juce::Identifier Name, T&& InitialValue)
    {
        return std::make_unique<ParamType<T>>(Name, InitialValue);
    }
}; // namespace ParamHelpers

struct Position3D
{
    float x, y, z;
    operator juce::String() const
    {
        juce::String res;
        res << "[ X: "  << x << ", Y: " << y << ", Z: " << z << " ]";

        return res;
    }
};

struct Position2D
{
    float x, y;
    operator juce::String() const
    {
        juce::String res;
        res << "[ X: "  << x << ", Y: " << y << " ]";

        return res;
    }
};



int main (int, char*)
{
    // example data init
    std::vector<float> myBuffer;
    for(int i = 0; i < 25; ++i)
    {
        myBuffer.emplace_back(static_cast<float>(i) + 0.1f);
    }

    // parameter list declaration
    std::vector<std::unique_ptr<Parameter>> paramList;
    
    paramList.push_back(ParamHelpers::Create<float>("HPF frequency"));
    paramList.push_back(ParamHelpers::Create<float>("Num Taps", 4));
    paramList.push_back(ParamHelpers::Create<bool>("Enabled?", false));
    paramList.push_back(ParamHelpers::Create<Position3D>("Source Position3D", {0.f, 25.f, 10.f}));
    paramList.push_back(ParamHelpers::Create<Position2D>("Source Position2D", {0.f, 25.f}));
    paramList.push_back(ParamHelpers::Create<std::vector<float>>("buffer", myBuffer));
    paramList.push_back(ParamHelpers::Create<std::vector<float>>("buffer", { 1.1f, 2.1f, 3.1f, 4.1f }));
    paramList.push_back(ParamHelpers::Create<std::vector<float>>("buffer", { 1.f, 2.f, 3.f }));
    paramList.push_back(ParamHelpers::Create<std::vector<float>>("buffer", { 1.f, 2.f }));
    paramList.push_back(ParamHelpers::Create<std::vector<float>>("buffer", { 1.f }));


    // parameter list debug view
    for(const auto& param : paramList)
    {
        auto name = param->GetName();
        juce::String type = param->GetType().name();
        juce::String value = *param;

        DBG(name + " = " + value + " -- type: (" + type + ")");
    }
    

    return 0;
}
