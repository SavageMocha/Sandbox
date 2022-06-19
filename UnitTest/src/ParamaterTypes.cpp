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
// ParameterList impl:
    // index operator for juce::Identifier
    std::unique_ptr<UiParameter>& ParameterList::operator[](const juce::Identifier Name)
    {
      return FindEntryByName(Name, parameters_)->paramPtr;
    }
    
    // juce::ValueTree sync
    juce::ValueTree ParameterList::GetStateAsTree() const
    {
      static juce::Identifier ParamList("Parameter_List");
      juce::ValueTree listTree(ParamList);
      
      for(const auto& entry : parameters_)
      {
        listTree.setProperty(entry.id, juce::var(entry.paramPtr->GetAsVar()), nullptr);
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
        FindEntryByName(name, parameters_)->paramPtr->SetAsVar(inTree.getProperty(name));
      }
      
      inTree.addListener(this);
    }

    void ParameterList::DesyncFromTree(juce::ValueTree& inTree)
    {
      inTree.removeListener(this);
    }

    // value tree listener callback
    void ParameterList::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property)
    {
      FindEntryByName(property, parameters_)->paramPtr->SetAsVar(tree.getProperty(property));
    }

} // namespace Haze
























