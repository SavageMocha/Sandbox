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

    // implicit cast to underlying type
    operator T&() { return data_; }
    operator T() { return data_; }
    
    // template interface
    T& operator*() { return data_; }
    const T& operator*() const { return data_; }

    // base interface
    virtual const juce::Identifier& GetName() const override { return name_; }
    virtual const std::type_info& GetType() const override { return typeid(data_); }
    
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


class ParameterList
{
public:
    template <typename T>
    ParameterList& add(const juce::Identifier Name, const T& InitialValue = {})
    {
        auto [ it, res ] = existingNames_.insert(Name);
        if(res)
        {
            list_.push_back(std::make_unique<ParamType<T>>(Name, InitialValue));
        }
        else
        {
            jassert(false); // inserting a parameter w/ an identical name to one that already exists in the list
            // ignoring insert
        }
        
        return *this;
    }

    template <typename T>
    ParameterList& add(const juce::Identifier Name, T&& InitialValue)
    {
        list_.push_back(std::make_unique<ParamType<T>>(Name, InitialValue));
        return *this;
    }

    // iterator forwarding
    std::vector<std::unique_ptr<Parameter>>::iterator begin() { return list_.begin(); }
    std::vector<std::unique_ptr<Parameter>>::iterator end() { return list_.end(); }
    
private:
    std::vector<std::unique_ptr<Parameter>> list_;
    std::set<juce::Identifier> existingNames_;
};

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
    ParameterList paramList;
    
    paramList
        .add<float>("HPF frequency")
        .add<float>("Num Taps", 4)
        .add<bool>("Enabled?", false)
        .add<Position3D>("Source Position3D", { 0.f, 25.f, 10.f })
        .add<Position2D>("Source Position2D", { 0.f, 25.f })
        .add<std::vector<float>>("buffer", myBuffer)
        .add<std::vector<float>>("buffer", { 1.1f, 2.1f, 3.1f, 4.1f })
        .add<std::vector<float>>("buffer", { 1.f, 2.f, 3.f })
        .add<std::vector<float>>("buffer", { 1.f, 2.f })
        .add<std::vector<float>>("buffer", { 1.f });


    // parameter list debug view
    for(const auto& param : paramList)
    {
        auto name = param->GetName();
        juce::String type = param->GetType().name();
        juce::String value = *param;

        DBG(name + " = " + value + " -- type: (" + type + ")");
    }

    
    // parameter manipulation
    ParamType<float> float1("My Float", 1.f);
    ParamType<float> float2("My Float", 2.f);

    ParamType<float> float3("My Float");
    *float3 = *float1 + *float2;
    jassert(*float3 == 3.f);

    return 0;
}
