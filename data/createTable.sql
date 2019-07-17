create database if not exists lightim default character set utf8mb4;

use lightim;

create table if not exists user(
user_id int  unsigned not null auto_increment comment '自增ID',
nickname varchar(32) not null comment '用户昵称',
password char(32) not null comment '用户密码',
lastRecvMsgId bigint unsigned not null default 0 comment '最后一次收到的msgid',
primary key (user_id)
)engine=innodb default charset=utf8mb4;

create table if not exists friends(
id bigint unsigned not null auto_increment,
id1 int unsigned not null,
id2 int unsigned not null,
primary key(id),
foreign key(id1) references user(user_id),
foreign key(id2) references user(user_id)
)engine=innodb default charset=utf8mb4;

create table if not exists msg(
msg_id bigint unsigned not null auto_increment comment '自增ID',
from_id int unsigned not null comment '发送者id',
to_id int unsigned not null comment '接收者id',
msg_type char(32) not null comment '消息类型',
content blob not null comment '内容',
primary key(msg_id),
foreign key(from_id) references user(user_id),
foreign key(to_id) references user(user_id)
)engine =innodb default charset=utf8mb4;

create table if not exists send_msg(
id bigint unsigned not null auto_increment comment '自增ID',
to_id int unsigned not null comment '接收者id',
content blob not null comment '内容',
primary key(id),
foreign key(to_id) references user(user_id)
)engine =innodb default charset=utf8mb4;
