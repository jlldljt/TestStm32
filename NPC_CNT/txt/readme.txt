


main
		系统启动
				  main > start_task >
									  tmr1               定时器  串口接收判断并启动usart_recv_task
									  tmr2               定时器  脚本收的超时判断
									  usart_recv_task    线程    不同状态下的串口接收的解析动作
									  usart_send_task    线程    不同状态下的串口发送
									  cdv_refresh_task   线程    联机模式下刷新CDV资源与外设
									  worker_manage_task 线程    管理脱机模式不同状态下工人的运行
									  worker_task        线程    8个工人线程
user_define
        其他全局定义
		          
user_config
        stm32f4的初始化函数及外设基础操作函数封装
		
modbus
        modbus 全局定义
		          线圈、寄存器定义      【*扩充添加modbus协议定义，主要在modbus.h文件中*】
				  线程、寄存器操作      【*扩充modbus协议定义之后，最好定义该定义的操作*】
		modbus 基础解析函数和功能码函数 【*联机模式用，modbus线圈、寄存器的操作，不涉及具体的资源和外设的操作，具体由cdv_refresh_task负责*】
		
cdv_resource
        CDV资源相关函数
		          CDV资源初始化
				  CDV资源与外设的刷新
				  CDV资源操作
				  
cdv_offline
        CDV脱机运行相关函数
		          脚本解析
				  应用层协议解析
				  
				  关于上一步的解法，可以在切换工人的时候，利用一个函数创建pos队列(或者一边运行，一边存储队列)，这里列出了所有的运动操作对应的运动前的位置
				  
				  
				  
				  parse中的crc匹配放在外面进行
				  debugparse中调试上一步下一步可以改成通过modbus中的数据来刷新，避免按多次之后，序号处理不过来而对不上，序号保存在寄存器，同时也减少了模式的数量
				  √fpga io/motor复用的自动识别 incoil设置fpgacfg
				  √MOTOR INREG 设置目标地址
				  √传脚本统一起来，最好开关一样，就地址不一样，需要一个全局变量
				  √预置多寄存器
				  ×添加串口电缸脚本收
				  √添加串口料盘脚本收
				  √调试模式、联机模式合并
				  中断要加intenter或者会hardfault？数组越界会hardfault
				  中断模式变更
				  √wifi
				  √脱机、联机模式合并
				  变频器整合
				  √调试模式上一步，下一步改成发序号,且有一个标记指示该往上走还是往下走，通过计数停止之前的上一步，下一步cnt差值判定；每一步下位机回复执行成功还是失败供app处理
				  √从脚本中读还是放在内存中
				  ×调试每一步要初始化
				  在检测到错误的语句后使用while（1）并一直往串口扔错误信息
				  √modbus 联机调试增加错误反馈，指令执行成功，原样反馈；失败，发送错误吗，modbusrequest
				  增加一个退出函数，调用一下即可以回到online\wifi初始态。可以在任何模式下使用
				  ×只有APP才需要wifi，所以APP这块要集成到wifi指令分析中。与kfc并列
				  √at发送添加
				  wifi发送接收定长
				  操作数据区其实要加锁
				  wifi拔掉自动连
				  不同串口的命令接收到同一个函数处理，可以在串口命令串中定义一个字节用来指示命令来自哪一个串口
				  √一运行osinit就死掉，原来是usart3中断没加osintenter，导致的
				  ×case的enum可否换成char的位标记，这样就不会因为改变了enum标记要导致原先的标记正在干的事情无法继续，当然是固定流程的除外。因固定流程的肯定会干完当前事情再去执行别的case（各有各的用处
				  料盘需要加入初始化函数，每次脱机调试的上一般下一步之前必须进行走初始格，不然因下一格是一直往下累加的，模拟每一步都会走
				  串口接收发送队列加上该命令从哪个串口而来的标识号
				  变量不间断运行显示 ，导致指针内存错误，死机。原因未找到，现在在addtx中增加等待缓存有空的操作。出错条件是缓存满之后，依旧执行运行显示。不操作缓存，也会出错
				  √g_run数组最后一个当成调试工人专用GetWorkerScript要改，得bugjump要改
				  串口接受缓存另外增加一个，专门是存中断过来的。定时器这边将这个缓存复制到接收队列中
				  嵌套调用的函数，需要设定一个统一的par结构体作为形参，所有函数都包含这一形参，这样，当少了一个需要传输下去的参数时，可以在结构体中加一个就好了，不用为每个函数修改一遍
				  运行显示的资源显示长度有内存泄漏问题
				  接收发送改的有点多，要调一下
				  应用层交互的主从命令 设备号分开，以方便编程
				  
				  串口缓存，不用缓存，保存一条执行中的，和一条当前接收的，不缓存，当前接收的马上解析并扔掉，如果是系统指令，则执行。也就是串口线程中要执行系统指令
				  串口发送死等的要加计时
				  串口接收缓存    开辟数组     有两个指针     已处理指针   和    最新命令指针     中断接收并更新最新命令指针，已处理指针根据  最新命令指针是否与自己不一样，不一样就进行命令的提取和处理   处理的命令必须是有协议尾的 提取可以提取到一块new的内存中
				  发送根据是否要回复，如果需要回复的，串口接收到数据先交给回复确认函数
				  DMA传数据到内存，要使用DMA中断，即DMA一个流结束时把内存copy出来，或者如果DMA连续转换或没转换完，直接读到的值可能是被覆盖的值
				  debug 和 offline 解析执行里 的有关资源的操作 都放到 一个新的线程里做， 这样就不会因为资源未操作完成，导致下一次操作无法响应，当然如果下一次还是有关资源的操作，则丢失了
				  
				  在接受线程里面，attx在串口下工作正常，在wifi下工作不正常，原因是wifi还需要与wifi模块交互，而接受线程这时候被占用，无法处理
				  如果考虑串口接受一直往缓存里放，使用标记或事件通知外面，则必须一次置两次标记或事件，防止丢失
				  
				  关于flash丢失的问题，估计是在流程操作flash时重启造成的
				  
				  基础硬件初始化 -》 资源初始化（采用函数指针，每个资源的初始化函数参数，返回值一致,比如typedef int (*fun[])(char*,int) fun f = { } -> 用户初始化 -》 作业系统初始化（线程）
				  
				  RS485通信异常，换485芯片
				  
				  系统未启动时不能使用系统资源
				  
				  READ_CDV_DIP 这个相关的要改，先注释
				  
				  ADC MDA 配置须在flash读取之前不然会被干扰导致转换数值变得很大，超过4096.有可能是芯片本身的问题
				  
				  
				  考虑WorkerTaskTCB 放到g_threadInfo中
				  
				  OSTaskCreate如果函数已经结束，可以通过它来启动，如果del之后貌似也可以启动，但是由于是强制终止，如果这时候占用资源未清理，可以会有问题。重复调用会把局部变量，psp清0，导致线程函数执行异常