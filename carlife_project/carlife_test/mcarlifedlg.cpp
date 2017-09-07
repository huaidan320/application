#include "mcarlifedlg.h"
#include <QPainter>
#include <QDebug>
//#include "mtexttipwidget.h"

#ifdef __ARM_CAMERA_SDK__
#include "cameracmd.h"
#endif
#ifdef __ARM_PLAYER_SDK__
#include <playermanage.h>
#endif

MCarLifeDlg::MCarLifeDlg()
{
    setStyleSheet("QWidget{background-color:rgb(61,61,61); color:white;} QWidget:focus{outline:none;}");
    resize(1120 , 480 );
    showBtnView();
#ifdef __ARM_CARLIFE_SDK__
    m_pCarlife = CarLife_Cmd::Instance();
#endif

#ifdef __ARM_VOLUME_SDK__
    m_volume = CVolume::Instance();
#endif

}

MCarLifeDlg::~MCarLifeDlg()
{
}

#ifdef __ARM_CARLIFE_SDK__
void MCarLifeDlg::carlifeSelfEvent(CarlifeEvent *e)
{
    switch (e->getCarlifeEventType()) {
    case  CARLIFE_ATTACH:   //Carlife device attach,当carlife界面show出时，插上手机carlife会发生发出此信号，因此连接按钮不能在此隐藏。
        MTextTipWidget::showTextTip(tr("Crarlife连接成功"),  this);
//        if(!m_pConnectCarlifeBtn->isVisible())
//            m_pConnectCarlifeBtn->setVisible(true);
        if(!m_pExitCarlifeBtn->isVisible())
             m_pExitCarlifeBtn->setVisible(true);
        MCARLIFE("CARLIFE_ATTACH");
        break;
    case  CARLFIE_DETACH:             //Carlife device deatach
//        m_pExitCarlifeBtn->show();
//        m_pConnectCarlifeBtn->show();
        MTextTipWidget::showTextTip(tr("Crarlife已断开"),  this);
        onExitCarlifeSlot();
        MCARLIFE("CARLFIE_DETACH");
        break;
    case  CARLIFE_MEDIA_START:    //Notify Carlife music sound start
        MCARLIFE("CARLIFE_MEDIA_START");
        break;
    case  CARLIFE_MEDIA_STOP:     //Notify Carlife music sound start
        MCARLIFE("CARLIFE_MEDIA_STOP");
        break;
    case  CARLIFE_TTS_START:          //Notify Carlife navi sound start
        MCARLIFE("CARLIFE_TTS_START");
#ifdef __ARM_VOLUME_SDK__
        m_volume->setVolume(m_volume->getVolume()-20);
#endif
        break;
    case  CARLIFE_TTS_STOP:       //Notify Carlife navi sound stop
        MCARLIFE("CARLIFE_TTS_STOP");
#ifdef __ARM_VOLUME_SDK__
        m_volume->setVolume(m_volume->getVolume()+20);
#endif
        break;
    case  CARLIFE_ERROR_STATE:    //Notify Carlife in error state , need call carlife_stop() to reset the app state.
        m_pCarlife->sendMsgToCarlife(CARLIFE_STOP);
        MCARLIFE("CARLIFE_ERROR_STATE");
        onExitCarlifeSlot();
        break;
    case  CARLIFE_GOTO_DESKTOP://Carlife exit, please call Carlife_stop() to reset carlife app.
        MCARLIFE("CARLIFE_GOTO_DESKTOP");
        onExitCarlifeSlot();
        break;
    default:
        break;
    }
}
#endif

void MCarLifeDlg::onConnectCarlifeSlot()
{
#ifdef __ARM_CARLIFE_SDK__
    if(! m_pCarlife->isDevConnect()){
        MTextTipWidget::showTextTip(tr("Crarlife未连接"),  this);
        return;
    }

    m_pCarlife->sendMsgToCarlife(CARLIFE_START);
    m_volume->setNavigationVolume(m_volume->getVolume());
 #endif
//    m_pConnectCarlifeBtn->hide();
//    m_pExitCarlifeBtn->hide();
//    QWidget *pa = qobject_cast<QWidget *> (this->parent());
//    this->resize(WIDGET_WIDTH + CONTROL_BAL_WIDTH, WIDGET_HEIGHT);
//    pa->resize(WIDGET_WIDTH + CONTROL_BAL_WIDTH, WIDGET_HEIGHT);
//    pa->move(0,0);
}

void MCarLifeDlg::onExitCarlifeSlot()
{
#ifdef __ARM_CARLIFE_SDK__
    qDebug()<<__LINE__<<"_______"<<"m_pCarlife->isDevConnect()"<<m_pCarlife->isDevConnect();
//    if(m_pCarlife->isDevConnect())
//    {
//        m_pCarlife->sendMsgToCarlife(CARLIFE_STOP);
//    }
    m_pCarlife->sendMsgToCarlife(CARLIFE_STOP);
#endif
//    QWidget *pa = qobject_cast<QWidget *> (this->parent());
//    this->resize(WIDGET_WIDTH, WIDGET_HEIGHT);
//    pa->resize(WIDGET_WIDTH, WIDGET_HEIGHT);
//    pa->move(CONTROL_BAL_WIDTH,0);

//    emit NotifyOuterMsg("pop_back");
}

void MCarLifeDlg::showBtnView()
{
//    int w = this->width();
//    int h = this->height();
//    m_pConnectCarlifeBtn = new QPushButton(tr("Connect-\nCarLife"), this);
//    m_pConnectCarlifeBtn->setGeometry(w*4/6, h/8, w/6, w/6);
//    m_pConnectCarlifeBtn->setStyleSheet(
//                "QPushButton{ background-color:rgb(81,81,81); color:white;font-size:22px; "
//                "border-radius:15px;border:2px  groove  gray;} "
//                "QPushButton:pressed{background-color:white;color:black;}");
//    m_pConnectCarlifeBtn->setFlat(true);
//    m_pConnectCarlifeBtn->setFocusPolicy(Qt::NoFocus);
//    //m_pConnectCarlifeBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
//    connect(m_pConnectCarlifeBtn, SIGNAL(clicked()), this,SLOT(onConnectCarlifeSlot()));

//    m_pExitCarlifeBtn = new QPushButton(tr("Exit-\nCarLife"), this);
//    m_pExitCarlifeBtn->setGeometry(w/6, h/8, w/6, w/6);
//    m_pExitCarlifeBtn->setStyleSheet(
//                "QPushButton{ background-color:rgb(81,81,81); color:white;font-size:22px; "
//                "border-radius:15px;border:2px  groove  gray;} "
//                "QPushButton:pressed{background-color:white;color:black;}");
//    m_pExitCarlifeBtn->setFlat(true);
//    m_pExitCarlifeBtn->setFocusPolicy(Qt::NoFocus);
//    //m_pConnectCarlifeBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
//    connect(m_pExitCarlifeBtn, SIGNAL(clicked()), this, SLOT(onExitCarlifeSlot()));
//      m_pExitCarlifeBtn->hide();

}

#include <Carlife.h>

void MCarLifeDlg::mousePressEvent(QMouseEvent *e)
{
    e=e;
//    qDebug()<<"press is "<<e->pos();
    qDebug("press is (%d, %d).", e->pos().x(), e->pos().y());

    CarlifeTouch(e->pos().x(), e->pos().y(), 0);

#ifdef __ARM_CARLIFE_SDK__
    m_pCarlife->sendMousePos(e->pos().x()*1120/1600, e->pos().y(), TOUCH_PRESS);
#endif
}

void MCarLifeDlg::mouseReleaseEvent(QMouseEvent *e)
{
    e=e;
    qDebug("relese is (%d, %d).", e->pos().x(), e->pos().y());

    CarlifeTouch(e->pos().x(), e->pos().y(), 1);

//    qDebug() <<"release is "<<e->pos();
#ifdef __ARM_CARLIFE_SDK__
    m_pCarlife->sendMousePos(e->pos().x()*1120/1600, e->pos().y(), TOUCH_RELEASE);
#endif
}

void MCarLifeDlg::showEvent(QShowEvent *)
{
#ifdef __ARM_CAMERA_SDK__
    if(CameraCmd::Instance()->isCameraShow()){
        CameraCmd::Instance()->stop();
        CameraCmd::Instance()->setClicked(false);
    }

    m_pCarlife->sendMsgToCarlife(CARLIFE_START);
    m_volume->setNavigationVolume(m_volume->getVolume());
#endif

#ifdef __ARM_PLAYER_SDK__
    if(! PlayerManage::Instance()->bStop()){
        PlayerManage::Instance()->exit_player();
    }
#endif

#ifdef __ARM_CARLIFE_SDK__
    MCARLIFE("Carlife is connect: %d", m_pCarlife->isDevConnect());
#endif    

}

void MCarLifeDlg::hideEvent(QHideEvent *)
{
#ifdef __ARM_CARLIFE_SDK__
    if(m_pCarlife->isDevConnect())
    {
        m_pCarlife->sendMsgToCarlife(CARLIFE_STOP);
    }
#endif

#ifdef __ARM_CAMERA_SDK__
    CameraCmd::Instance()->displayImage(false);
    CameraCmd::Instance()->setClicked(true);
#endif
}



void MCarLifeDlg::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    paint.fillRect(this->rect(), Qt::black);
}

