# How to implement Pinocchio?

## How to construct *QSP*

- Q1. 在GGPR的论文中，QSP的定义是针对boolean函数，即函数f只有一比特输出，那么对于一般的boolean电路，怎么定义QSP呢？

- Q2. 多项式应该如何表示呢？

    首先，每个boolean gate对应的SP具有12个多项式，9个根。
    
    第二，复杂的布尔电路对应的SP应该就有 *12s* 个多项式，*9s* 个根，每个多项式是 *9s-1* 次多项式。每个多项式用 *9s* 个点表示，即{(r_i, y_i): i = 1, ..., 9s }。但其实这些点大部分是(r_i, 0)，所以每个多项式其实只需要大约 **s** 个域元素来表示。

    第三，通过Lagrange插值法，可以将多项式从**稀疏表示** 转换成**系数表示**。

- Q3. QSP如何构造呢？
    - 每个gate构造一个多项式版的SP；
    - 构造电路的checker func的SP；
    - 构造consistency checker；
    - CRT。

   其中在第3步中也需要用CRT来处理多项式，而如果用SparsePolynomial方式来表示多项式的话，CRT就变得非常简单。

   单个wire的consistency checker包含两组多项式，每个多项式由{(x, p(x))}来表示；尤其是，对于(x, p(x)=0)这种点，我们选择不记录。而且每个wire的checker中的目标多项式ti(x)一定是2*L'次 多项式，且其中的其他多项式是。


- Q4. SparsePolynomial 如何计算在某一个点的取值？

   什么是[Lagrange插值法](https://en.wikipedia.org/wiki/Lagrange_polynomial)？
   根据Lagrange插值法，该多项式p其实等于sum(L_i(x)), 这里的L_i(x)就是所谓的Lagrange基多项式。

   假设一个n次多项式p, 给定其在n+1个不同点的取值，那么

- Q5 给定了一个输入，如何算出两个多项式v(x), w(x)?


## How to construct *QAP*
QAP的构造相对比较简单，核心在于如何把arithmetic circuit分解成若干个multi subcircuit的复合。

- 问题1. 怎么构造一个circuit呢？
