#ifndef __CORE_WINDOWS_H
#define __CORE_WINDOWS_H

/* IO definitions (access restrictions to peripheral registers) */

#ifndef __clang__
  /* CLANG compiler generate error because __I and __O */
  #ifdef __cplusplus
    #define   __I     volatile             /*!< Defines 'read only' permissions */
  #else
    #define   __I     volatile const       /*!< Defines 'read only' permissions */
  #endif
  #define     __O     volatile             /*!< Defines 'write only' permissions */
#endif

#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */



//void NVIC_SystemReset(void);
#define NVIC_SystemReset()



#endif  /* #ifndef __CORE_WINDOWS_H */
