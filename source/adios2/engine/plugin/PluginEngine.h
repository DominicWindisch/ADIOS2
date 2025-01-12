/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * PluginEngine.h Support for an engine implemented outside libadios2
 *
 *  Created on: July 5, 2021
 *      Author: Chuck Atkins <chuck.atkins@kitware.com>
 *              Caitlin Ross <caitlin.ross@kitware.com>
 */

#ifndef ADIOS2_ENGINE_PLUGIN_PLUGINENGINE_H_
#define ADIOS2_ENGINE_PLUGIN_PLUGINENGINE_H_

#include "PluginEngineInterface.h"

#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <string>      // for string
#include <type_traits> // for add_pointer

#include "adios2/common/ADIOSMacros.h"
#include "adios2/common/ADIOSTypes.h"
#include "adios2/core/Engine.h"
#include "adios2/core/IO.h"
#include "adios2/core/Variable.h"
#include "adios2/helper/adiosComm.h"

namespace adios2
{
namespace core
{
namespace engine
{

/** A front-end wrapper for an engine implemented outside of libadios2 */
class PluginEngine : public Engine
{
public:
    // Function pointers used for the plugin factory methods
    using EngineCreatePtr = std::add_pointer<PluginEngineInterface *(
        IO &, const std::string &, const Mode, helper::Comm)>::type;
    using EngineDestroyPtr =
        std::add_pointer<void(PluginEngineInterface *)>::type;
    using EngineCreateFun =
        std::function<std::remove_pointer<EngineCreatePtr>::type>;
    using EngineDestroyFun =
        std::function<std::remove_pointer<EngineDestroyPtr>::type>;

    PluginEngine(IO &io, const std::string &name, const Mode mode,
                 helper::Comm comm);
    virtual ~PluginEngine();

    StepStatus BeginStep(StepMode mode,
                         const float timeoutSeconds = 0.f) override;
    void PerformPuts() override;
    void PerformGets() override;
    void EndStep() override;

protected:
    void Init() override;

#define declare(T)                                                             \
    void DoPutSync(Variable<T> &, const T *) override;                         \
    void DoPutDeferred(Variable<T> &, const T *) override;                     \
    void DoGetSync(Variable<T> &, T *) override;                               \
    void DoGetDeferred(Variable<T> &, T *) override;

    ADIOS2_FOREACH_STDTYPE_1ARG(declare)
#undef declare

    void DoClose(const int transportIndex = -1) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_Impl;
};

} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif /* ADIOS2_ENGINE_PLUGIN_PLUGINENGINE_H_ */
