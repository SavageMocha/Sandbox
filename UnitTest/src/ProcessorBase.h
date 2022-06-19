
#pragma once

#include "ParameterTypes.h"

namespace Haze
{

    class ProcessorInterface
    {
    public:
        // virtual interface
        virtual ~ProcessorInterface() = default;

        virtual const ParameterList& getUiParameterList() const = 0;

        virtual void exec() = 0;

    }; // class ProcessorInterface

    

    class ProcessorProxy
    {
    public:
        void SetProcessor(const ProcessorInterface& processor)
        {
        }

    private:
        ParameterList paramList_;
    }; // class ProcessorProxy

} // namespace Haze