/*
  * @brief   本错误处理仅作为集中状态处理，程序不支持多任务设定
*/

#ifndef ERR_HANDLE_H_H
#define ERR_HANDLE_H_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __ERR_HANDLE_MOUDLE__
#define ___EXT
#else
#define ___EXT extern
#endif

#define ERR_GROUP_LEN 8

  typedef struct
  {
    uint8_t b_global;                   //X外部读取
    uint8_t b_found_edge;               //X外部读取
    uint8_t b_lost_edge;                //X外部读取
    uint8_t b_disp_switch_pulse;        //Y外部设定，显示值切换脉冲
    uint16_t disp_now;                  //X外部读取，当前显示值
    uint16_t val[ERR_GROUP_LEN];        //XY当前值,外部设定
    uint16_t old[ERR_GROUP_LEN];        //X内部使用
    uint16_t found_edge[ERR_GROUP_LEN]; //X外部读取
    uint16_t lost_edge[ERR_GROUP_LEN];  //X外部读取
    uint16_t mask[ERR_GROUP_LEN];       //X外部设定
  } sys_err_type;

/*
  定义err常量规则
  bits[15:0]  - err flag
  bits[18:16] - err group
  */
  
#define sys_err_make(group, pos) ((group<<16)|(1UL << pos))
  
#define sys_err_set(obj, err) obj.val[err >> 16] |=  (err & 0xFFFF)
#define sys_err_clr(obj, err) obj.val[err >> 16] &= ~(err & 0xFFFF)
#define sys_err_msk(obj, msk) obj.mask[err >> 16] = msk & 0xFFFF

#define sys_err_get(obj, err) (obj.val[err >> 16] & (err & 0xFFFF))
#define sys_err_found_edge_get(obj, err) (obj.found_edge[err >> 16] & (err & 0xFFFF))
#define sys_err_lost_edge_get(obj, err) (obj.lost_edge[err >> 16] & (err & 0xFFFF))

  void err_init(sys_err_type *sys_err);
  void err_poll(sys_err_type *sys_err);

#ifdef __cplusplus
}
#endif

#endif /* ERR_HANDLE_H_H */
