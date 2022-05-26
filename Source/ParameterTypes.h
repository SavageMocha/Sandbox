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
    bool operator==(const T& other) const
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
    
    // downcast helper (runtime assert on failure)
    template <typename T>
    ParamType<T>* DowncastChecked() const
    {
      ParamType<T>* downPtr = dynamic_cast<ParamType<T>*>(this);
      jassert(downPtr); // dynamic_cast failed! T != underlying 
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

  


  
  class ParameterList : public std::unordered_map<std::string, std::unique_ptr<Parameter>>
  {
  public:

    // builder  
    template <typename T>
    ParameterList& add(juce::Identifier Name, T&& DefaultValue = {})
    {
      this->emplace(std::make_unique<ParamType<T>>(Name, std::forward<T>(DefaultValue)));
      return *this;
    }

    // templatize index operator
    template <typename T>
    const T& Get(const juce::Identifier& inName) const
    {
      std::string key = inName.toString().toStdString();
      return static_cast<const T&>(std::unordered_map<std::string, std::unique_ptr<Parameter>>::operator[](key));
    }

    template <typename T>
    T& Get(const juce::Identifier& inName)
    {
      std::string key = inName.toString().toStdString();
      return static_cast<T&>(std::unordered_map<std::string, std::unique_ptr<Parameter>>::operator[](key));
    }
    
  }; //ParameterList
} // namespace Haze
























