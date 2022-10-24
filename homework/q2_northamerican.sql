select Id,ShipCountry,
       iif(ShipCountry in ('USA','Canada','Mexico'),'NorthAmerica','OtherPlace') AS "NorthAmerica or OtherPlace"
from "Order" order by Id ASC 
limit 20 
offset (select (15445-min(Id)) from "Order");