/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_EDITOR_STEXTSTATUS_HPP__
#define __UITOOLS_EDITOR_STEXTSTATUS_HPP__

#include "uiTools/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QPointer>

class QLabel;

namespace uiTools
{

namespace editor
{

/**
 * @brief   STextStatus is used to displays and update values (int, double or string) in a QLabel.
 * Values are set using slots
 *
 * @code{.xml}
 * <service uid="..." type="::uiTools::editor::STextStatus">
 *    <label> my label </label>
 *    <color> my color </color>
 * </service>
 * @endcode
 * - \b label (optional, by default "") : the text to show before size of the vector
 * - \b color (optional, by default "red") : the needed color of the displayed label in a CSS style as names (ex: red),
 * rgb/rgba (ex: rgb(0,255,137,0.3)) or hexadecimal (ex: #355C66).
 */
class UITOOLS_CLASS_API STextStatus : public QObject,
                                      public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (STextStatus)(::gui::editor::IEditor) )

    /// Constructor. Do nothing.
    UITOOLS_API STextStatus();

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~STextStatus();

    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_SET_STRING_PARAMETER_SLOT;

protected:

    /**
     * @brief Configure the service
     */
    virtual void configuring() override;

    /**
     * @brief Install the layout.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() override;

    /// Does nothing
    virtual void updating() override;

    /// Does nothing
    virtual void swapping() override;

private:

    ///Slot called when a integer value is changed
    void setIntParameter(int _val);
    ///Slot called when a double value is changed
    void setDoubleParameter(double _val);
    ///Slot called when a boolean value is changed
    void setBoolParameter(bool _val);
    ///Slot called when a enum value is changed
    void setStringParameter(std::string _val);

    ///Value to be displayed
    QPointer< QLabel > m_labelValue;
    ///Static text to be displayed (ex: MyValue: #Value )
    QPointer< QLabel > m_labelStaticText;

};

} // namespace editor
} // namespace uiTools

#endif /*__UITOOLS_EDITOR_STEXTSTATUS_HPP__*/
