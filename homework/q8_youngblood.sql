select distinct RegionDescription,FirstName,LastName,BirthDate
from (select Region.Id,RegionDescription,FirstName,LastName,max(BirthDate),BirthDate 
      from Employee inner join EmployeeTerritory on Employee.Id=EmployeeTerritory.EmployeeId
inner join Territory on EmployeeTerritory.TerritoryId=Territory.Id
inner join Region on Territory.RegionId=Region.Id group by RegionDescription)
order by Id