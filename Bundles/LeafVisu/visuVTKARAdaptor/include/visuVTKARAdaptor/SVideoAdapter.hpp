/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SVIDEOADAPTER_HPP__
#define __VISUVTKARADAPTOR_SVIDEOADAPTER_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

class vtkImageData;
class vtkTexture;
class vtkActor;


namespace visuVTKARAdaptor
{

/**
 * @brief   Adaptor to render a video frame from a 2D-image.
 * @class   SVideoAdapter
 */
class VISUVTKARADAPTOR_CLASS_API SVideoAdapter : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SVideoAdapter)(::fwRenderVTK::IVtkAdaptorService) );

    /// Constructor
    SVideoAdapter() throw();

    /// Destructor
    virtual ~SVideoAdapter() throw();

protected:

    /// Create the actor and mapper used to show the video frame.
    VISUVTKARADAPTOR_API void doStart() throw(fwTools::Failed);

    /**
     * @verbatim
       <adaptor id="video" class="::visuVTKARAdaptor::SVideoAdapter" objectId="imageKey">
        <config renderer="default" cameraUID="..." reverse="true" />
       </adaptor>
       @endverbatim
     * - \b renderer : defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b cameraUID (optional) : defines the uid of the camera used to place video plane.
     * - \b reverse (optional)(default: true) : if true, the actor is rotated in z and y axis.
     */
    VISUVTKARADAPTOR_API void configuring() throw(fwTools::Failed);

    /// Calls doUpdate()
    VISUVTKARADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updated the frame from the current Image.
    VISUVTKARADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /// Removes the actor from the renderer
    VISUVTKARADAPTOR_API void doStop() throw(fwTools::Failed);

    /// Listens the image message to update the adaptor.
    VISUVTKARADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

private:

    /// Update image opacity and visibility
    void updateImageOpacity();

    vtkImageData* m_imageData; ///< vtk image created from current data Image. It is shown in the frame.
    vtkTexture* m_texture;  ///< texture used to show the image
    vtkActor * m_actor;  ///< actor to show frame

    bool m_isTextureInit; /// true if the texture is initialized

    std::string m_cameraUID; ///< uid of the camera

    bool m_reverse; ///< if true, the actor is rotated in z and y axis.
};

} //namespace visuVTKARAdaptor

#endif /* __VISUVTKARADAPTOR_SVIDEOADAPTER_HPP__ */