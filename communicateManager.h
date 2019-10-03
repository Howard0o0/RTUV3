/*
 * author   :   howard
 * date     :   2019/09/29
*/

#ifndef __COMMUNICATE_MANAGER_H
#define __COMMUNICATE_MANAGER_H

typedef struct communicateDev
{
    char *name;
    int (*isCanUse)(void);
    int (*init)(void);
    int (*getMsg)(char *msgRecv);
    int (*sendMsg)(char *msgSend);
    struct communicateDev *ptNext;
}T_CommunicateDev,*PT_CommunicateDev;


/******Interfaces for up class **************************/

/*!
 * @brief : an communicate device driver call this functin to register into Dev NodeList
 *
 * @param[in] ptCommunicateDev  : Structure instance of a specific device.
 *
 * @retval zero -> Success / other value -> Error.
 */
int RegisterCommunicateDev(PT_CommunicateDev ptCommunicateDev);

/*!
 * @brief : show communicate device available
 */
void ShowCommunicateDevs(void);

/*!
 * @brief : get a Communicate device 
 *
 * @param[in] devName  : communicate device name
 *
 * @retval NULL -> appointed device is not registed / other value -> address of device's handler.
 */
PT_CommunicateDev getCommunicateDev(char *devName);


#endif