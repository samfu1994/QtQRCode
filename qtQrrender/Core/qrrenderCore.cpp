#include "qrrenderCore.h"

#include <Windows.h>

HANDLE render::run(void){
#if __MULTI_PROCESS__
	HANDLE h;
	h = CreateThread(NULL,
		0,
		render::ThreadFunc,
		this,
		0,
		NULL);
	return h;
#else
	ThreadFunc(this);
	return NULL;
#endif /* __MULTI_PROCESS__ */
}

DWORD WINAPI render::ThreadFunc(LPVOID param){
	render *p = (render *)param;
	int ret = 0;
	ret = p->preprocess();
	if (ret){
		return ret;
	}
	ret = p->process();
	if (ret){
		return ret;
	}
	ret = p->postprocess();
	if (ret){
		return ret;
	}
#if __USING_QT__
    emit p->processfinish();
#endif
	return 0;
}

int render::process(){
    return 0;
}

int render::preprocess(){
    return 0;
}

int render::postprocess(){
    return 0;
}


