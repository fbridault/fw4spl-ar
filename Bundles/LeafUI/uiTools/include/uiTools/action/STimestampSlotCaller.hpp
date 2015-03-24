/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_STIMESTAMPSLOTCALLER_HPP__
#define __UITOOLS_STIMESTAMPSLOTCALLER_HPP__

#include <fwCore/base.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiTools/config.hpp"


namespace uiTools
{
namespace action
{

/**
 * @class STimestampSlotCaller
 * @brief Runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.
 *
 * @verbatim
     ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
     slot->asyncRun(timestamp);
   @endverbatim
 */
class UITOOLS_CLASS_API STimestampSlotCaller : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro ( (STimestampSlotCaller)(::fwGui::IActionSrv) );

    /// Constructor
    UITOOLS_API STimestampSlotCaller();

    /// Destructor
    UITOOLS_API ~STimestampSlotCaller();


protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string HasSlotIDType;
    typedef std::pair< HasSlotIDType, ::fwCom::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    /// Does nothing
    UITOOLS_API virtual void starting() throw( ::fwTools::Failed );

    /**
     * @brief Configure the service
     *
     * @verbatim
        <service uid="..." type="::fwGui::IActionSrv" impl="::uiTools::STimestampSlotCaller" autoConnect="no">
            <slots>
              <slot>hasSlotsId/slotKey</slot>
              <slot>hasSlots2Id/slot2Key</slot>
          </slots>
        </service>
       @endverbatim
     * - \b slot : defines the slot to run
     *   - \b hasSlotsId : fwId of the HasSlots (usually a service)
     *   - \b slotKey : the identifier of the slot in the HasSlots
     */
    UITOOLS_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing
    UITOOLS_API virtual void stopping() throw( ::fwTools::Failed );

    /// This method run the specified slots.
    UITOOLS_API virtual void updating() throw( ::fwTools::Failed );

    /// vector representing slots
    SlotInfoContainerType m_slotInfos;
};

} // namespace action
} // namespace uiTools


#endif /* __UITOOLS_STIMESTAMPSLOTCALLER_HPP__ */