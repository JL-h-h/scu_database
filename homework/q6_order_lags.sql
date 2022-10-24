select Id,OrderDate,lag(OrderDate,1,OrderDate) over (order by OrderDate),
      round(julianday(OrderDate)-julianday(lag(OrderDate,1,OrderDate) over (order by OrderDate)),2)
from "Order" 
where CustomerId='BLONP' 
order by OrderDate asc 
limit 0,10;