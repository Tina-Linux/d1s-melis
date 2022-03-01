/***********************************************************************/
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


