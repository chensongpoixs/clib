/********************************************************************
	created:	2015/1/8

	author:		chensong
	
	purpose:	不可拷贝对象基类 

*********************************************************************/
#ifndef _C_NONCOPYABLE_H_
#define _C_NONCOPYABLE_H_

namespace chen
{
    class cnoncopyable
    {
    public:
        cnoncopyable() {}
        ~cnoncopyable() {}
    private:
        cnoncopyable(const cnoncopyable&);
        cnoncopyable& operator=(const cnoncopyable&);
    };
} // namespace chen
#endif // _C_NONCOPYABLE_H_

