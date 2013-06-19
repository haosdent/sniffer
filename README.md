网络流量监控工具
====================
分别使用Raw Socket和PF_Ring抓取数据包，对比其所造成的性能损失。

测试结果
-------

### 0.测试环境说明

2台物理机，由一台机压测另外一台机上的Nginx，每台物理机的信息如下：
* CPU：i5-3450(4核/3.1GHZ)
* 内存：32G(8G/1600*4)
* 硬盘：2TB(No Raid)
* 网卡：主板集成网卡，千兆
* 操作系统：Ubuntu 13.04(Linux 3.8.0-22-generic x86_64)
* Nginx版本：1.4.1

### 1.使用Web Bench进行测试

* Web Bench版本：1.5
* 测试命令如下：
```bash
webbench -c 2000 -t 60 http://slave1/index.html
```

* 测试结果（单元格中单位为pages/min | bytes/sec，例如1744527|24530372表示1744527pages/min |  24530372bytes/sec，）：
```
 +-------+----------------+----------------+----------------+
 |  No.  |      None      | sniffer_pfring | sniffer_socket |
 +-------+----------------+----------------+----------------+
 |   0   |1744527|24530372|1812358|25493636|1930610|27151190|
 +-------+----------------+----------------+----------------+
 |   1   |1501696|21114692|1541362|21677162|1846734|25977330|
 +-------+----------------+----------------+----------------+
 |   2   |1402686|19720820|1882342|26475308|2152540|30278958|
 +-------+----------------+----------------+----------------+
 |   3   |1362858|19162744|1098016|15444825|1746950|24573314|
 +-------+----------------+----------------+----------------+
 |   4   |1539255|21645556|1293544|18191904|2083660|29309606|
 +-------+----------------+----------------+----------------+
 |average|1510204|21234836|1525524|21456567|1952098|27458079|
 +-------+----------------+----------------+----------------+
```

* 结论：Web Bench测试结果表明raw socket的性能优于pf_ring，pf_ring优于空白对照。
### 2.使用ApacheBench进行测试

* ApacheBench版本：2.2
* 测试命令如下：
```bash
ab -n 100000 http://slave1/index.html
```

* 测试结果（单元格中单位为ms/page）：
```
+-------+-----+--------------+--------------+
|  No.  |None |sniffer_pfring|sniffer_socket|
+-------+-----+--------------+--------------+
|   0   |0.808|    3.607     |    8.377     |
+-------+-----+--------------+--------------+
|   1   |0.923|    3.510     |    7.863     |
+-------+-----+--------------+--------------+
|   2   |0.883|    3.773     |              |
+-------+-----+--------------+--------------+
|   3   |0.881|              |              |
+-------+-----+--------------+--------------+
|   4   |0.711|              |              |
+-------+-----+--------------+--------------+
|average|0.841|              |              |
+-------+-----+--------------+--------------+
```

* 结论：ApacheBench测试结果表明空白对照的性能优于pf_ring，pf_ring优于raw socket。

### 3.直接传输文件

* 文件大小：3.2GB
* 测试命令如下：
```bash
ab -n 100000 http://slave1/index.html
```

* 测试结果（单元格中单位为MB/s）：
```
+-------+-----+--------------+--------------+
|  No.  |None |sniffer_pfring|sniffer_socket|
+-------+-----+--------------+--------------+
|   0   |0.808|    3.607     |    8.377     |
+-------+-----+--------------+--------------+
|   1   |0.923|    3.510     |    7.863     |
+-------+-----+--------------+--------------+
|   2   |0.883|    3.773     |              |
+-------+-----+--------------+--------------+
|   3   |0.881|              |              |
+-------+-----+--------------+--------------+
|   4   |0.711|              |              |
+-------+-----+--------------+--------------+
|average|0.841|              |              |
+-------+-----+--------------+--------------+
```

* 结论：ApacheBench测试结果表明空白对照的性能优于pf_ring，pf_ring优于raw socket。


### 3.小结

测试结果感觉比较怪。