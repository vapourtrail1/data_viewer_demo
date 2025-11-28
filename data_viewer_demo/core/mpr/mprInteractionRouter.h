#pragma once
#include <QElapsedTimer>
#include <QObject>

class vtkEventQtSlotConnect;
class vtkResliceImageViewer;

namespace core::mpr {

    class MprAssembly;
    class MprState;

    /*
     MprInteractionRouter 将 VTK 交互事件映射为状态更新。
     */
    class MprInteractionRouter : public QObject
    {
        Q_OBJECT
    public:
        MprInteractionRouter();
        ~MprInteractionRouter() override;

        void setAssembly(MprAssembly* assembly);
        void setState(MprState* state);

        void wire();
        void unwire();

    private slots:
        void handleInteraction();

    private:
        void connectViewer(vtkResliceImageViewer* viewer);

        MprState* m_state = nullptr;
        MprAssembly* m_assembly = nullptr;
        vtkEventQtSlotConnect* m_connector = nullptr;
        QElapsedTimer m_timer;
        int m_minIntervalMs = 16;
    };

} // namespace core::mpr