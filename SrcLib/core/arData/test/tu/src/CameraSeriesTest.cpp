/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTest/helper/compare.hpp>

#include <arData/CameraSeries.hpp>
#include <arData/Camera.hpp>

#include <arDataCamp/Version.hpp>

#include <fwCore/spyLog.hpp>

#include "CameraSeriesTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::CameraSeriesTest );

namespace arData
{
namespace ut
{

//------------------------------------------------------------------------------

void CameraSeriesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CameraSeriesTest::tearDown()
{
    //Force link with arDataCamp
    const int arfVersion = ::arDataCamp::Version::s_CURRENT_VERSION;
}

//------------------------------------------------------------------------------

::arData::CameraSeries::sptr initCameraSeries()
{
    ::arData::CameraSeries::sptr series = ::arData::CameraSeries::New();

    // --------------- Camera 1 ----------------------
    ::arData::Camera::sptr camera1 = ::arData::Camera::New();
    camera1->setCx(3.5);
    camera1->setCy(9.3);
    camera1->setFx(48.2);
    camera1->setFy(7.3);
    camera1->setSkew(1.9);
    camera1->setDistortionCoefficient(1.1, 2.2, 3.3, 4.4, 5.5);
    series->addCamera(camera1);

    // --------------- Camera 2 ----------------------
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();
    camera2->setCx(86.5);
    camera2->setCy(543.);
    camera2->setFx(4.4);
    camera2->setFy(5.14);
    camera2->setSkew(0.19);
    camera2->setDistortionCoefficient(2.4, 5.1, 66., 4.1, 6.4);
    series->addCamera(camera2);

    // --------------- Extrinsic matrix ----------------------
    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
    for (int i = 0; i<4; ++i)
    {
        for (int j = 0; j<4; ++j)
        {
            mat->setCoefficient(i, j, 2*i+j);
        }
    }

    series->setExtrinsicMatrix(1, mat);

    return series;
}

//------------------------------------------------------------------------------

void CameraSeriesTest::cameraTest()
{
    ::arData::CameraSeries::sptr series = ::arData::CameraSeries::New();
    CPPUNIT_ASSERT(series);

    ::fwData::TransformationMatrix3D::sptr identity = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr mat      = ::fwData::TransformationMatrix3D::New();
    for (int i = 0; i<4; ++i)
    {
        for (int j = 0; j<4; ++j)
        {
            mat->setCoefficient(i, j, 2*i+j);
        }
    }

    ::arData::Camera::sptr camera1 = ::arData::Camera::New();
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();
    ::arData::Camera::sptr camera3 = ::arData::Camera::New();

    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera1));
    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera2));
    CPPUNIT_ASSERT_THROW(series->addCamera(camera2), ::fwCore::Exception);

    CPPUNIT_ASSERT(series->getExtrinsicMatrix(0));
    CPPUNIT_ASSERT(::fwTest::helper::compare(identity, series->getExtrinsicMatrix(0)));
    CPPUNIT_ASSERT(!series->getExtrinsicMatrix(1));
    CPPUNIT_ASSERT_NO_THROW(series->setExtrinsicMatrix(1, mat));
    CPPUNIT_ASSERT_THROW(series->setExtrinsicMatrix(2, mat), ::fwCore::Exception);
    CPPUNIT_ASSERT_THROW(series->getExtrinsicMatrix(2), ::fwCore::Exception);
    CPPUNIT_ASSERT(series->getExtrinsicMatrix(1) == mat);

    CPPUNIT_ASSERT_EQUAL(size_t(2), series->getNumberOfCameras());

    CPPUNIT_ASSERT(series->getCamera(0) == camera1);
    CPPUNIT_ASSERT(series->getCamera(1) == camera2);
    CPPUNIT_ASSERT_THROW(series->getCamera(2), ::fwCore::Exception);

    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera3));
    CPPUNIT_ASSERT(series->getCamera(2) == camera3);

    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera1));
    CPPUNIT_ASSERT_EQUAL(size_t(2), series->getNumberOfCameras());
    CPPUNIT_ASSERT(series->getCamera(0) == camera2);
    CPPUNIT_ASSERT_THROW(series->removeCamera(camera1), ::fwCore::Exception);

    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera2));
    CPPUNIT_ASSERT_EQUAL(size_t(1), series->getNumberOfCameras());
    CPPUNIT_ASSERT(series->getCamera(0) == camera3);
    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera3));

    CPPUNIT_ASSERT_EQUAL(size_t(0), series->getNumberOfCameras());
}

//------------------------------------------------------------------------------

void CameraSeriesTest::shallowCopyTest()
{
    ::arData::CameraSeries::sptr series  = initCameraSeries();
    ::arData::CameraSeries::sptr series2 = ::arData::CameraSeries::New();

    series2->shallowCopy(series);

    CPPUNIT_ASSERT_EQUAL(series->getNumberOfCameras(), series2->getNumberOfCameras());
    CPPUNIT_ASSERT_EQUAL(series->getCamera(0), series2->getCamera(0));
    CPPUNIT_ASSERT_EQUAL(series->getCamera(1), series2->getCamera(1));
}

//------------------------------------------------------------------------------

void CameraSeriesTest::deepCopyTest()
{
    ::arData::CameraSeries::sptr series = initCameraSeries();
    ::arData::CameraSeries::sptr series2;
    series2 = ::fwData::Object::copy< ::arData::CameraSeries >(series);

    CPPUNIT_ASSERT(::fwTest::helper::compare(series, series2));
}

} //namespace ut
} //namespace arData
