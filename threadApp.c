#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/*
 * 程序清单：这是一个独立看门狗设备使用例程
 * 例程导出了 wdt_sample 命令到控制终端
 * 命令调用格式：wdt_sample wdt
 * 命令解释：命令第二个参数是要使用的看门狗设备名称，为空则使用例程默认的看门狗设备。
 * 程序功能：程序通过设备名称查找看门狗设备，然后初始化设备并设置看门狗设备溢出时间。
 *           然后设置空闲线程回调函数，在回调函数里会喂狗。
*/

#define WDT_DEVICE_NAME         "wdt"   // 看门狗设备的名称

static rt_device_t wdg_dev;             // 看门狗设备句柄

/* 空闲线程回调函数 */
static void idle_hook(void)
{
  rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
  rt_kprintf("feed watch dog!\n");
}

static int wdt_sample(int argc, char *argv[])
{
  rt_err_t ret = RT_EOK;
  rt_uint32_t timeout = 1;  // 看门狗溢出时间,单位s
  char name[RT_NAME_MAX];

  if (argc == 2)
  {
    rt_strncpy(name, argv[1], RT_NAME_MAX);
  }
  else
  {
    rt_strncpy(name, WDT_DEVICE_NAME, RT_NAME_MAX);
  }


  // 获取设备句柄
  wdg_dev = rt_device_find(name);
  if (!wdg_dev)
  {
    rt_kprintf("find %s failed!\n", name);
    return RT_ERROR;
  }

  // 设置看门狗溢出时间
  ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT,&timeout);
  if (ret != RT_EOK)
  {
    rt_kprintf("set %s timeout failed!\n", name);
    return RT_ERROR;
  }

  // 启动看门狗
  ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
  if (ret != RT_EOK)
  {
    rt_kprintf("start %s failed!\n", name);
    return RT_ERROR;
  }

  // 设置空闲线程回调函数
  rt_thread_idle_sethook(idle_hook);

  return ret;
}

MSH_CMD_EXPORT(wdt_sample, wdt sample);

