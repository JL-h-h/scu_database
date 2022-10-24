select distinct ShipName , substr(ShipName,0,instr(ShipName,'-'))
from "Order" 
where ShipName like '%-%' 
order by ShipName;