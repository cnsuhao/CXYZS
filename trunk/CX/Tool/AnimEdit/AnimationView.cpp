#include "stdafx.h"
#include "AnimationView.h"
#include "AnimMng.h"
#include "2d/CCNode.h"
USING_NS_CC;

AnimDocker::AnimDocker(void)
{
    SetView(m_view);
    SetBarWidth(4);
    SetCaption(_T("动画"));
}

AnimDocker::~AnimDocker(void)
{

}


void AnimDocker::OnEditAnimNode(EditClass* node, int type)
{
    switch (type)
    {
    case FrameType::Position:
        {
            m_view.SetUserScale(32.0f);
            break;
        }
    case FrameType::Scale:
        {
            m_view.SetUserScale(1.0f/8.0f);
            break;
        }
    case FrameType::Rotation:
        {
            m_view.SetUserScale(16.0f);
            break;
        }
    case FrameType::Opacity:
        {
            m_view.SetUserScale(16.0f);
            break;
        }
    default:
        break;
    }

    // 进入
    ViewRedraw();
}

void AnimDocker::ViewRedraw()
{
     m_view.Invalidate(TRUE);
     m_view.ResetScrollPosition();
}

void AnimDocker::OnExitAnim()
{
    // 退出
    ViewRedraw();
    SetCaption(_T("动画"));
}

void AnimDocker::PlayAnim()
{
    gAnimMng.OnPlay();
}

AnmiView::AnmiView()
{
    initialied = false;
    indexX = 0;
    indexY = 0;
    fontHeight = 12;
    SetUserScale(1.0f);
}

AnmiView::~AnmiView()
{

}

void AnmiView::PreCreate(CREATESTRUCT& cs)
{
    cs.style |= WS_VSCROLL | WS_HSCROLL;
}

void AnmiView::OnInitialUpdate()
{
    font.CreateFont( fontHeight,        //   nHeight   
        0,                      //   nWidth   
        0,                      //   nEscapement 
        0,                      //   nOrientation   
        FW_NORMAL,              //   nWeight   
        FALSE,                  //   bItalic   
        FALSE,                  //   bUnderline   
        0,                      //   cStrikeOut   
        ANSI_CHARSET,           //   nCharSet   
        OUT_DEFAULT_PRECIS,     //   nOutPrecision   
        CLIP_DEFAULT_PRECIS,    //   nClipPrecision   
        DEFAULT_QUALITY,        //   nQuality   
        DEFAULT_PITCH   |   FF_SWISS,     //   nPitchAndFamily     
        _T("宋体"));

    initialied = true;

    SetScrollRange(SB_VERT, -99, 99, FALSE);
    SetScrollRange(SB_HORZ, 0, 200-1, FALSE);
    SetScrollPos(SB_VERT, 0, TRUE);
    SetScrollPos(SB_HORZ, 0, TRUE);

    pen.CreatePen(BS_SOLID, 1, RGB(192,192,192));
    redPen.CreatePen(BS_SOLID, 1, RGB(255,0,0));
    bluePan.CreatePen(BS_SOLID, 1, RGB(0,0,255));


    blueBursh.CreateSolidBrush(RGB(0,0,255));
    redBursh.CreateSolidBrush(RGB(255,0,0));
    Invalidate(TRUE);
}

void AnmiView::OnDraw(CDC* pDC)
{
    if (!gAnimMng.IsEditingNode())
    {
        return;
    }

    if (initialied)
    {
        pDC->SelectObject(&font);
        pDC->SelectObject(&pen);
        float sx, sy;
        char buff[32];
        int deltaY = int(AnmiView::Tranform::innerScaleX * 0.5f);
        int countY = int(AnmiView::Tranform::clinetH / deltaY);
        //////////////////////////////////////////////////////////////////////////
        Tranform::WorldToClient(0, 0, sx, sy,0,0,Tranform::offX,Tranform::offY,Tranform::clinetW,Tranform::clinetH,Tranform::innerScaleX, Tranform::innerScaleY);
        int value = sy;
        for (int i = 0; i < countY; ++i)
        {
            pDC->MoveTo(sx, value);
            pDC->LineTo(AnmiView::Tranform::clinetW,value);

            value -= deltaY;
        }
        int deltaX = int(AnmiView::Tranform::innerScaleX);
        int countX = int(AnmiView::Tranform::clinetW / deltaX);
        value = sx;
        for (int i = 0; i < countX; i++)
        {
            pDC->MoveTo(value,sy);
            pDC->LineTo(value,0);

            value += deltaX;   
        }
        //////////////////////////////////////////////////////////////////////////
        Tranform::WorldToClient(0, 0, sx, sy,0,0,0,0,Tranform::clinetW,Tranform::clinetH,Tranform::innerScaleX, Tranform::innerScaleY);
        CRect rc;
        rc.left = 0;
        rc.right = rc.left + AnmiView::Tranform::innerScaleX * AnmiView::Tranform::offX;
        value = sy - 0.5f * AnmiView::Tranform::innerScaleY;
        for (int i = 0; i < countY; ++i)
        {
            rc.bottom = value;
            rc.top = rc.bottom - AnmiView::Tranform::innerScaleY;

            if (i != 0)
            {
                if (userScale < 1.0f)
                {
                    sprintf(buff,"%.3f", float(indexY + i) * userScale);
                }
                else
                {
                    sprintf(buff,"%d", int((indexY + i) * userScale));
                }
                pDC->DrawText(buff,-1, rc, DT_SINGLELINE | DT_RIGHT);
            }
            value -= deltaY;
        }
        rc.bottom = sy + 4;
        rc.top = rc.bottom - fontHeight;
        value = sx + AnmiView::Tranform::innerScaleX * AnmiView::Tranform::offX;
        for (int i = 0; i < countX; i++)
        {
            rc.left = value;
            rc.right = rc.left + AnmiView::Tranform::innerScaleX;
            sprintf(buff,"%d", i + indexX);
            pDC->DrawText(buff,-1, rc, DT_SINGLELINE);
            value += AnmiView::Tranform::innerScaleX;   
        }

        //////////////////////////////////////////////////////////////////////////
        AnimFrame* pFrame = gAnimMng.Find(gAnimMng.GetEditingNode()->GetNode()->getName());
        int type = gAnimMng.GetEditingType();
        switch (type)
        {
        case FrameType::Position:
            DrawTranslate(pFrame, pDC);
            break;
        case FrameType::Scale:
            DrawScale(pFrame, pDC);
            break;
        case FrameType::Rotation:
            DrawRotate(pFrame, pDC);
            break;
        case FrameType::Opacity:
            DrawOpacity(pFrame, pDC);
            break;
        default:
            break;
        }
    }
}

void AnmiView::ResetScrollPosition()
{
    indexY = 0;
    indexX = 0;
    SetScrollPos(SB_VERT, 0, TRUE);
    SetScrollPos(SB_HORZ, 0, TRUE);
    Tranform::viewX = 0;
    Tranform::viewY = 0;
}

LRESULT AnmiView::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    //char buff[32];
    //sprintf(buff,"%s(%x)\n", TranslateWMsg(uMsg), uMsg);
    //TRACE(buff);

    switch (uMsg)
    {
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
        if (gAnimMng.IsEditingNode())
        {
            SetFocus();
            return TRUE;
        }break;
    case WM_SIZE:
        {
            Tranform::clinetW = LOWORD(lParam); // width of client area 
            Tranform::clinetH = HIWORD(lParam); // height of client area
            Invalidate(TRUE);
        }break;
    case WM_VSCROLL:
        {
            switch(LOWORD(wParam))
            {
            case SB_LINEUP:     // 上翻一行
                TRACE("上翻一行\n");
                if (indexY < 99)
                {
                    ++indexY;
                    SetScrollPos(SB_VERT, -indexY, TRUE);
                    Invalidate(TRUE);
                    AnmiView::Tranform::viewY += 1.0f;
                }
                
                break;
            case SB_LINEDOWN:   // 下翻一行
                TRACE("上翻一行\n");
                if (indexY > -99)
                {
                    --indexY;
                    SetScrollPos(SB_VERT, -indexY, TRUE);
                    Invalidate(TRUE);
                    AnmiView::Tranform::viewY -= 1.0f;
                }
                break;
            case SB_PAGEUP:     // 向上翻一整页
                TRACE("向上翻一整页\n");
                break;
            case SB_PAGEDOWN:   // 向下翻一整页
                TRACE("向下翻一整页\n");
                break;
            case SB_THUMBPOSITION:  // 拖动滑块滑块被放下时
                TRACE("拖动滑块滑块被放下时\n");
                indexY = -(short)HIWORD(wParam);
                AnmiView::Tranform::viewY = indexY;
                SetScrollPos(SB_VERT, -indexY, TRUE);
                Invalidate(TRUE);
                break;
            default:
                break;
            } 
        }break;
    case WM_HSCROLL:
        {
            switch(LOWORD(wParam))
            {
            case SB_LINEUP:     // 左翻一行
                TRACE("左翻一行\n");
                if (indexX > 0)
                {
                    --indexX;
                    SetScrollPos(SB_HORZ, indexX, TRUE);
                    Invalidate(TRUE);
                    AnmiView::Tranform::viewX -= 1.0f;
                }break;
            case SB_LINEDOWN:   // 右翻一行
                TRACE("右翻一行\n");
                if (indexX < 200)
                {
                    ++indexX;
                    SetScrollPos(SB_HORZ, indexX, TRUE);
                    Invalidate(TRUE);
                    AnmiView::Tranform::viewX += 1.0f;
                }
                break;
            case SB_PAGEUP:     // 左翻一页
                TRACE("左翻一页\n");
                break;
            case SB_PAGEDOWN:   // 右翻一页
                TRACE("右翻一页\n");
                break;
            case SB_THUMBPOSITION:  // 拖动滑块滑块被放下时
                TRACE("拖动滑块滑块被放下时\n");
                indexX = -(short)HIWORD(wParam);
                AnmiView::Tranform::viewX = indexX;
                SetScrollPos(SB_HORZ, indexX, TRUE);
                Invalidate(TRUE);
                break;
            default:
                break;
            }
        }break;
    case WM_RBUTTONUP :
        {
            if (gAnimMng.IsEditingNode())
            {
                int type = gAnimMng.GetEditingType();
                CPoint curPos = GetCursorPos();
                CPoint wp = curPos;
                ScreenToClient(wp);
                float wx, wy;
                Tranform::ClientToWorld(float(wp.x),float(wp.y), wx, wy);
                int frameId = int(wx + 0.5f);

                CMenu TopMenu(IDW_MENU_NODE);
                CMenu* pSubMenu = TopMenu.GetSubMenu(1);
                if (gAnimMng.HasCurrentFrameData(type, frameId))
                {
                    pSubMenu->EnableMenuItem(IDM_DEL_FRAME,FALSE);
                    pSubMenu->EnableMenuItem(IDM_ADD_FRAME,TRUE);
                }else
                {
                    if (frameId > 0)
                    {
                        pSubMenu->EnableMenuItem(IDM_DEL_FRAME,TRUE);
                        pSubMenu->EnableMenuItem(IDM_ADD_FRAME,FALSE);
                    }
                    else
                    {
                        pSubMenu->EnableMenuItem(IDM_DEL_FRAME,TRUE);
                        pSubMenu->EnableMenuItem(IDM_ADD_FRAME,TRUE);
                    }
                }
                SetFocus();
                UINT uSelected = pSubMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, curPos.x, curPos.y, this, NULL);
                unsigned char interpolation = GetMyApp()->m_Frame.GetInterpolation();
                Node* node = gAnimMng.GetEditingNode()->GetNode();
                switch (uSelected)
                {
                case IDM_ADD_FRAME : 
                    {
                        switch (type)
                        {
                        case FrameType::Position:
                            gAnimMng.SetCurrentFrameData(type, interpolation, frameId, node->getPositionX(), node->getPositionY());
                            break;
                        case FrameType::Scale:
                            gAnimMng.SetCurrentFrameData(type, interpolation, frameId, node->getScaleX(), node->getScaleY());
                            break;
                        case FrameType::Rotation:
                            gAnimMng.SetCurrentFrameData(type, interpolation, frameId, node->getRotation());
                            break;
                        case FrameType::Opacity:
                            gAnimMng.SetCurrentFrameData(type, interpolation, frameId, float(node->getOpacity()));
                        default:
                            break;
                        }
                        
                        Invalidate(TRUE);
                    }break;
                case IDM_DEL_FRAME:
                    { 
                        gAnimMng.DelCurrentFrameData(type, frameId);
                        Invalidate(TRUE);
                    }break; 
                default:
                    break;
                }
                return TRUE;
            }
        }break;
    case WM_MOUSEWHEEL:
        {
            short wheel = (short)HIWORD(wParam) / WHEEL_DELTA;
            if (wheel > 0){
                SetUserScale(GetUserScale() * 2.0f);
                Invalidate(TRUE);
            }
            else{
                SetUserScale(GetUserScale() / 2.0f);
                Invalidate(TRUE);
            }
        }
        break;
    default:
        break;
    }
    return WndProcDefault(uMsg, wParam, lParam);
}

void AnmiView::SetUserScale(float scale)
{
    userScale = scale;
    userInvScale = 1.0/userScale;
}

void AnmiView::DrawTranslate(AnimFrame* pFrame, CDC* pDC)
{
    float sx; float sy;
    Tranform::WorldToClient(0, 0, sx, sy,0,0,0,0,Tranform::clinetW,Tranform::clinetH,Tranform::innerScaleX, Tranform::innerScaleY);
    PositionData* prevData = NULL;
    PositionData* data = NULL;
    CRect rt;
    int length = pFrame->m_position_array.size();
    if(length > 1)
    {
        pDC->SelectObject(&bluePan);
        std::list<PositionData*>::iterator it = pFrame->m_position_array.begin();
        ++it;
        for ( ; it != pFrame->m_position_array.end(); ++it)
        {
            pDC->SelectObject(&bluePan);
            std::list<PositionData*>::iterator frontIt = it; --frontIt;
            prevData = (*frontIt);
            data = *it;
            Tranform::WorldToClient(float(prevData->frameId),prevData->position.x*userInvScale, sx, sy);
            pDC->MoveTo(sx, sy);
            Tranform::WorldToClient(float(data->frameId),data->position.x*userInvScale, sx, sy);
            pDC->LineTo(sx, sy);
            rt.left = sx - 2;
            rt.right = sx + 2;
            rt.top = sy - 2;
            rt.bottom = sy + 2;
            pDC->FillRect(rt, &blueBursh);

            pDC->SelectObject(&redPen);
            Tranform::WorldToClient(float(prevData->frameId),prevData->position.y*userInvScale, sx, sy);
            pDC->MoveTo(sx, sy);
            Tranform::WorldToClient(float(data->frameId),data->position.y*userInvScale, sx, sy);
            pDC->LineTo(sx, sy);
            rt.left = sx - 2;
            rt.right = sx + 2;
            rt.top = sy - 2;
            rt.bottom = sy + 2;
            pDC->FillRect(rt, &redBursh);
        }
    }
}

void AnmiView::DrawScale(AnimFrame* pFrame, CDC* pDC)
{
    float sx; float sy;
    Tranform::WorldToClient(0, 0, sx, sy,0,0,0,0,Tranform::clinetW,Tranform::clinetH,Tranform::innerScaleX, Tranform::innerScaleY);
    ScaleData* prevData = NULL;
    ScaleData* data = NULL;
    CRect rt;
    int length = pFrame->m_scale_array.size();
    if(length > 1)
    {
        pDC->SelectObject(&bluePan);
        std::list<ScaleData*>::iterator it = pFrame->m_scale_array.begin();
        ++it;
        for ( ; it != pFrame->m_scale_array.end(); ++it)
        {
            pDC->SelectObject(&bluePan);
            std::list<ScaleData*>::iterator frontIt = it; --frontIt;
            prevData = (*frontIt);
            data = *it;
            Tranform::WorldToClient(float(prevData->frameId),prevData->scale.x*userInvScale, sx, sy);
            pDC->MoveTo(sx, sy);
            Tranform::WorldToClient(float(data->frameId),data->scale.x*userInvScale, sx, sy);
            pDC->LineTo(sx, sy);
            rt.left = sx - 2;
            rt.right = sx + 2;
            rt.top = sy - 2;
            rt.bottom = sy + 2;
            pDC->FillRect(rt, &blueBursh);

            pDC->SelectObject(&redPen);
            Tranform::WorldToClient(float(prevData->frameId),prevData->scale.y*userInvScale, sx, sy);
            pDC->MoveTo(sx, sy);
            Tranform::WorldToClient(float(data->frameId),data->scale.y*userInvScale, sx, sy);
            pDC->LineTo(sx, sy);
            rt.left = sx - 2;
            rt.right = sx + 2;
            rt.top = sy - 2;
            rt.bottom = sy + 2;
            pDC->FillRect(rt, &redBursh);
        }
    }
}

void AnmiView::DrawRotate(AnimFrame* pFrame, CDC* pDC)
{
    float sx; float sy;
    Tranform::WorldToClient(0, 0, sx, sy,0,0,0,0,Tranform::clinetW,Tranform::clinetH,Tranform::innerScaleX, Tranform::innerScaleY);
    RotateData* prevData = NULL;
    RotateData* data = NULL;
    CRect rt;
    int length = pFrame->m_rotation_array.size();
    if(length > 1)
    {
        pDC->SelectObject(&bluePan);
        std::list<RotateData*>::iterator it = pFrame->m_rotation_array.begin();
        ++it;
        for ( ; it != pFrame->m_rotation_array.end(); ++it)
        {
            pDC->SelectObject(&bluePan);
            std::list<RotateData*>::iterator frontIt = it; --frontIt;
            prevData = (*frontIt);
            data = *it;
            Tranform::WorldToClient(float(prevData->frameId),prevData->dstAngle*userInvScale, sx, sy);
            pDC->MoveTo(sx, sy);
            Tranform::WorldToClient(float(data->frameId),data->dstAngle*userInvScale, sx, sy);
            pDC->LineTo(sx, sy);
            rt.left = sx - 2;
            rt.right = sx + 2;
            rt.top = sy - 2;
            rt.bottom = sy + 2;
            pDC->FillRect(rt, &blueBursh);
        }
    }
}

void AnmiView::DrawOpacity(AnimFrame* pFrame, CDC* pDC)
{
    float sx; float sy;
    Tranform::WorldToClient(0, 0, sx, sy,0,0,0,0,Tranform::clinetW,Tranform::clinetH,Tranform::innerScaleX, Tranform::innerScaleY);
    OpacityData* prevData = NULL;
    OpacityData* data = NULL;
    CRect rt;
    int length = pFrame->m_opacity_array.size();
    if(length > 1)
    {
        pDC->SelectObject(&bluePan);
        std::list<OpacityData*>::iterator it = pFrame->m_opacity_array.begin();
        ++it;
        for ( ; it != pFrame->m_opacity_array.end(); ++it)
        {
            pDC->SelectObject(&bluePan);
            std::list<OpacityData*>::iterator frontIt = it; --frontIt;
            prevData = (*frontIt);
            data = *it;
            Tranform::WorldToClient(float(prevData->frameId),prevData->opacity*userInvScale, sx, sy);
            pDC->MoveTo(sx, sy);
            Tranform::WorldToClient(float(data->frameId),data->opacity*userInvScale, sx, sy);
            pDC->LineTo(sx, sy);
            rt.left = sx - 2;
            rt.right = sx + 2;
            rt.top = sy - 2;
            rt.bottom = sy + 2;
            pDC->FillRect(rt, &blueBursh);
        }
    }
}

void AnmiView::Tranform::WorldToView(float wx, float wy, float& vx, float& vy, float userViewX, float userViewY, float userOffX, float userOffY)
{
    vx = wx - userViewX + userOffX;
    vy = wy - userViewY + userOffY;
}

void AnmiView::Tranform::ViewToClient(float vx, float vy, float& cx, float& cy, float userClinetW, float userClinetH, float userInnerScaleX, float userInnerScaleY)
{
    cx = vx * userInnerScaleX;
    cy = userClinetH - vy * userInnerScaleY - 3;
}

void AnmiView::Tranform::ClientToView(float cx, float cy, float& vx, float& vy, float userClinetW, float userClinetH, float userInnerScaleX, float userInnerScaleY)
{
    vx = cx / userInnerScaleX;
    vy = (userClinetH - cy  - 3) / userInnerScaleY;
}

void AnmiView::Tranform::ViewToWorld(float vx, float vy, float& wx, float& wy, float userViewX, float userViewY, float userOffX, float userOffY)
{
    wx = vx + userViewX - userOffX;
    wy = vy + userViewY - userOffY;
}

float AnmiView::Tranform::viewX = 0;
float AnmiView::Tranform::viewY = 0;
float AnmiView::Tranform::offX = 2;
float AnmiView::Tranform::offY = 1;
float AnmiView::Tranform::clinetW = 0;
float AnmiView::Tranform::clinetH = 0;
float AnmiView::Tranform::innerScaleX = 20.0f;
float AnmiView::Tranform::innerScaleY = 10.0f;