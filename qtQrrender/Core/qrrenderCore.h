#ifndef ___RERENDER_CORE___
#define ___RERENDER_CORE___

#define __MULTI_PROCESS__   0
#define __USING_QT__        1
// this is the framework for this project

#include <Windows.h>

#if __USING_QT__
#include <QObject>
#endif


/**
 *			RETURN VAL
 *	0:		process successfully executed
 *	other:	error message
 */


/**
 *						RENDER CLASS
 *	The class render is the base class for all processes including
 *	qrCode generator, halftone image generator, and combine process.
 *	Function run should not be modefied in child classes, this 
 *	finction provide standard phase for the process. Only pre/post/
 *	process should be implemented in child class.
 *					
 *						USAGE FOR THIS CORE
 *	Child class should implement pre/post/process, and inherent run
 *	function. Using feed function to feed data in to this processor
 *	then use getData to fetch the output Data.
 *	This kind of design is convenient for transfering to multiprocess
 *	program. To implement that feature, a set of new functions are
 *	needed
 */

class render
#if __USING_QT__
        : public QObject
#endif
{
#if __USING_QT__
Q_OBJECT
#endif
private:
	// call before process
    virtual int preprocess();
	// call after process
    virtual int postprocess();
	// process data
    virtual int process();
    // thread function
    static DWORD WINAPI ThreadFunc(LPVOID param);

public:
    // generate new thread(optional)
    // manage all tmp data,create run time enviroment
    HANDLE run();

#if __USING_QT__
signals:
    void processfinish();
#endif
};


#endif /* ___RERENDER_CORE___ */
