/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SCameraConfigLauncher.hpp"

#include <arData/CalibrationInfo.hpp>
#include <arData/Camera.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataIO/reader/TransformationMatrix3DReader.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>

namespace uiCalibration
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SCameraConfigLauncher, ::fwData::Composite);

SCameraConfigLauncher::SCameraConfigLauncher() noexcept
{
}

//------------------------------------------------------------------------------

SCameraConfigLauncher::~SCameraConfigLauncher() noexcept
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::configuring()
{
    this->initialize();
    ::fwServices::IService::ConfigType configuration = this->getConfigTree();

    SLM_ASSERT("There must be one (and only one) <config/> element.",
               configuration.count("config") == 1 );
    const ::fwServices::IService::ConfigType& srvconfig = configuration;
    const ::fwServices::IService::ConfigType& config    = srvconfig.get_child("config");

    const ::fwServices::IService::ConfigType& intrinsic = config.get_child("intrinsic");
    const ::fwServices::IService::ConfigType& extrinsic = config.get_child("extrinsic");

    m_intrinsicLauncher.parseConfig(intrinsic, this->getSptr());
    m_extrinsicLauncher.parseConfig(extrinsic,  this->getSptr());
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::starting()
{
    this->create();

    m_cameraSeries = this->getInOut< ::arData::CameraSeries >("cameraSeries");
    SLM_ASSERT("Missing cameraSeries.", m_cameraSeries);

    m_activitySeries = this->getInOut< ::fwMedData::ActivitySeries >("activitySeries");
    SLM_ASSERT("Missing activitySeries.", m_activitySeries);

    auto qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QHBoxLayout* layout = new QHBoxLayout();

    m_cameraComboBox = new QComboBox();
    layout->addWidget(m_cameraComboBox);

    QIcon addIcon(QString::fromStdString(::fwRuntime::getBundleResourceFilePath("media", "icons/Import.svg").string()));
    m_addButton = new QPushButton(addIcon, "");
    m_addButton->setToolTip("Add a new camera.");
    layout->addWidget(m_addButton);

    QIcon importIcon(QString::fromStdString(::fwRuntime::getBundleResourceFilePath("arMedia",
                                                                                   "icons/CameraSeries.svg").string()));
    m_importButton = new QPushButton(importIcon, "");
    m_importButton->setToolTip("Import an intrinsic calibration.");
    layout->addWidget(m_importButton);

    QIcon removeIcon(QString::fromStdString(::fwRuntime::getBundleResourceFilePath("arMedia",
                                                                                   "icons/remove.svg").string()));
    m_removeButton = new QPushButton(removeIcon, "");
    m_removeButton->setToolTip("Remove the camera.");
    layout->addWidget(m_removeButton);

    m_extrinsicButton = new QPushButton("Extrinsic");
    layout->addWidget(m_extrinsicButton);
    m_extrinsicButton->setCheckable(true);

    qtContainer->setLayout( layout );

    const size_t nbCam = m_cameraSeries->getNumberOfCameras();

    if (nbCam == 0)
    {
        this->addCamera();

        m_extrinsicButton->setEnabled(false);
        m_removeButton->setEnabled(false);
    }
    else
    {
        for (size_t i = 0; i < nbCam; ++i)
        {
            m_cameraComboBox->addItem(QString("Camera %1").arg(i+1));
        }

        const bool moreThanOneCamera = (nbCam > 1);

        m_extrinsicButton->setEnabled(moreThanOneCamera);
        m_removeButton->setEnabled(moreThanOneCamera);
    }

    QObject::connect(m_cameraComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCameraChanged(int)));
    QObject::connect(m_addButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    QObject::connect(m_importButton, SIGNAL(clicked()), this, SLOT(onImportClicked()));
    QObject::connect(m_removeButton, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));
    QObject::connect(m_extrinsicButton, SIGNAL(toggled(bool)), this, SLOT(onExtrinsicToggled(bool)));

    this->startIntrinsicConfig(0);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::stopping()
{
    m_intrinsicLauncher.stopConfig();
    m_extrinsicLauncher.stopConfig();

    this->destroy();
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::updating()
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::swapping()
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onCameraChanged(int index)
{
    OSLM_ASSERT("Bad index: " << index,
                index >= 0 && static_cast<size_t>(index) < m_cameraSeries->getNumberOfCameras());

    if (index == 0)
    {
        m_extrinsicButton->setChecked(false);
        m_extrinsicButton->setEnabled(false);
    }
    else
    {
        m_extrinsicButton->setEnabled(true);
    }

    if (m_extrinsicButton->isChecked())
    {
        this->startExtrinsicConfig(static_cast<size_t>(index));
    }
    else
    {
        this->startIntrinsicConfig(static_cast<size_t>(index));
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onAddClicked()
{
    m_extrinsicButton->setEnabled(true);
    m_removeButton->setEnabled(true);

    this->addCamera();
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onImportClicked()
{
    ::fwGui::dialog::LocationDialog dialog;
    dialog.addFilter("Matrix", "*.trf");
    dialog.setTitle("Select the matrix file to load");
    dialog.setOption(::fwGui::dialog::ILocationDialog::READ);

    auto result = ::fwData::location::SingleFile::dynamicCast( dialog.show() );

    bool success = true;
    size_t width, height;
    try
    {
        ::fwGui::dialog::InputDialog widthDialog;
        widthDialog.setTitle("Width of the camera frames");
        widthDialog.setMessage("Enter the width in pixels of the camera frames: ");
        widthDialog.setInput("<Width>");
        auto widthStr = widthDialog.getInput();
        width = std::stoul(widthStr);

        ::fwGui::dialog::InputDialog heightDialog;
        heightDialog.setTitle("Height of the camera frames");
        heightDialog.setMessage("Enter the height in pixels of the camera frames: ");
        heightDialog.setInput("<Height>");
        auto heightStr = heightDialog.getInput();
        height = std::stoul(heightStr);
    }
    catch(std::exception const& e)
    {
        success = false;
    }

    if(result && success)
    {
        auto matrix = ::fwData::TransformationMatrix3D::New();
        auto reader = ::fwDataIO::reader::TransformationMatrix3DReader::New();
        reader->setObject( matrix );
        reader->setFile(result->getPath());
        reader->read();

        const auto camIdx = m_cameraComboBox->currentIndex();
        auto camera       = m_cameraSeries->getCamera(camIdx);
        camera->setFx(matrix->getCoefficient(0, 0));
        camera->setFy(matrix->getCoefficient(1, 1));
        camera->setCx(matrix->getCoefficient(0, 2));
        camera->setCy(matrix->getCoefficient(1, 2));
        camera->setHeight(height);
        camera->setWidth(width);
        camera->setIsCalibrated(true);
        camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType>(::arData::Camera::s_INTRINSIC_CALIBRATED_SIG)
        ->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onRemoveClicked()
{
    const size_t index = static_cast<size_t>(m_cameraComboBox->currentIndex());
    if (index > 0)
    {
        m_cameraComboBox->blockSignals(true);

        // Remove camera
        ::arData::Camera::sptr camera = m_cameraSeries->getCamera(index);
        m_cameraSeries->removeCamera(camera);
        ::arData::CameraSeries::RemovedCameraSignalType::sptr sig;
        sig = m_cameraSeries->signal< ::arData::CameraSeries::RemovedCameraSignalType >(
            ::arData::CameraSeries::s_REMOVED_CAMERA_SIG);
        sig->asyncEmit(camera);

        // Remove calibrationInfo
        std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);
        m_activitySeries->getData()->getContainer().erase(calibrationInfoKey);

        const size_t nbCam = m_cameraSeries->getNumberOfCameras();
        if (nbCam == 1)
        {
            m_extrinsicButton->setEnabled(false);
            m_removeButton->setEnabled(false);
        }
        // Renamed all items from 1 to nbCam
        m_cameraComboBox->clear();
        for (size_t i = 0; i < nbCam; ++i)
        {
            m_cameraComboBox->addItem(QString("Camera %1").arg(i+1));
        }
        // select first camera
        m_cameraComboBox->setCurrentIndex(0);
        this->startIntrinsicConfig(0);
        m_cameraComboBox->blockSignals(false);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", "The first camera can not be deleted");
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onExtrinsicToggled(bool checked)
{
    const size_t index = static_cast<size_t>(m_cameraComboBox->currentIndex());
    OSLM_ASSERT("Bad index: " << index, index < m_cameraSeries->getNumberOfCameras());
    if (checked)
    {
        this->startExtrinsicConfig(index);
    }
    else
    {
        this->startIntrinsicConfig(index);
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startIntrinsicConfig(size_t index)
{
    ::fwServices::helper::ConfigLauncher::FieldAdaptorType replaceMap;

    ::arData::Camera::sptr camera = m_cameraSeries->getCamera(index);

    std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);
    ::fwData::Composite::sptr data            = m_activitySeries->getData();
    ::arData::CalibrationInfo::sptr calibInfo =
        ::arData::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfoKey]);

    replaceMap["camera"]          = camera->getID();
    replaceMap["calibrationInfo"] = calibInfo->getID();

    m_extrinsicLauncher.stopConfig();
    m_intrinsicLauncher.stopConfig();
    m_intrinsicLauncher.startConfig(this->getSptr(), replaceMap);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startExtrinsicConfig(size_t index)
{
    const size_t cameraIdx = std::max(index, size_t(1));

    ::arData::Camera::sptr camera1 = m_cameraSeries->getCamera(0);
    ::arData::Camera::sptr camera2 = m_cameraSeries->getCamera(cameraIdx);

    // Check if the two cameras are calibrated
    if (camera1->getIsCalibrated() && camera2->getIsCalibrated())
    {
        // Add 2 calibration info in ActivitySeries if not exist
        std::string calibrationInfo1Key = "calibrationInfoExtr0_" + ::boost::lexical_cast<std::string>(cameraIdx);
        std::string calibrationInfo2Key = "calibrationInfoExtr1_" + ::boost::lexical_cast<std::string>(cameraIdx);

        ::fwData::Composite::sptr data = m_activitySeries->getData();
        ::arData::CalibrationInfo::sptr calibInfo1;
        ::arData::CalibrationInfo::sptr calibInfo2;
        // Get the calibrationInfo from the activity series if it exists or create it.
        if (data->find(calibrationInfo1Key) == data->end() || data->find(calibrationInfo2Key) == data->end())
        {
            calibInfo1 = ::arData::CalibrationInfo::New();
            calibInfo2 = ::arData::CalibrationInfo::New();

            data->getContainer()[calibrationInfo1Key] = calibInfo1;
            data->getContainer()[calibrationInfo2Key] = calibInfo2;
        }
        else
        {
            calibInfo1 = ::arData::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfo1Key]);
            calibInfo2 = ::arData::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfo2Key]);
        }

        ::fwServices::registry::FieldAdaptorType replaceMap;

        replaceMap["camera1"]          = camera1->getID();
        replaceMap["camera2"]          = camera2->getID();
        replaceMap["calibrationInfo1"] = calibInfo1->getID();
        replaceMap["calibrationInfo2"] = calibInfo2->getID();
        replaceMap["camIndex"]         = std::to_string(index);

        m_extrinsicLauncher.stopConfig();
        m_intrinsicLauncher.stopConfig();
        m_extrinsicLauncher.startConfig(this->getSptr(), replaceMap);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Calibration", "Cameras must be intrinsically calibrated.");
        m_extrinsicButton->setChecked(false);
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::addCamera()
{
    const size_t nbCam = m_cameraSeries->getNumberOfCameras();

    ::arData::Camera::sptr camera = ::arData::Camera::New();

    // Add the CalibrationInfo in activitySeries to be saved in activity
    std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(nbCam);
    ::arData::CalibrationInfo::sptr calibInfo                       = ::arData::CalibrationInfo::New();
    m_activitySeries->getData()->getContainer()[calibrationInfoKey] = calibInfo;

    // Add the camera
    m_cameraSeries->addCamera(camera);
    ::arData::CameraSeries::AddedCameraSignalType::sptr sig;
    sig = m_cameraSeries->signal< ::arData::CameraSeries::AddedCameraSignalType >(
        ::arData::CameraSeries::s_ADDED_CAMERA_SIG);
    sig->asyncEmit(camera);

    m_cameraComboBox->blockSignals(true);
    m_cameraComboBox->addItem(QString("Camera %1").arg(nbCam+1));
    m_cameraComboBox->setCurrentIndex(static_cast<int>(nbCam));
    m_extrinsicButton->setChecked(false);
    this->startIntrinsicConfig(nbCam);
    m_cameraComboBox->blockSignals(false);
}

//------------------------------------------------------------------------------

} // namespace uiCalibration

