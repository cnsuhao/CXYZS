#include "stdafx.h"
#include "SpriteAttributeView.h"
#include "SelectHelper.h"
#include "2d/CCNode.h"
#include "2d/CCSprite.h"
#include "2d/CCSpriteFrameCache.h"
#include "platform/CCFileUtils.h"
#include "2d/CCAnimation.h"
#include "2d/CCActionInterval.h"
using namespace std;

USING_NS_CC;

//Unicode编码转为多字节编码
bool UnicodeToMB(std::vector<char>& pmb, const wchar_t* pun, int uLen)
{
    // convert an widechar string to Multibyte 
    int MBLen = WideCharToMultiByte(CP_ACP, 0, pun, uLen, NULL, 0, NULL, NULL);
    if (MBLen <=0)
    {
        return false;
    }
    pmb.resize(MBLen);
    int nRtn = WideCharToMultiByte(CP_ACP, 0, pun, uLen, &*pmb.begin(), MBLen, NULL, NULL);

    if(nRtn != MBLen)
    {
        pmb.clear();
        return false;
    }
    return true;
}

SpriteAttributeView::SpriteAttributeView(UINT nResID):CDialog(nResID)
{

}

SpriteAttributeView::~SpriteAttributeView()
{

}

HWND SpriteAttributeView::Create(CWnd* pParent)
{
    return DoModeless(pParent);
}

BOOL SpriteAttributeView::OnInitDialog()
{
    AttachItem(IDW_OPEN_SPRITE,	 selBtn);
    AttachItem(IDW_PATH_SPRITE, pathEdit);

    return TRUE;
}

BOOL SpriteAttributeView::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (LOWORD(wParam))
    {
    case IDW_OPEN_SPRITE:	OnSeletFileBtn();		return TRUE;
    }
    return FALSE;
}

INT_PTR SpriteAttributeView::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{



    return DialogProcDefault(uMsg, wParam, lParam);
}

void SpriteAttributeView::OnSeletFileBtn()
{
    if (gSelectHepler.GetSelectNode() == NULL)
    {
        return;
    }

    CFile File;
    CString str = File.OpenFileDialog(0, OFN_FILEMUSTEXIST, _T("Open File"), 
        _T("png files(*.png)\0*.png\0jpeg files(*.jpg;*jpeg)\0*.jpg;*jpeg\0plist files(*.plist)\0*.plist\0\0"), this);

    if (!str.IsEmpty())
    {
        // Retrieve the PlotPoint data
        if (FileOpen(str))
        {
            // Save the filename
            ///m_PathName = str;
            //AddMRUEntry(str);
        }
        else{
            //m_PathName=_T("");
        }
    }

}

void SpriteAttributeView::OnNodeSelect(cc2d::Node* node)
{

}

BOOL SpriteAttributeView::FileOpen(LPCTSTR szFilename)
{
    //std::vector<char> fileNameVec;
    //UnicodeToMB(fileNameVec, szFilename, wcslen(szFilename));
    //string fileName;
    //fileName.append(fileNameVec.begin(), fileNameVec.end());
    string fileName(szFilename);
     int type = 0;
    int p = fileName.rfind('.');
    if (p != string::npos)
    {
        string subStr(fileName.begin() + p, fileName.end());
        if (subStr == ".jpg" || subStr == ".jpeg" || subStr == ".png")
        {
            type = 0;
        }
        else if (subStr == ".plist")
        {
            type = 1;
        }
        else
        {
            return FALSE;
        }
    }
   
    EditSprite* eNode =static_cast<EditSprite*>(gSelectHepler.GetSelectNode());
    Sprite* sprite = eNode->sprite;
    sprite->stopAllActions();
    switch (type)
    {
        case 0:
            {   
                sprite->setTexture(fileName);
            }break;
        case 1:
            {
                int p = fileName.rfind('.');
                string fileImage(fileName.begin(), fileName.begin() + p);
                fileImage.append(".png");
                int p2 = fileName.rfind('\\');
                string keyName(fileName.begin()+p2+1, fileName.begin() + p);
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fileName, fileImage);
                ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fileName);
                int FrameCount = dict["frames"].asValueMap().size();
                Vector<SpriteFrame*> animFrames(FrameCount);
                char str[64] = {0};
                for (int i = 0; i < FrameCount; i++)
                {
                    sprintf(str, "%s_%d.png", keyName.c_str(), i);
                    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
                    animFrames.pushBack(frame);		
                }
                Animation* animation = Animation::createWithSpriteFrames(animFrames, 1.0f/FrameCount);
                sprite->runAction(RepeatForever::create(Animate::create(animation)));
            }break;
        default:
            return FALSE;
    }

    return TRUE;
}
