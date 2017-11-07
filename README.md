# pinocchio_test

Q1. 在GGPR的论文中，QSP的定义是针对boolean函数，即函数f只有一比特输出，那么对于一般的boolean电路，
怎么定义QSP呢？

Q2. 多项式应该如何表示呢？
A2. 首先，每个boolean gate对应的SP具有12个多项式，9个根，每个是8次多项式。第二，复杂的布尔电路对应的SP应该就有12*s个多项式，9*s个根，每个多项式是。

（n+1）个点就可以唯一确定一个n次多项式。
虽然，每个多项式的次数很高，但是其实他们都非常稀疏，也就是说，大部分的多项式在

Q3. QSP如何构造呢？
a). 首先对每个gate构造一个多项式版的SP；
b). 然后构造电路的checker func的SP；
c). 构造consistency checker；
d). CRT。

其中在第3步中也需要用CRT来处理多项式，而如果用SparsePolynomial方式来表示多项式的话，CRT就变得非常简单。

单个wire的consistency checker包含两组多项式，每个多项式由{(x, p(x))}来表示；尤其是，对于(x, p(x)=0)这种点，我们选择不记录。而且每个wire的checker中的目标多项式ti(x)一定是2*L'次 多项式，且其中的其他多项式是。
