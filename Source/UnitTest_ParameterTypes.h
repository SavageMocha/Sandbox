/*
  ==============================================================================

    UnitTest_ParameterTypes.h
    Created: 15 May 2022 2:39:07pm
    Author:  maxmo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Haze
{
namespace UnitTests
{
  
  class UiParamTest : public juce::UnitTest
  {
  public:
    // ctor
    UiParamTest() : UnitTest("Ui Parameter types") {}

    virtual void runTest() override final;
    
  private:

    
  }; // UiParamTest
  
  static UiParamTest Test; // static addition to the test array
  
} // UnitTests
} // Haze
