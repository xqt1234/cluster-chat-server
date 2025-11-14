create table user(id int unsigned primary key auto_increment,username varchar(50) not null,password varchar(64) not null);
create table friend(userid int unsigned not null,friendid int unsigned not null,primary key(userid,friendid));
create table allgroup(id int unsigned primary key auto_increment,ownerid int unsigned not null,groupname varchar(50) not null,groupdesc varchar(200));
create table groupuser(groupid int unsigned not null,userid int unsigned not null,role enum('owner','admin','normal') default 'normal',index idx_userid(userid),primary key(groupid,userid));
create table offlinemsg(id int unsigned primary key auto_increment,userid int unsigned not null,message varchar(500) not null,create_time datetime default current_timestamp,index idx_userid(userid));