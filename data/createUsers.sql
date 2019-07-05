create procedure createUser()
begin
    set @no = 1;
    while @no <= 10000 do
        insert into user(nickname,password) values(concat('test',@no),md5('test'));
        set @no=@no+1;
    end while;
end;
call createUser;