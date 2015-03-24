/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SOPENCVINTRINSIC_HPP__
#define __VIDEOCALIBRATION_SOPENCVINTRINSIC_HPP__

#include "videoCalibration/ICalibration.hpp"
#include "videoCalibration/config.hpp"

namespace videoCalibration
{
/**
 * @brief   SOpenCVIntrinsic service that computes intrinsic calibration with openCV.
 * @class   SOpenCVIntrinsic
 */
class VIDEOCALIBRATION_CLASS_API SOpenCVIntrinsic : public ::videoCalibration::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVIntrinsic)(::videoCalibration::ICalibration));

    /// Constructor.
    VIDEOCALIBRATION_API SOpenCVIntrinsic() throw ();

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~SOpenCVIntrinsic() throw ();

protected:

    /// Does nothing
    /**
     * @brief Configures the service.
     * @verbatim
       <service impl="::videoCalibration::SOpenCVIntrinsic" >
            <calibrationInfoID> ... </calibrationInfoID>
            <board width="17" height="13" />
       </service>
       @endverbatim
     * - \b calibrationInfoID: FwId of calibrationInfo.
     * - \b board : defines the number of square in 2 dimensions of the chessboard.
     */
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Calls stopping and starting.
    VIDEOCALIBRATION_API void swapping() throw (fwTools::Failed);

    /// Computes intrinsic calibration
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Removes connections
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

private:

    ///  FwId of calibrationInfo
    std::string m_calibrationInfoID;

    /// Width of the chessboard used for calibration
    unsigned int m_width;

    /// Height of the chessboard used for calibration
    unsigned int m_height;


};
} // namespace videoCalibration

#endif // __VIDEOCALIBRATION_SOPENCVINTRINSIC_HPP__