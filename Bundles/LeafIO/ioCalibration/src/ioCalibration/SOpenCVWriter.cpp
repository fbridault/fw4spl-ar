/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioCalibration/SOpenCVWriter.hpp"

#include <arData/CameraSeries.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/core.hpp>

#include <sstream>

namespace ioCalibration
{
fwServicesRegisterMacro(::io::IWriter, ::ioCalibration::SOpenCVWriter);
// ----------------------------------------------------------------------------

SOpenCVWriter::SOpenCVWriter()
{

}

// ----------------------------------------------------------------------------

SOpenCVWriter::~SOpenCVWriter()
{

}

// ----------------------------------------------------------------------------

void SOpenCVWriter::configuring() throw (::fwTools::Failed)
{
    ::io::IWriter::configuring();
}

// ----------------------------------------------------------------------------

void SOpenCVWriter::configureWithIHM() throw (::fwTools::Failed)
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool SOpenCVWriter::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Enter file name");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML file", "*.xml");
    dialogFile.addFilter("YAML file", "*.yaml *.yml");

    ::fwData::location::SingleFile::sptr result
        = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile( _sDefaultPath );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
        ok = true;
    }
    else
    {
        this->clearLocations();
    }

    return ok;
}

//----------------------------------------------------------------------------

void SOpenCVWriter::starting() throw (::fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SOpenCVWriter::stopping() throw (::fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SOpenCVWriter::updating() throw (::fwTools::Failed)
{

    ::arData::CameraSeries::csptr camSeries = this->getInput< ::arData::CameraSeries >("target");
    SLM_ASSERT("CameraSeries is null", camSeries);

    bool use_dialog = false;
    if(!this->hasLocationDefined())
    {
        use_dialog = this->defineLocationGUI();
        if(!use_dialog)
        {
            return;
        }
    }

    ::fwData::mt::ObjectReadLock lock(camSeries);
    size_t numberOfCameras = camSeries->getNumberOfCameras();

    std::vector< ::arData::Camera::sptr > cameras;
    std::vector< ::cv::Mat > cameraMatrices;
    std::vector< ::cv::Mat > cameraDistCoefs;

    // Set the cameras
    ::fwData::TransformationMatrix3D::sptr extrinsicMatrix;
    ::cv::Mat extrinsic = ::cv::Mat::eye(4, 4, CV_64F);

    ::fwData::mt::ObjectReadLock camSeriesLock(camSeries);

    for(size_t i = 0; i < numberOfCameras; ++i)
    {
        cameras.push_back(camSeries->getCamera(i));
        cameraMatrices.push_back(::cv::Mat::eye(3, 3, CV_64F));
        cameraDistCoefs.push_back(::cv::Mat::eye(5, 1, CV_64F));

        cameraMatrices[i].at<double>(0, 0) = cameras[i]->getFx();
        cameraMatrices[i].at<double>(1, 1) = cameras[i]->getFy();
        cameraMatrices[i].at<double>(0, 2) = cameras[i]->getCx();
        cameraMatrices[i].at<double>(1, 2) = cameras[i]->getCy();

        for(int c = 0; c < 5; ++c)
        {
            cameraDistCoefs[i].at<double>(c, 0) = cameras[i]->getDistortionCoefficient()[static_cast<size_t>(c)];
        }
    }

    ::cv::FileStorage fs(this->getFile().string().c_str(), ::cv::FileStorage::WRITE);

    fs<<"nbCameras"<<static_cast<int>(numberOfCameras);

    for( size_t c = 0; c < numberOfCameras; ++c)
    {
        std::stringstream camNum;
        camNum << "camera_"<<c;

        fs << camNum.str() << "{";
        fs << "id"<<camSeries->getCamera(c)->getCameraID().c_str();
        fs << "description" << camSeries->getCamera(c)->getDescription().c_str();
        fs << "imageWidth" << static_cast< int> (camSeries->getCamera(c)->getWidth());
        fs << "imageHeight" << static_cast< int >(camSeries->getCamera(c)->getHeight());
        fs << "matrix" << cameraMatrices[c];
        fs << "distortion" << cameraDistCoefs[c];

        extrinsicMatrix = camSeries->getExtrinsicMatrix(c);
        if(extrinsicMatrix)
        {
            for(int i = 0; i < 4; ++i)
            {
                for(int j = 0; j < 4; ++j)
                {
                    extrinsic.at< double >(i, j) = extrinsicMatrix->getCoefficient(static_cast<size_t>(i),
                                                                                   static_cast<size_t>(j));
                }
            }
            fs<<"extrinsic"<<extrinsic;
        }
        fs << "}";
    }

    fs.release();

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

void SOpenCVWriter::swapping() throw (::fwTools::Failed)
{
    this->stop();
    this->start();
}

// ----------------------------------------------------------------------------

::io::IOPathType SOpenCVWriter::getIOPathType() const
{
    return ::io::FILE;
}

// ----------------------------------------------------------------------------
} //namespace ioCalibration