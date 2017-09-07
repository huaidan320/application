#ifndef MCARLIFEDLG_H
#define MCARLIFEDLG_H

#include <QPushButton>
#include <QMouseEvent>
//#include "QYfveFrame.h"
//#include "common/carlife_sdk.h"
#include <QDebug>

#ifdef __ARM_VOLUME_SDK__
#include "cvolume.h"
#endif
class MCarLifeDlg : public QWidget
{
    Q_OBJECT

public:
    MCarLifeDlg();
    ~MCarLifeDlg();
signals:
#ifdef __ARM_CARLIFE_SDK__
    void sendCarLifeCmd(event_st);
#endif

public slots:
    void onConnectCarlifeSlot(void);
    void onExitCarlifeSlot(void);

//    void exitSlot();

protected:
#ifdef __ARM_CARLIFE_SDK__
    virtual void carlifeSelfEvent(CarlifeEvent *e);
#endif
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void paintEvent(QPaintEvent *);
private:
    QPushButton *m_pConnectCarlifeBtn;
    QPushButton *m_pExitCarlifeBtn;
    void showBtnView(void);
#ifdef __ARM_CARLIFE_SDK__
    CarLife_Cmd *m_pCarlife;
#endif

#ifdef __ARM_VOLUME_SDK__
    CVolume *m_volume;
#endif
};

#endif // MCARLIFEDLG_H
