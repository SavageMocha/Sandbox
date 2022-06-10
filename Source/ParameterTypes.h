/*
  ==============================================================================

    ParameterTypes.h
    Created: 15 May 2022 2:28:58pm
    Author:  maxmo

  ==============================================================================
*/

#pragma once

#include <type_traits>
#include <memory>

namespace Haze
{
// forwards
  template <typename T>
  class ParamType;
  class Parameter;


  // (concept)
  class Parameter
  {
  public:
    virtual ~Parameter() = default;

    // common interface
    [[nodiscard]] virtual const type_info& Type() const = 0;

    // assignmentt
    template <typename T>
    Parameter& operator=(const T& inValue)
    {
      if(auto* downPtr = DowncastChecked<T>())
      {
        *downPtr = inValue;
      }
      
      return *this;
    }

    // comparison
    template <typename T>
    bool IsEqualTo(const T& other) const
    {
      if(auto* downPtr = DowncastChecked<T>())
      {
        return **downPtr == other;
      }

      return false;
    }

    // Get underlying data
    template <typename T>
    const T& Get()
    {
      return **DowncastChecked<T>();
    }

  private:
    
    // downcast helpers (runtime assert on failure)
    template <typename T>
    ParamType<T>* DowncastChecked()
    {
      ParamType<T>* downPtr = dynamic_cast<ParamType<T>*>(this);
      jassert(downPtr); // dynamic_cast failed! T != underlying tye
      return downPtr;
    }

    template <typename T>
    const ParamType<T>* DowncastChecked() const
    {
      const ParamType<T>* downPtr = dynamic_cast<const ParamType<T>*>(this);
      jassert(downPtr); // dynamic_cast failed! T != underlying type
      return downPtr;
    }
    

  }; // Parameter
    

  
  template <typename T>
  class ParamType : public Parameter
  {
  public:
    ParamType() = delete;
    ParamType(T&& value) : Parameter(), data_(value) {}
    
    [[nodiscard]] const type_info& Type() const override final { return typeid(T); }


    ParamType& operator=(const T& inValue)
    {
      data_ = inValue;
      return *this;
    }

    const T& operator*() const
    {
      return data_;
    }

  private:
    T data_;
    
  }; // ParamType<T>

  

  using ParamListEntryType = std::unordered_map<std::string, std::unique_ptr<Parameter>>;

  // todo: instead of elements being the raw unique_ptr,
  // wrap that in a struct that hides the ptr-deref syntax, and has ui meta-data
  // a similar struct can be handed to a ParameterList::add() overload
  class ParameterList : public ParamListEntryType
  {
  public:

    // builder  
    template <typename T>
    ParameterList& add(juce::Identifier Name, T&& DefaultValue = {})
    {
      // todo: disallow duplicate entries (name collision)
      this->ParamListEntryType::operator[](Name.toString().toStdString())
        = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      return *this;
    }

    template <typename T>
    ParameterList& add(std::string& Name, T&& DefaultValue = {})
    {
      this->ParamListEntryType::operator[](Name)
        = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      return *this;
    }

    std::unique_ptr<Parameter>& operator[](const juce::Identifier Name)
    {
      return this->ParamListEntryType::operator[](Name.toString().toStdString());
    }
    
    
  }; //ParameterList
} // namespace Haze
























