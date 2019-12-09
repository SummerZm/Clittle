## <b>C语言如何正确的数组传参</b> ##
> 正确的方式简洁明了，而错误的原因总是千奇百怪，我们应该立足于相对正确的基础上学习。

### <b>两种数组传参场景</b> ###
-  场景一：定长数组传参
    ```C
    // OK
     void func(int hash[4]) 
     {
         printf("hash: %d\n", hash[1]);
        ......   
     }
    ```
-  场景二： 不定长数组传参
    ```C
    // OK
    void func(int** hash, int len)
    {
        // 强制转换，将int**指针的引用域(强转前大小为入参所占内存的大小)，缩小至int大小
        int *table = (int*)hash;
        printf("hash: %d\n",  table[1]);
        .......
    }
     ```

 ### <b>常见错误数组传参</b> ###

>  数组传参错误导致问题本质：引用域大小不正确，进而索引访问越界溢出

- 不规范写法
        
    ```C
        // Error
        int hash[4];
        // 编译会有警告，函数实现若没有强转，会段错误。
        void func1 (int* hash[]);
        void func2 (int* hash[4])
    ```

- 示例代码
    ```C
    #include<stdio.h>

    typedef struct _r {
        int height;
        int width;
    } R;

    R info[] = { {10,10}, {11,11} };

    void testfn2(R data[2] /*R** data  or R* data[]  or  R* data[2]*/) 
    {
        R* data1 = (R*)data;

        // error: printf("height: %d\n", data->height);  if argu is R** data,  without cast.
        // error: printf("height: %d\n", data[0]->height);

        printf("height: %d\n", data1->height);
        data1->height = 113;
        printf("height: %d\n", data1->height);

        printf("height: %d\n", (data1+1)->height);
        //error: printf("height: %d\n", data1[1]->height);
        return;
    }

    int main() {
        //printf("height: %d\n", info[0].height);
        //testfn2(&info);
        testfn2(info);
        return 0;
    }

    ```