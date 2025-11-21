#include "core/mpr/mprInteractionRouter.h"
#include <vtkCommand.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkResliceCursor.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceImageViewer.h>
#include "core/mpr/mprAssembly.h"
#include "core/mpr/mprState.h"

namespace core::mpr {

    MprInteractionRouter::MprInteractionRouter()
        : QObject(nullptr)
    {
        m_connector = vtkEventQtSlotConnect::New();
        m_timer.invalidate();
    }

    MprInteractionRouter::~MprInteractionRouter()
    {
        unwire();
        if (m_connector) {
            m_connector->Delete();
            m_connector = nullptr;
        }
    }

    void MprInteractionRouter::setAssembly(MprAssembly* assembly)
    {
        m_assembly = assembly;
    }

    void MprInteractionRouter::setState(MprState* state)
    {
        m_state = state;
    }

    void MprInteractionRouter::wire()
    {
        if (!m_connector || !m_assembly) {
            return;
        }

        connectViewer(m_assembly->axialViewer());
        connectViewer(m_assembly->coronalViewer());
        connectViewer(m_assembly->sagittalViewer());

        /*if (auto* actor = m_assembly->cursorActor()) {
            m_connector->Connect(actor, vtkCommand::InteractionEvent, this, SLOT(handleInteraction()));
        }*/

        m_timer.start();
    }

    void MprInteractionRouter::unwire()
    {
        if (m_connector) {
            m_connector->Disconnect();
        }
    }

    void MprInteractionRouter::handleInteraction()
    {
        if (!m_state || !m_assembly) {
            return;
        }

        if (m_timer.isValid() && m_timer.elapsed() < m_minIntervalMs) {
            return;
        }

        // 直接刷新所有视图，保持 2D/3D 同步。
        m_assembly->refreshAll();
        m_timer.restart();
    }

    void MprInteractionRouter::connectViewer(vtkResliceImageViewer* viewer)
    {
        if (!viewer || !m_connector) {
            return;
        }

        if (auto* widget = viewer->GetResliceCursorWidget()) {
            m_connector->Connect(widget, vtkCommand::InteractionEvent, this, SLOT(handleInteraction()));
        }
    }

} // namespace core::mpr