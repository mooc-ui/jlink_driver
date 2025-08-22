using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace hxTestTool
{
    struct complex//定义复数  
    {

        //复数a+bi中  
        //a为实部，b为虚部  
        public double a;
        public double b;
        //实数和复数相乘  

        public static complex commul(double x, complex y)
        {
            complex c = new complex();
            c.a = x * y.a;
            c.b = x * y.b;

            return c;
        }
        //复数和复数相乘  重载上一个函数  
        public static complex commul(complex x, complex y)
        {
            complex c = new complex();
            c.a = x.a * y.a - x.b * y.b;
            c.b = x.a * y.b + x.b * y.a;

            return c;
        }
        //复数和复数相加  
        public static complex comsum(complex x, complex y)
        {
            complex c = new complex();
            c.a = x.a + y.a;
            c.b = x.b + y.b;

            return c;
        }
        //实数和复数相加  
        public static complex comsum(double x, complex y)
        {
            complex c = new complex();
            c.a = x + y.a;
            c.b = y.b;

            return c;
        }
        //复数和复数相减  

        public static complex decrease(complex x, complex y)
        {
            complex c = new complex();

            c.a = x.a - y.a;
            c.b = x.b - y.b;


            return c;
        }

        public static complex decrease(double x, complex y)
        {
            complex c = new complex();

            c.a = x - y.a;
            c.b = 0 - y.b;


            return c;
        }
        //复数的递归相乘求复数的N次方  
        public static complex powcc(complex x, double n)
        {
            int k;
            complex xout;
            xout.a = 1;
            xout.b = 0;
            for (k = 1; k <= n; k++)
            {
                xout = complex.commul(xout, x);
            }
            return xout;
        }
        public void show()
        {
            Console.Write(a + " + " + b + "i      ");
        }
    }
    class FFT
    {
        //计算ω=exp（j*2*pi/n）n为信号长度  
        public static complex omega(int n)
        {
            complex x;
            x.a = Math.Cos(0 - 2 * Math.PI / n);
            x.b = Math.Sin(0 - 2 * Math.PI / n);
            return x;
        }
        //计算DFT  
        public static complex[] dft(double[] signal, int n)  //(信号，信号长度)  
        {
            int i, j;
            complex w1;
            w1 = omega(n);//ω=exp（j*2*pi/n）n为信号长度  
            complex[] w = new complex[n];//储存w N次方的数组  
            for (i = 0; i < n; i++)
            {
                w[i] = complex.powcc(w1, i);
            }
            complex[] f = new complex[n];
            complex temp;  //w[i]的次方  
            complex temp1; //f中单项的值  
            for (i = 0; i < n; i++)
            {
                f[i].a = 0;
                f[i].b = 0;
                for (j = 0; j < n; j++)
                {
                    temp = complex.powcc(w[i], j);
                    temp1 = complex.commul(signal[j], temp);
                    f[i] = complex.comsum(f[i], temp1);
                    f[i].a = Math.Round(f[i].a, 2);
                    f[i].b = Math.Round(f[i].b, 2);
                }
            }

            return f;
        }
        //实序列的FFT  
        public static complex[] fft(double[] input, int n)
        {


            ///输入序列只有一个元素，输出这个元素并返回  


            ///输入序列的长度  


            int length = n;






            ///输入序列的长度的一半  


            int half = length / 2;






            ///有输入序列的长度确定输出序列的长度  


            complex[] output = new complex[length];


            ///序列中下标为偶数的点  


            double[] evens = new double[half];


            for (int i = 0; i < half; i++)
            {


                evens[i] = input[2 * i];


            }






            //偶数列的DFT  
            complex[] evenResult = dft(evens, half);






            //序列中下标为奇数的点  


            double[] odds = new double[half];






            for (int i = 0; i < half; i++)
            {


                odds[i] = input[2 * i + 1];


            }






            //奇数列的DFT  


            complex[] oddResult = dft(odds, half);




            complex w1;
            w1 = omega(n);//ω=exp（j*2*pi/n）n为信号长度  
            complex[] w = new complex[n];//储存w N次方的数组  
            for (int i = 0; i < n; i++)
            {
                w[i] = complex.powcc(w1, i);
            }
            for (int k = 0; k < half; k++)
            {




                ///进行蝶形运算  


                complex oddPart = complex.commul(oddResult[k], w[k]);
                output[k] = complex.comsum(evenResult[k], oddPart);
                output[k].a = Math.Round(output[k].a, 2);
                output[k].b = Math.Round(output[k].b, 2);






                output[k + half] = complex.decrease(evenResult[k], oddPart);
                output[k + half].a = Math.Round(output[k + half].a, 2);
                output[k + half].b = Math.Round(output[k + half].b, 2);




            }






            ///返回FFT或IFFT的结果  


            return output;


        }
        

    }
}
