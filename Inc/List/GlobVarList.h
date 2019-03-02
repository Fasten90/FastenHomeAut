/*
 *    GlobalVariables.h
 *    Created on:   2016-09-07
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Global Variable Handler
 *    Target:       STM32Fx
 */

#ifndef GLOBVARLIST_H_
#define GLOBVARLIST_H_

/**
 * \Note        Be careful! If this file has compile error, please check: GlobalVarHandler.h module need included before this (GlobVarList.h)
 */


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const GlobVarH_VarRecord_t GlobVarH_VarList[];
extern const GlobVarH_ID_t GlobVarH_MaxCommandNum;

extern GlobVarH_VarListInfo_t GlobVarH_Variables;



#endif /* GLOBVARLIST_H_ */
