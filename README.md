# RGM-Order-Booking

http://rgmadvisors.com/problems/orderbook/

* Complexity for processing an Add Order message

    - Every Add Order information will be inserted into two tables.

        First: An unordered_map (hash table) "orderRec" to record order ID and its transaction (type, price, count). O(1)

        Second: An map (binary tree) "ask/bid PriceRec" to record price and it count. O(log n), n: size of PriceRec table.

    - If the # of shares is meet or exceed the target size of shares, we wil travere ask/bid PriceRec to calculate income/expence. O(n), n: size of PriceRec table.

    - The average time complexity should be: O(n)

* Complexity for processing an Reduce Order message

    - Every Reduce Order information will be used in two tables.

        First: Find this order ID's transaction information (type, price, count) from "orderRec" (hash table). O(1)

        Second: Use price found from first step to further update the information in ask/bid PriceRec (binary tree). O(log n), n: size of PriceRec table.

    - If the # of shares is meet or exceed the target size of shares, we wil travere ask/bid PriceRec to calculate income/expence. O(n), n: size of PriceRec table.

    - The average time complexity should be: O(n)

* Future Work

  - Currently, I read input data from file line by line, I think file I/O will be one of performance bottlenect in this version. If we have enough memory to store these input, the performance should be improved.

  - There are some functions in my program. E.g. AddOrder, ReduceOrder, printResult, etc. Currently, I just call them within nultiple function call. However, if we can utilized the feature of multithread, each function can be performed within different threads. I think the performance can be also improved.


