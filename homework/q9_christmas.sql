select GROUP_CONCAT(ProductName)
from (select ProductName
      from Customer inner join 'Order' on Customer.Id="Order".CustomerId
                    inner join OrderDetail on "Order".Id=OrderDetail.OrderId
                    inner join Product on Product.Id=OrderDetail.ProductId
      where CompanyName='Queen Cozinha' and OrderDate like '2014-12-25%'
      order by Product.Id);