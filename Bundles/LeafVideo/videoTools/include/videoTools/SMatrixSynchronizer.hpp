/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SMATRIXSYNCHRONIZER_HPP__
#define __VIDEOTOOLS_SMATRIXSYNCHRONIZER_HPP__

#include "videoTools/ISynchronizer.hpp"
#include "videoTools/config.hpp"

#include <extData/MatrixTL.hpp>

#include <fwCore/base.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwData/TransformationMatrix3D.hpp>



namespace videoTools
{
/**
 * @brief   SMatrixSynchronizer reads the content of a MatrixTL into a TransformationMatrix3D.
 * @class   SMatrixSynchronizer
 */
class VIDEOTOOLS_CLASS_API SMatrixSynchronizer : public ::videoTools::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SMatrixSynchronizer)(::videoTools::ISynchronizer));

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SMatrixSynchronizer() throw ();

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~SMatrixSynchronizer() throw ()
    {
    }

    /// Slot definition
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_MATRIX_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> UpdateMatrixSlotType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @verbatim
       <service impl="::videoTools::SMatrixSynchronizer" type="::videoTools::ISynchronizer">
            <TL>MatrixTLUid</TL>
       </service>
       @endverbatim
     * - \b TL : Defines the uid of the MatrixTL containing the matrices.
     */
    VIDEOTOOLS_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    VIDEOTOOLS_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOTOOLS_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOTOOLS_API void swapping() throw (fwTools::Failed)
    {
    }

    /// Does nothing.
    VIDEOTOOLS_API void updating() throw (fwTools::Failed)
    {
    }

private:

    /// The uid of the MatrixTL
    std::string m_matrixUid;
    /// The matrixTL
    ::extData::MatrixTL::sptr m_matrixTL;
    /// slot to update the TransformationMatrix3D with the timeline buffer
    UpdateMatrixSlotType::sptr m_slotUpdateMatrix;
    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// This function fills the TransformationMatrix3D with the current buffer content of the MatrixTL
    void updateMatrix(::fwCore::HiResClock::HiResClockType timestamp);


};

} //namespace videoTools
#endif  // __VIDEOTOOLS_SMATRIXSYNCHRONIZER_HPP__