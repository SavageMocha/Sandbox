/*
  ==============================================================================

    ParameterTypes.h
    Created: 15 May 2022 2:28:58pm
    Author:  maxmo

  ==============================================================================
*/

#include "ParameterTypes.h"

namespace Haze
{

    // index operator for juce::Identifier
    std::unique_ptr<UiParameter>& ParameterList::operator[](const juce::Identifier Name)
    {
      return ParamMap[Name.toString().toStdString()];
    }

    // index operator for juce::String
    std::unique_ptr<UiParameter>& ParameterList::operator[](const juce::String& Name)
    {
      return ParamMap[Name];
    }
    
    // juce::ValueTree sync
    juce::ValueTree ParameterList::BootstrapValueTree() const
    {
      static juce::Identifier ParamList("Parameter_List");
      juce::ValueTree listTree(ParamList);
      
      for(const auto& entry : ParamMap)
      {
        listTree.setProperty({entry.first}, juce::var(entry.second->GetAsVar()), nullptr);
      }

      return listTree;
    }

    void ParameterList::SyncToTree(juce::ValueTree& inTree)
    {
      // take on the current state of inTree
      const int numProperties = inTree.getNumProperties();
      for (int i = 0; i < numProperties; ++i)
      {
        juce::Identifier name (inTree.getPropertyName(i));
        ParamMap[name.toString()]->SetAsVar(inTree.getProperty(name));
      }
      
      inTree.addListener(this);
    }

    void ParameterList::DesyncFromTree(juce::ValueTree& inTree)
    {
      inTree.removeListener(this);
    }

    // value tree listener callback
    void ParameterList::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
    {
      ParamMap[property.toString()]->SetAsVar(treeWhosePropertyHasChanged.getProperty(property));
    }
} // namespace Haze























