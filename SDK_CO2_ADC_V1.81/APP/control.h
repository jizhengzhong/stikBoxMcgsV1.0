
#ifndef CONTROL_H_H
#define	CONTROL_H_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _CONTROL_MODULE_
#define  MY_EXT
#else
#define  MY_EXT  extern
#endif  

    void ControlTask(void const *argument);

#undef MY_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* LEDCTR_H */
