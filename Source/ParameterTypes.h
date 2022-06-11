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
  class UiParameter;


  // (concept)
  class UiParameter
  {
  public:
    virtual ~UiParameter() = default;

    // common interface
    [[nodiscard]] virtual const type_info& Type() const = 0;

    // to/from juce::var (serialization)
    [[nodiscard]] virtual juce::var GetAsVar() const = 0;
    virtual void SetAsVar(const juce::var& inVar) = 0;

    // assignment
    template <typename T>
    UiParameter& operator=(const T& inValue)
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

    template <typename T>
    T& GetRef()
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
  class ParamType : public UiParameter
  {
  public:
    ParamType() = delete;
    ParamType(T&& value) : UiParameter(), data_(value) {}
    
    [[nodiscard]] const type_info& Type() const override final { return typeid(T); }

    [[nodiscard]] virtual juce::var GetAsVar() const
    {
      static_assert(
        std::is_constructible<juce::var, T>::value || std::is_convertible<T, juce::var>::value);
      
      if constexpr (std::is_constructible<juce::var, T>::value)
      {
        return juce::var(data_);
      }
      else if constexpr (std::is_convertible<T, juce::var>::value)
      {
        return static_cast<juce::var>(data_);
      }

      return {};
    }
    
    virtual void SetAsVar(const juce::var& inVar)
    {
      data_ = inVar;
    } 


    ParamType& operator=(const T& inValue)
    {
      data_ = inValue;
      return *this;
    }

    const T& operator*() const
    {
      return data_;
    }

    T& operator*()
    {
      return data_;
    }

  private:
    T data_;
    
  }; // ParamType<T>
  

  using ParamListEntryType = std::unordered_map<std::string, std::unique_ptr<UiParameter>>;
  
  class ParameterList : public ParamListEntryType
  {
  public:
    // builder methods
    // add w/ juce::Identifier
    template <typename T>
    ParameterList& add(juce::Identifier Name, T&& DefaultValue = {})
    {
      std::string str = Name.toString().toStdString();
      return add(str, std::forward<T>(DefaultValue));
    }

    // add w/ std::string
    template <typename T>
    ParameterList& add(std::string& Name, T&& DefaultValue = {})
    {
      // name collision (previous entry will be stomped!)
      jassert(this->find(Name) == this->end());
      
      this->ParamListEntryType::operator[](Name)
        = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      return *this;
    }

    // add + get ref w/ juce::identifier
    template <typename T>
    T& addAndGetRef(juce::Identifier Name, T&& DefaultValue = {})
    {
      return addAndGetRef(Name.toString().toStdString(), std::forward<T>(DefaultValue));
    }

    // add + get ref w/ std::string
    template <typename T>
    T& addAndGetRef(std::string& Name, T&& DefaultValue = {})
    {
      std::unique_ptr<UiParameter> NewEntry = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      
      T& ref = NewEntry->GetRef<T>;
      this->ParamListEntryType::operator[](Name) = std::move(NewEntry);

      return ref;
    }

    // index operator oveload for juce::Identifier
    std::unique_ptr<UiParameter>& operator[](const juce::Identifier Name)
    {
      return this->ParamListEntryType::operator[](Name.toString().toStdString());
    }

    
    juce::ValueTree BootstrapValueTree() const
    {
      static juce::Identifier ParamList("Parameter_List");
      juce::ValueTree listTree(ParamList);
      
      for(const auto& entry : *this)
      {
        // todo: need access to a "to juce::var" function
        listTree.setProperty({entry.first}, juce::var(entry.second->GetAsVar()), nullptr);
      }

      return listTree;
    }

    void SyncToTree(juce::ValueTree inTree)
    {
      // todo
    }
    
  }; //ParameterList
} // namespace Haze
























