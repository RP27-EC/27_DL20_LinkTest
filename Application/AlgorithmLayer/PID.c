#include "pid.h"
#include "rp_math.h"
/**
 *  @name   single_pid_ctrl
 */
void single_pid_ctrl(pid_ctrl_t *pid)
{
    // 保存误差值(需要在外面自行计算误差)
	//pid->err = pid->target-pid->measure;
	pid->integral += pid->err;  
    pid->integral = constrain(pid->integral, -pid->integral_max, +pid->integral_max);
    // p i d 输出项计算
    pid->pout = pid->kp * pid->err;
    pid->iout = pid->ki * pid->integral;
	pid->last_dout=pid->dout;
    // 累加pid输出值
    pid->out = pid->pout + pid->iout + pid->dout;
    pid->out = constrain(pid->out, -pid->out_max, pid->out_max);
    // 记录上次误差值
    pid->last_err = pid->err;
}

