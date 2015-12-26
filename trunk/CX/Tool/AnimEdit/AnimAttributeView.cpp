#include "stdafx.h"
#include "AnimAttributeView.h"
#include "AnimMng.h"
#include "SelectHelper.h"
#include "2d/CCNode.h"

USING_NS_CC;

AnimAttributeView::AnimAttributeView(UINT nResID):CDialog(nResID)
{

}

AnimAttributeView::~AnimAttributeView(void)
{
}

HWND AnimAttributeView::Create(CWnd* pParent)
{
    return DoModeless(pParent);
}

BOOL AnimAttributeView::OnInitDialog()
{
    AttachItem(IDC_ANIM_TEXT1, m_text1);
    AttachItem(IDC_ANIM_TEXT2, m_text2);
    AttachItem(IDC_ANIM_TEXT3, m_text3);

    AttachItem(IDC_ANIM_EDIT1, m_editText1);
    AttachItem(IDC_ANIM_EDIT2, m_editText2);
    AttachItem(IDC_ANIM_EDIT3, m_editText3);

    AttachItem(IDC_EXIT_ANIM, m_exit);

    AttachItem(IDC_LIST1, m_interpolations);

    m_interpolations.AddString("Linear");
    m_interpolations.AddString("Sine_EaseIn");
    m_interpolations.AddString("Sine_EaseOut");
    m_interpolations.AddString("Sine_EaseInOut");
    m_interpolations.AddString("Quad_EaseIn");
    m_interpolations.AddString("Quad_EaseOut");
    m_interpolations.AddString("Quad_EaseInOut");
    m_interpolations.AddString("Cubic_EaseIn");
    m_interpolations.AddString("Cubic_EaseOut");
    m_interpolations.AddString("Cubic_EaseInOut");
    m_interpolations.AddString("Quart_EaseIn");
    m_interpolations.AddString("Quart_EaseOut");
    m_interpolations.AddString("Quart_EaseInOut");
    m_interpolations.AddString("Quint_EaseIn");
    m_interpolations.AddString("Quint_EaseOut");
    m_interpolations.AddString("Quint_EaseInOut");
    m_interpolations.AddString("Expo_EaseIn");
    m_interpolations.AddString("Expo_EaseOut");
    m_interpolations.AddString("Expo_EaseInOut");
    m_interpolations.AddString("Circ_EaseIn");
    m_interpolations.AddString("Circ_EaseOut");
    m_interpolations.AddString("Circ_EaseInOut");
    m_interpolations.AddString("Elastic_EaseIn");
    m_interpolations.AddString("Elastic_EaseOut");
    m_interpolations.AddString("Elastic_EaseInOut");
    m_interpolations.AddString("Back_EaseIn");
    m_interpolations.AddString("Back_EaseOut");
    m_interpolations.AddString("Back_EaseInOut");
    m_interpolations.AddString("Bounce_EaseIn");
    m_interpolations.AddString("Bounce_EaseOut");
    m_interpolations.AddString("Bounce_EaseInOut");

    m_interpolations.SetCurSel(0);
    return TRUE;
}

BOOL AnimAttributeView::OnCommand(WPARAM wParam, LPARAM lParam)
{
    int rID = LOWORD(wParam);
    int msg = HIWORD(wParam);
    if (msg == BN_CLICKED)
    {
        if (rID == IDC_EXIT_ANIM)
        {
            gAnimMng.ExitEidtNode();
            return TRUE;
        }
        else if (rID == IDC_PLAY_ANIM)
        {
            GetMyApp()->m_Frame.PlayAnim();
            return TRUE;
        }
    }
    else if (LBN_SELCHANGE)
    {
        //if (rID == IDC_LIST1)
        //{
        //    gAnimMng.SetCurrentFrameData(gAnimMng.GetEditingType(), GetInterpolationIndex(), gAnimMng.GetLastEditingFrameId());
        //}
    }

    return CDialog::OnCommand(wParam, lParam);
}

INT_PTR AnimAttributeView::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DialogProcDefault(uMsg, wParam, lParam);
}

void AnimAttributeView::OnOK()
{
    float t;
    CWnd* focus = GetFocus();
    int type = gAnimMng.GetEditingType();
    EditClass* eNode = gAnimMng.GetEditingNode();
    Node* node = eNode->GetNode();
    switch (focus->GetDlgCtrlID())
    {
    case IDC_ANIM_EDIT1:
        {
            CString& text = GetDlgItemText(IDC_ANIM_EDIT1);
            switch (type)
            {
            case FrameType::Position:
                {
                    bool getSucc = (sscanf(text.c_str(),"%f", &t) == 1);
                    if (getSucc)
                    {
                        node->setPositionX(t);
                        gSelectHepler.NodeMoveUpdate();
                    }else{
                        sprintf(gSprBuff, "%f", node->getPositionX());
                        SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);
                    }
                    break;
                }
            case FrameType::Scale:
                {
                    bool getSucc = (sscanf(text.c_str(),"%f", &t) == 1);
                    if (getSucc)
                    {
                        node->setScaleX(t);
                    }else{
                        sprintf(gSprBuff, "%f", node->getScaleX());
                        SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);
                    }
                    break;
                }
            case FrameType::Rotation:
                {
                    bool getSucc = (sscanf(text.c_str(),"%f", &t) == 1);
                    if (getSucc)
                    {
                        node->setRotation(t);
                    }else{
                        sprintf(gSprBuff, "%f", node->getRotation());
                        SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);
                    }
                    break;
                }
            case FrameType::Opacity:
                {
                    bool getSucc = (sscanf(text.c_str(),"%f", &t) == 1);
                    if (getSucc)
                    {
                        node->setOpacity(GLbyte(t));
                    }else{
                        sprintf(gSprBuff, "%f", node->getOpacity());
                        SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);
                    }
                    break;
                }
            default:
                break;
            }
            break;
        }
    case IDC_ANIM_EDIT2:
        {
            float t;
            CString& text = GetDlgItemText(IDC_ANIM_EDIT2);
            bool getSucc = (sscanf(text.c_str(),"%f", &t) == 1);

            switch (type)
            {
            case FrameType::Position:
                {
                    if (getSucc)
                    {
                        node->setPositionY(t);
                        gSelectHepler.NodeMoveUpdate();
                    }else{
                        sprintf(gSprBuff, "%f", node->getPositionY());
                        SetDlgItemText(IDC_ANIM_EDIT2, gSprBuff);
                    }
                    break;
                }
            case FrameType::Scale:
                {
                    if (getSucc)
                    {
                        node->setScaleY(t);
                    }else{
                        sprintf(gSprBuff, "%f", node->getScaleY());
                        SetDlgItemText(IDC_ANIM_EDIT2, gSprBuff);
                    }
                    break;
                }
            default:
                break;
            }
            break;
        }
    case IDC_ANIM_EDIT3:
        {
            break;
        }
    }
}

void AnimAttributeView::OnNodeSelect(EditClass* eNode, int type)
{
    Node* node = eNode->GetNode();
    if (node)
    {
        switch (type)
        {
        case FrameType::Position :
            {
                SetDlgItemText(IDC_ANIM_TEXT1, "位置X:");
                SetDlgItemText(IDC_ANIM_TEXT2, "位置Y:");
                SetDlgItemText(IDC_ANIM_TEXT3, "");
                
                sprintf(gSprBuff, "%f", node->getPositionX());
                SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);

                sprintf(gSprBuff, "%f", node->getPositionY());
                SetDlgItemText(IDC_ANIM_EDIT2, gSprBuff);

                SetDlgItemText(IDC_ANIM_EDIT3, "");
                break;
            }
        case FrameType::Scale :
            {
                SetDlgItemText(IDC_ANIM_TEXT1, "缩放X:");
                SetDlgItemText(IDC_ANIM_TEXT2, "缩放Y:");
                SetDlgItemText(IDC_ANIM_TEXT3, "");

                sprintf(gSprBuff, "%f", node->getScaleX());
                SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);

                sprintf(gSprBuff, "%f", node->getScaleY());
                SetDlgItemText(IDC_ANIM_EDIT2, gSprBuff);

                SetDlgItemText(IDC_ANIM_EDIT3, "");
                break;
            }
        case FrameType::Rotation :
            {
                SetDlgItemText(IDC_ANIM_TEXT1, "旋转值:");
                SetDlgItemText(IDC_ANIM_TEXT2, "");
                SetDlgItemText(IDC_ANIM_TEXT3, "");

                sprintf(gSprBuff, "%f", node->getRotation());
                SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);

                SetDlgItemText(IDC_ANIM_EDIT2, "");

                SetDlgItemText(IDC_ANIM_EDIT3, "");
                break;
            }
        case FrameType::Opacity :
            {
                SetDlgItemText(IDC_ANIM_TEXT1, "透明度:");
                SetDlgItemText(IDC_ANIM_TEXT2, "");
                SetDlgItemText(IDC_ANIM_TEXT3, "");

                float opacity = node->getOpacity();
                sprintf(gSprBuff, "%f", opacity);
                SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);

                SetDlgItemText(IDC_ANIM_EDIT2, "");

                SetDlgItemText(IDC_ANIM_EDIT3, "");
                break;
            }
        default:
            break;
        }
    }
}

void AnimAttributeView::OnMoveEnd(EditClass* eNode)
{
    Node* node = eNode->GetNode();
    if (node && gAnimMng.GetEditingType() == (int)FrameType::Position)
    {
        sprintf(gSprBuff, "%f", node->getPositionX());
        SetDlgItemText(IDC_ANIM_EDIT1, gSprBuff);

        sprintf(gSprBuff, "%f", node->getPositionY());
        SetDlgItemText(IDC_ANIM_EDIT2, gSprBuff);
    }
}

unsigned char AnimAttributeView::GetInterpolationIndex()
{
    return m_interpolations.GetCurSel();
}
