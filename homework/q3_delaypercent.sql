select distinct Shipper.CompanyName, delayRate
from Shipper,
             (select lateOrder.ShipVia, round((lateOrder.late/cast(allOrder.cnt as double))*100,2) as delayRate
              from (select ShipVia,count(*) as late 
                    from 'Order' 
                    where ShippedDate > RequiredDate 
                    group by ShipVia) as lateOrder,
                   (select ShipVia,count(*) as cnt 
                    from 'Order' 
                    group by ShipVia) as allOrder
              where lateOrder.ShipVia = allOrder.ShipVia)
              as t
where Shipper.Id = t.ShipVia 
order by delayRate desc;