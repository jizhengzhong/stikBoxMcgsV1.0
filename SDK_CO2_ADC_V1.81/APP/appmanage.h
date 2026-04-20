
#ifndef APP_MANAGE_H
#define	APP_MANAGE_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _APP_MANAGE_MODULE_
#define  MY_EXT
#else
#define  MY_EXT  extern
#endif  

MY_EXT void Poweroff_mode(void);

#undef MY_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* APP_MANAGE_H */
