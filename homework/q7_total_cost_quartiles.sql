select name,CustomerId,total
from (select *,ntile(4) over (order by total asc ) as mark
      from (select ifnull(CompanyName,'MISSING_NAME') as name,CustomerId,round(sum(UnitPrice*Quantity),2) as total
            from "Order" inner join OrderDetail on "Order".Id=OrderDetail.OrderId
                         left join Customer on "Order".CustomerId=Customer.Id
            group by CustomerId order by total))
where mark=1