# 2PL_multithread
This is to implement the concept of 2PL using multithread in c++.
### What is 2PL?
2PL is **growing phase** and **shrinking phase**. It's a simple way to manage transactions.<br>
We use the concept of lock to prevent the data from being modified by more than one process/thread/user/...<br>
To do the concurrent control, we can use 2PL.<br>
In growing phase, we can lock and operate, while we can only unlock in shrinking phase.


---
If you want more info about my program, you can visit the "flow chart" folder.
---
*cpp_header version: 2020-6-10-21:02-UTC+8*
