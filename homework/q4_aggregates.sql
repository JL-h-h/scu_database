select CategoryName,
       count(*),
       round(avg(Product.UnitPrice),2),
       min(Product.UnitPrice),
       max(Product.UnitPrice),
       sum(UnitsOnOrder) from Product,Category
where Category.Id=Product.CategoryId 
group by CategoryId 
having count(*)>10 
order by CategoryId;