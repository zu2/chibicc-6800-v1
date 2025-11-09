#

     $0000
   ↑ Low address
|-----------------------| ← \_\_alloca\_bottom\_\_
| Dynamic allocations   | (alloca)
|-----------------------|
| VLA storage area      | ← Actual VLA array data
|-----------------------| ← @bp (current base pointer)
| Local variables       |
|-----------------------|
|  VLA Pointer (void\*)  | ← Pointer to VLA (local variable)
|-----------------------|
|  VLA Size (size\_t)   | ← elements × type\_size (local variable)
|-----------------------|
|  \_\_alloca\_\_bottom\_\_   | ← heap top address (local variable)
|-----------------------|
| Function argument     | (via register)
|-----------------------|
| Old @bp               |
|-----------------------|
| Return address        |
|-----------------------|
| Function arguments    | (via stack)
|-----------------------| 
   ↓ High address
     $FFFF


VLAが二次元以上((p\[x\]\[y\])の場合、yのサイズとx*yのサイズをローカル変数として記憶する。サイズはsizeof(p[0][0])*y と sizeof(p[0][0])*x*y が入る
