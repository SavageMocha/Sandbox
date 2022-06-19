/*
  ==============================================================================

    ParameterTypes.h
    Created: 15 May 2022 2:28:58pm
    Author:  maxmo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <type_traits>
#include <algorithm>
#include <optional>
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

    template<typename T>
    void SetInPlaceClamper(std::function<void(T&)>&& lambda)
    {
        ParamType<T>* **downPtr = DowncastChecked<T>();
        jassert(downPtr);

        downPtr->InPlaceClamper = lambda;
    }

    template<typename T>
    std::function<void(T&)>& GetInPlaceClamper()
    {
        ParamType<T>* **downPtr = DowncastChecked<T>();
        jassert(downPtr);

        return downPtr->InPlaceClamper;
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

    std::function<void(T&)> InPlaceClamper = [](T& t){juce::ignoreUnused(t);};

  private:
    T data_;    
  }; // ParamType<T>



  struct UiMetadata
  {
    juce::String ToolTip_;
    juce::String Units_;
    bool bPreferSliderOverKnob_;
    bool bIsLogarithmic_;

    // ctor
    UiMetadata(juce::String&& ToolTip = "N/A", juce::String&& Units = {}, bool bPreferSliderOverKnob = false, bool bIsLogarithmic = false)
    : ToolTip_(ToolTip)
    , Units_(Units)
    , bPreferSliderOverKnob_(bPreferSliderOverKnob)
    , bIsLogarithmic_(bIsLogarithmic)
    {}
  };
  

  class ParameterList : public juce::ValueTree::Listener
  {
      struct ParameterEntry
      {
        juce::Identifier id;
        std::unique_ptr<UiParameter> paramPtr;

        bool operator==(const juce::Identifier& Name) { return Name == id; } // std::find_if

        // ctor
        ParameterEntry(const juce::Identifier& inId, std::unique_ptr<UiParameter>&& inParamPtr)
        : id(inId)
        , paramPtr(std::forward<std::unique_ptr<UiParameter>>(inParamPtr))
        {}
      };

      struct UiMetadataEntry
      {
        juce::Identifier id;
        UiMetadata Metadata;

        bool operator==(const juce::Identifier& Name) { return Name == id; } // std::find_if

        // ctor
        UiMetadataEntry(const juce::Identifier& inId, UiMetadata&& inMetadata)
        : id(inId)
        , Metadata(inMetadata)
        {}
      };


  public:
    // builder method
    template <typename T>
    ParameterList& add(const juce::Identifier& Name, T&& DefaultValue = {}, UiMetadata&& MetaData = {})
    {
      // check for name collision (previous entry will be stomped!)
      jassert(parameters_.end() == std::find(parameters_.begin(), parameters_.end(), Name));
      jassert(uiMetadata_.end() == std::find(uiMetadata_.begin(), uiMetadata_.end(), Name));

      parameters_.emplace_back(ParameterEntry(Name, std::make_unique<ParamType<T>>(std::forward<T>(DefaultValue))));
      uiMetadata_.emplace_back(UiMetadataEntry(Name, std::forward<UiMetadata>(MetaData)));

      return *this;
    }

    // index operator for juce::Identifier
    std::unique_ptr<UiParameter>& operator[](const juce::Identifier Name);
    
    // juce::ValueTree sync
    juce::ValueTree GetStateAsTree() const;
    void SyncToTree(juce::ValueTree& inTree);
    void DesyncFromTree(juce::ValueTree& inTree);

  private:
    // value tree listener callback
    virtual void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;

    // helper functions for finding an element by name
    template <typename T>
    T* FindEntryByName(juce::Identifier Name, std::vector<T>& vec)
    {
      if(auto it = std::find(vec.begin(), vec.end(), Name); it != vec.end())
      {
        return &(*it);
      }
      
      // we should never be asking about an entry that doesn't exist!
      jassert(false);
      return nullptr;
    }

    // underlying "lists"
    std::vector<ParameterEntry> parameters_;
    std::vector<UiMetadataEntry> uiMetadata_;

  }; // class ParameterList
  
} // namespace Haze
























