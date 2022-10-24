select disN,CompanyName,ContactName
from (select ProductName as disN,min(OrderDate),CustomerId 
      from Product
         inner join OrderDetail as OD on Product.Id=OD.ProductId
         inner join "Order" as O on O.Id=OD.OrderId 
         where Discontinued=1 
         group by disN) as TMP
         inner join Customer as C on C.Id=TMP.CustomerId
order by disN