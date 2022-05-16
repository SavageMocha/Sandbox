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

  // (concept)
  class Parameter
  {
  public:
    // dtor
    virtual ~Parameter() = default;
    
    // virtual interface
    [[nodiscard]] virtual juce::Identifier GetName() const = 0;
    [[nodiscard]] virtual const type_info& GetType() const = 0;
  

  private:
    
  }; // Parameter

  template <typename T>
  class ParamType : public Parameter
  {
  public:
    ParamType() = delete;
    
    ParamType(juce::Identifier inName, T inDefaultValue)
    : name_(std::move(inName))
    , data_(std::move(inDefaultValue))
    {}
    
    // Parameter interface
    [[nodiscard]] virtual juce::Identifier GetName() const override final { return name_; }
    [[nodiscard]] virtual const type_info& GetType() const override final { return typeid(T); }
    
  private:
    juce::Identifier name_;
    T data_;
    
  }; // ParamType<T>


  class ParameterList : public std::vector<std::unique_ptr<Parameter>>
  {
  public:
    template <typename T>
    ParameterList& add(juce::Identifier Name, T&& DefaultValue = {})
    {
      emplace_back(std::make_unique<ParamType<T>>(Name, std::forward<T>(DefaultValue)));
      return *this;
    }

    
  }; //ParameterList
  
} // namespace Haze
























