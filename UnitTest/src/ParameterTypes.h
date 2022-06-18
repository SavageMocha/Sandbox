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
      std::string str = Name.toString().toStdString();
      return add(str, std::forward<T>(DefaultValue));
    }

    // add w/ std::string
    template <typename T>
    ParameterList& add(std::string& Name, T&& DefaultValue = {})
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
      return addAndGetRef(Name.toString().toStdString(), std::forward<T>(DefaultValue));
    }

    // add + get ref w/ std::string
    template <typename T>
    T& addAndGetRef(std::string& Name, T&& DefaultValue = {})
    {
      std::unique_ptr<UiParameter> NewEntry = std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue));
      
      T& ref = NewEntry->GetRef<T>();
      ParamMap[Name] = std::move(NewEntry);

      return ref;
    }

    // index operator for juce::Identifier
    std::unique_ptr<UiParameter>& operator[](const juce::Identifier Name)
    {
      return ParamMap[Name.toString().toStdString()];
    }

    // index operator for std::string
    std::unique_ptr<UiParameter>& operator[](const std::string& Name)
    {
      return ParamMap[Name];
    }

    // index operator for const char[] of any length
    template <unsigned int N>
    std::unique_ptr<UiParameter>& operator[](const char (&Name)[N])
    {
      return ParamMap[std::string(Name)];
    }
    
    // juce::ValueTree sync
    juce::ValueTree BootstrapValueTree() const
    {
      static juce::Identifier ParamList("Parameter_List");
      juce::ValueTree listTree(ParamList);
      
      for(const auto& entry : ParamMap)
      {
        listTree.setProperty({entry.first}, juce::var(entry.second->GetAsVar()), nullptr);
      }

      return listTree;
    }

    void SyncToTree(juce::ValueTree& inTree)
    {
      // take on the current state of inTree
      const int numProperties = inTree.getNumProperties();
      for (int i = 0; i < numProperties; ++i)
      {
        juce::Identifier name (inTree.getPropertyName(i));
        ParamMap[name.toString().toStdString()]->SetAsVar(inTree.getProperty(name));
      }
      
      inTree.addListener(this);
    }

    void DesyncFromTree(juce::ValueTree& inTree)
    {
      inTree.removeListener(this);
    }

    private:

    // value tree listener callback
    virtual void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
      ParamMap[property.toString().toStdString()]->SetAsVar(treeWhosePropertyHasChanged.getProperty(property));
    }

    // underlying "list" (map)
    std::unordered_map<std::string, std::unique_ptr<UiParameter>> ParamMap;

  }; // class ParameterList
} // namespace Haze
























