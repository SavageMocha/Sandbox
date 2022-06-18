/*
  ==============================================================================

    ParameterTypes.h
    Created: 15 May 2022 2:28:58pm
    Author:  maxmo

  ==============================================================================
*/

#include <JuceHeader.h>
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
    [[nodiscard]] virtual const std::type_info& Type() const = 0;

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
    

  }; // class Parameter
    

  
  template <typename T>
  class ParamType : public UiParameter
  {
  public:
    ParamType() = delete;
    ParamType(T&& value) : UiParameter(), data_(value) {}
    
    [[nodiscard]] const std::type_info& Type() const override final { return typeid(T); }

    [[nodiscard]] virtual juce::var GetAsVar() const override
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
    
    virtual void SetAsVar(const juce::var& inVar) override
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



  

  // todo: provide ui metadata (range, tooltip, etc.)
  class ParameterList : public juce::ValueTree::Listener
  {
  public:
    // builder methods
    // add w/ juce::Identifier
    template <typename T>
    ParameterList& add(juce::Identifier Name, T&& DefaultValue = {})
    {
      juce::String str = Name.toString();
      return add(str, std::forward<T>(DefaultValue));
    }

    // add w/ juce::String
    template <typename T>
    ParameterList& add(juce::String& Name, T&& DefaultValue = {})
    {
      // name collision (previous entry will be stomped!)
      jassert(ParamMap.find(Name) == ParamMap.end());
      
      ParamMap[Name] = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      return *this;
    }

    // add + get ref w/ juce::identifier
    template <typename T>
    T& addAndGetRef(juce::Identifier Name, T&& DefaultValue = {})
    {
      return addAndGetRef(Name.toString(), std::forward<T>(DefaultValue));
    }

    // add + get ref w/ juce::String
    template <typename T>
    T& addAndGetRef(juce::String& Name, T&& DefaultValue = {})
    {
      std::unique_ptr<UiParameter> NewEntry = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      
      T& ref = NewEntry->GetRef<T>();
      ParamMap[Name] = std::move(NewEntry);

      return ref;
    }

    // index operator for juce::Identifier
    std::unique_ptr<UiParameter>& operator[](const juce::Identifier Name);

    // index operator for juce::String
    std::unique_ptr<UiParameter>& operator[](const juce::String& Name);

    // index operator for const char[] of any length
    template <unsigned int N>
    std::unique_ptr<UiParameter>& operator[](const char (&Name)[N])
    {
      return ParamMap[juce::String(Name)];
    }
    
    // juce::ValueTree sync
    juce::ValueTree BootstrapValueTree() const;

    void SyncToTree(juce::ValueTree& inTree);

    void DesyncFromTree(juce::ValueTree& inTree);

    private:

    // value tree listener callback
    virtual void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    // underlying "list" (map)
    std::unordered_map<juce::String, std::unique_ptr<UiParameter>> ParamMap;

  }; // class ParameterList
} // namespace Haze
























