/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*  弹性滑动模拟算法，根据手势识别返回的speed值，其值只能是0-6之间的整数值
    调用    int speed_step(int speed, int height, int step_table[]) 函数

    函数说明：
    参数：int speed为手势识别返回的滑动速度值
          int height为整个应用时候的总体长度：比如96个像素
          int step_table[]数组需要用户开辟空间大小是92个int数据
    返回值：int
            返回的为算法填充的step_table[]的数组的长度。也就是用户需要用到的数组的长度
            比如：92、80……
*/
/***********************************************************************/
static int table[7][3] =
{
    {0, 0, 0},
    {1, 1, 1},
    {1, 2, 1},
    {4, 3, 1},
    {5, 4, 2},
    {6, 5, 2},
    {8, 6, 2}
};

int step_table[128];

int speed_step(int speed, int height, int step_table[])
{
    int n;
    int i, j;
    int coef[3] = {5, 8, 16};

    if (speed == 0)
    {
        return 0;
    }

    n = 0;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < table[speed][i]*coef[i]; j++)
        {
            step_table[n + j] = height / coef[i];
        }

        n += table[speed][i] * coef[i];
    }

    return n;
}

int speed_step2(int speed, int height, int step_table[])
{
    int n;
    int i, j;

    if (speed == 0)
    {
        return 0;
    }

    n = 0;

    for (i = 0; i < 2 * speed; i++)
    {
        step_table[i] = height;
        n++;
    }

    for (i = 0 ; i < speed * height / 10 ; i++)
    {
        step_table[n + i] = 10;
    }

    n +=  speed * height / 10;

    for (j = 0; j < speed * height - 10 * i; j++)
    {
        step_table[n + j] = 1;
    }

    n += speed * height - 10 * i;
    return n;
}
