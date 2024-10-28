#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <verilated.h>
#include <verilated_fst_c.h>
#include "Vtop.h"
int main(int argc,char** argv){

    VerilatedContext* contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    Vtop* top = new Vtop(contextp);//实例化模块
    VerilatedFstC* tfp = new VerilatedFstC;//初始化波形文件
    contextp->traceEverOn(true);//追踪波形
    top->trace(tfp, 99);//追踪波形
    tfp->open("simx.fst");//打开波形文件
    while (!contextp->gotFinish()) {
        int a = rand() & 1;
        int b = rand() & 1;
        top->a = a;
        top->b = b;
        top->eval(); //验证模型
        printf("a = %d, b = %d, f = %d\n", a, b, top->f);
        tfp->dump(contextp->time()); //波形文件,dump wave
        contextp->timeInc(1); //时间步进
        assert(top->f == (a ^ b));
    }
    top->final(); //模型销毁
    delete top;
    tfp->close();
    delete contextp;
    return 0;
}