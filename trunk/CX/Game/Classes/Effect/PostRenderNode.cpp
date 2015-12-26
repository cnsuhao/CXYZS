#include "PostRenderNode.h"

USING_NS_CC;

#include "CCGrid.h"
#include "CCGLProgram.h"
#include "CCGLStateCache.h"

class Grid3DEx : public Grid3D
{
    Grid3DEx():_state(NULL)
    {

    }
public:

    static Grid3DEx* create(const Size& gridSize)
    {
        Grid3DEx *ret= new (std::nothrow) Grid3DEx();

        if (ret)
        {
            if (ret->initWithSize(gridSize))
            {
                ret->autorelease();
            }
            else
            {
                delete ret;
                ret = nullptr;
            }
        }

        return ret;
    }

     virtual void blit() override
     {
         int n = _gridSize.width * _gridSize.height;

         GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD );
         
         if (_state)
         {
             Director* director = Director::getInstance();
             CCASSERT(nullptr != director, "Director is null when seting matrix stack");

             Mat4 matrixMV;
             matrixMV = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

             _state->apply(matrixMV);
         }
         else
         {
             _shaderProgram->use();
             _shaderProgram->setUniformsForBuiltins();
         }

         //
         // Attributes
         //

         // position
         glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, _vertices);

         // texCoords
         glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, _texCoordinates);

         glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, _indices);
     }

     void SetGLProgramState(cc2d::GLProgramState* state) { _state = state; }
protected:
    cc2d::GLProgramState* _state;
};




PostRenderNode::PostRenderNode(void):_filter(NULL)
{
    _quad = Grid3DEx::create(Size(1,1));
    CCASSERT(_quad, "");
    setGrid(_quad);
}


PostRenderNode::~PostRenderNode(void)
{
}

PostRenderNode* PostRenderNode::create()
{
    PostRenderNode *pNode = new (std::nothrow) PostRenderNode();
    if (pNode)
    {
        if (pNode->init())
        {
            pNode->autorelease();
        }
        else
        {
            CC_SAFE_RELEASE_NULL(pNode);
        }
    }

    return pNode;
}

void PostRenderNode::UseFilter(FilterBase* filter)
{
    if (_filter == filter)
        return;

    if (_filter){
        _filter->release();
    }

    _filter = filter;
    if (_filter)
    {
        _filter->retain();
        _quad->setActive(true);
        if (_quad)
        {
            static_cast<Grid3DEx*>(_quad)->SetGLProgramState(_filter->GetGLProgramState());
        }
    }
    else
    {
        _quad->setActive(false);
    }
}

void PostRenderNode::SetActive(bool active)
{
    if (_quad)
    {
        _quad->setActive(active);
    }
}
